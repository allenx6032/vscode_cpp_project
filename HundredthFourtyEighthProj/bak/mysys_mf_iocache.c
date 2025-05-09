/* Copyright (C) 2000 MySQL AB

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; version 2 of the License.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

/*
  Cashing of files with only does (sequential) read or writes of fixed-
  length records. A read isn't allowed to go over file-length. A read is ok
  if it ends at file-length and next read can try to read after file-length
  (and get a EOF-error).
  Possibly use of asyncronic io.
  macros for read and writes for faster io.
  Used instead of FILE when reading or writing whole files.
  This code makes mf_rec_cache obsolete (currently only used by ISAM)
  One can change info->pos_in_file to a higher value to skip bytes in file if
  also info->read_pos is set to info->read_end.
  If called through open_cached_file(), then the temporary file will
  only be created if a write exeeds the file buffer or if one calls
  my_b_flush_io_cache().

  If one uses SEQ_READ_APPEND, then two buffers are allocated, one for
  reading and another for writing.  Reads are first done from disk and
  then done from the write buffer.  This is an efficient way to read
  from a log file when one is writing to it at the same time.
  For this to work, the file has to be opened in append mode!
  Note that when one uses SEQ_READ_APPEND, one MUST write using
  my_b_append !  This is needed because we need to lock the mutex
  every time we access the write buffer.

TODO:
  When one SEQ_READ_APPEND and we are reading and writing at the same time,
  each time the write buffer gets full and it's written to disk, we will
  always do a disk read to read a part of the buffer from disk to the
  read buffer.
  This should be fixed so that when we do a my_b_flush_io_cache() and
  we have been reading the write buffer, we should transfer the rest of the
  write buffer to the read buffer before we start to reuse it.
*/

#define MAP_TO_USE_RAID
#include "mysys_priv.h"
#include <libmysql_m_string.h>
#ifdef HAVE_AIOWAIT
#include "libmysql_mysys_err.h"
static void my_aiowait(my_aio_result *result);
#endif
#include <errno.h>

#ifdef THREAD
#define lock_append_buffer(info) \
 pthread_mutex_lock(&(info)->append_buffer_lock)
#define unlock_append_buffer(info) \
 pthread_mutex_unlock(&(info)->append_buffer_lock)
#else
#define lock_append_buffer(info)
#define unlock_append_buffer(info)
#endif

#define IO_ROUND_UP(X) (((X)+IO_SIZE-1) & ~(IO_SIZE-1))
#define IO_ROUND_DN(X) ( (X)            & ~(IO_SIZE-1))

/*
  Setup internal pointers inside IO_CACHE

  SYNOPSIS
    setup_io_cache()
    info		IO_CACHE handler

  NOTES
    This is called on automaticly on init or reinit of IO_CACHE
    It must be called externally if one moves or copies an IO_CACHE
    object.
*/

void setup_io_cache(IO_CACHE* info)
{
  /* Ensure that my_b_tell() and my_b_bytes_in_cache works */
  if (info->type == WRITE_CACHE)
  {
    info->current_pos= &info->write_pos;
    info->current_end= &info->write_end;
  }
  else
  {
    info->current_pos= &info->read_pos;
    info->current_end= &info->read_end;
  }
}


static void
init_functions(IO_CACHE* info)
{
  enum cache_type type= info->type;
  switch (type) {
  case READ_NET:
    /*
      Must be initialized by the caller. The problem is that
      _my_b_net_read has to be defined in sql directory because of
      the dependency on THD, and therefore cannot be visible to
      programs that link against mysys but know nothing about THD, such
      as myisamchk
    */
    break;
  case SEQ_READ_APPEND:
    info->read_function = _my_b_seq_read;
    info->write_function = 0;			/* Force a core if used */
    break;
  default:
    info->read_function =
#ifdef THREAD
                          info->share ? _my_b_read_r :
#endif
                                        _my_b_read;
    info->write_function = _my_b_write;
  }

  setup_io_cache(info);
}


/* FUNCTIONS TO SET UP OR RESET A CACHE */


/*
  Initialize an IO_CACHE object

  SYNOPSOS
    init_io_cache()
    info		cache handler to initialize
    file		File that should be associated to to the handler
			If == -1 then real_open_cached_file()
			will be called when it's time to open file.
    cachesize		Size of buffer to allocate for read/write
			If == 0 then use my_default_record_cache_size
    type		Type of cache
    seek_offset		Where cache should start reading/writing
    use_async_io	Set to 1 of we should use async_io (if avaiable)
    cache_myflags	Bitmap of differnt flags
			MY_WME | MY_FAE | MY_NABP | MY_FNABP |
			MY_DONT_CHECK_FILESIZE

  RETURN
    0  ok
    #  error
*/

int init_io_cache(IO_CACHE *info, File file, size_t cachesize,
		  enum cache_type type, my_off_t seek_offset,
		  pbool use_async_io, myf cache_myflags)
{
  size_t min_cache;
  my_off_t pos;
  my_off_t end_of_file= ~(my_off_t) 0;
  DBUG_ENTER("init_io_cache");
  DBUG_PRINT("enter",("cache: %p  type: %d  pos: %ld",
		      info, (int) type, (ulong) seek_offset));

  info->file= file;
  info->type= TYPE_NOT_SET;	    /* Don't set it until mutex are created */
  info->pos_in_file= seek_offset;
  info->pre_close= info->pre_read= info->post_read= info->post_write= NULL;
  info->arg = 0;
  info->alloced_buffer = 0;
  info->buffer=0;
  info->seek_not_done= 0;

  if (file >= 0)
  {
    pos= my_tell(file, MYF(0));
    if ((pos == (my_off_t) -1) && (my_errno == ESPIPE))
    {
      /*
         This kind of object doesn't support seek() or tell(). Don't set a
         flag that will make us again try to seek() later and fail.
      */
      info->seek_not_done= 0;
      /*
        Additionally, if we're supposed to start somewhere other than the
        the beginning of whatever this file is, then somebody made a bad
        assumption.
      */
      DBUG_ASSERT(seek_offset == 0);
    }
    else
      info->seek_not_done= test(seek_offset != pos);
  }

  info->disk_writes= 0;
#ifdef THREAD
  info->share=0;
#endif

  if (!cachesize && !(cachesize= my_default_record_cache_size))
    DBUG_RETURN(1);				/* No cache requested */
  min_cache=use_async_io ? IO_SIZE*4 : IO_SIZE*2;
  if (type == READ_CACHE || type == SEQ_READ_APPEND)
  {						/* Assume file isn't growing */
    if (!(cache_myflags & MY_DONT_CHECK_FILESIZE))
    {
      /* Calculate end of file to avoid allocating oversized buffers */
      end_of_file=my_seek(file,0L,MY_SEEK_END,MYF(0));
      /* Need to reset seek_not_done now that we just did a seek. */
      info->seek_not_done= end_of_file == seek_offset ? 0 : 1;
      if (end_of_file < seek_offset)
	end_of_file=seek_offset;
      /* Trim cache size if the file is very small */
      if ((my_off_t) cachesize > end_of_file-seek_offset+IO_SIZE*2-1)
      {
	cachesize= (size_t) (end_of_file-seek_offset)+IO_SIZE*2-1;
	use_async_io=0;				/* No need to use async */
      }
    }
  }
  cache_myflags &= ~MY_DONT_CHECK_FILESIZE;
  if (type != READ_NET && type != WRITE_NET)
  {
    /* Retry allocating memory in smaller blocks until we get one */
    cachesize= ((cachesize + min_cache-1) & ~(min_cache-1));
    for (;;)
    {
      size_t buffer_block;
      if (cachesize < min_cache)
	cachesize = min_cache;
      buffer_block= cachesize;
      if (type == SEQ_READ_APPEND)
	buffer_block *= 2;
      if ((info->buffer=
	   (uchar*) my_malloc(buffer_block,
			     MYF((cache_myflags & ~ MY_WME) |
				 (cachesize == min_cache ? MY_WME : 0)))) != 0)
      {
	info->write_buffer=info->buffer;
	if (type == SEQ_READ_APPEND)
	  info->write_buffer = info->buffer + cachesize;
	info->alloced_buffer=1;
	break;					/* Enough memory found */
      }
      if (cachesize == min_cache)
	DBUG_RETURN(2);				/* Can't alloc cache */
      /* Try with less memory */
      cachesize= (cachesize*3/4 & ~(min_cache-1));
    }
  }

  DBUG_PRINT("info",("init_io_cache: cachesize = %lu", (ulong) cachesize));
  info->read_length=info->buffer_length=cachesize;
  info->myflags=cache_myflags & ~(MY_NABP | MY_FNABP);
  info->request_pos= info->read_pos= info->write_pos = info->buffer;
  if (type == SEQ_READ_APPEND)
  {
    info->append_read_pos = info->write_pos = info->write_buffer;
    info->write_end = info->write_buffer + info->buffer_length;
#ifdef THREAD
    pthread_mutex_init(&info->append_buffer_lock,MY_MUTEX_INIT_FAST);
#endif
  }
#if defined(SAFE_MUTEX) && defined(THREAD)
  else
  {
    /* Clear mutex so that safe_mutex will notice that it's not initialized */
    bzero((char*) &info->append_buffer_lock, sizeof(info));
  }
#endif

  if (type == WRITE_CACHE)
    info->write_end=
      info->buffer+info->buffer_length- (seek_offset & (IO_SIZE-1));
  else
    info->read_end=info->buffer;		/* Nothing in cache */

  /* End_of_file may be changed by user later */
  info->end_of_file= end_of_file;
  info->error= info->hard_write_error_in_the_past= 0;
  info->type= type;
  init_functions(info);
#ifdef HAVE_AIOWAIT
  if (use_async_io && ! my_disable_async_io)
  {
    DBUG_PRINT("info",("Using async io"));
    info->read_length/=2;
    info->read_function=_my_b_async_read;
  }
  info->inited=info->aio_result.pending=0;
#endif
  DBUG_RETURN(0);
}						/* init_io_cache */

	/* Wait until current request is ready */

#ifdef HAVE_AIOWAIT
static void my_aiowait(my_aio_result *result)
{
  if (result->pending)
  {
    struct aio_result_t *tmp;
    for (;;)
    {
      if ((int) (tmp=aiowait((struct timeval *) 0)) == -1)
      {
	if (errno == EINTR)
	  continue;
	DBUG_PRINT("error",("No aio request, error: %d",errno));
	result->pending=0;			/* Assume everythings is ok */
	break;
      }
      ((my_aio_result*) tmp)->pending=0;
      if ((my_aio_result*) tmp == result)
	break;
    }
  }
  return;
}
#endif


/*
  Use this to reset cache to re-start reading or to change the type
  between READ_CACHE <-> WRITE_CACHE
  If we are doing a reinit of a cache where we have the start of the file
  in the cache, we are reusing this memory without flushing it to disk.
*/

my_bool reinit_io_cache(IO_CACHE *info, enum cache_type type,
			my_off_t seek_offset,
			pbool use_async_io __attribute__((unused)),
			pbool clear_cache)
{
  DBUG_ENTER("reinit_io_cache");
  DBUG_PRINT("enter",("cache: %p type: %d  seek_offset: %lu  clear_cache: %d",
		      info, type, (ulong) seek_offset,
		      (int) clear_cache));

  /* One can't do reinit with the following types */
  DBUG_ASSERT(type != READ_NET && info->type != READ_NET &&
	      type != WRITE_NET && info->type != WRITE_NET &&
	      type != SEQ_READ_APPEND && info->type != SEQ_READ_APPEND);

  /* If the whole file is in memory, avoid flushing to disk */
  if (! clear_cache &&
      seek_offset >= info->pos_in_file &&
      seek_offset <= my_b_tell(info))
  {
    /* Reuse current buffer without flushing it to disk */
    uchar *pos;
    if (info->type == WRITE_CACHE && type == READ_CACHE)
    {
      info->read_end=info->write_pos;
      info->end_of_file=my_b_tell(info);
      /*
        Trigger a new seek only if we have a valid
        file handle.
      */
      info->seek_not_done= (info->file != -1);
    }
    else if (type == WRITE_CACHE)
    {
      if (info->type == READ_CACHE)
      {
	info->write_end=info->write_buffer+info->buffer_length;
	info->seek_not_done=1;
      }
      info->end_of_file = ~(my_off_t) 0;
    }
    pos=info->request_pos+(seek_offset-info->pos_in_file);
    if (type == WRITE_CACHE)
      info->write_pos=pos;
    else
      info->read_pos= pos;
#ifdef HAVE_AIOWAIT
    my_aiowait(&info->aio_result);		/* Wait for outstanding req */
#endif
  }
  else
  {
    /*
      If we change from WRITE_CACHE to READ_CACHE, assume that everything
      after the current positions should be ignored
    */
    if (info->type == WRITE_CACHE && type == READ_CACHE)
      info->end_of_file=my_b_tell(info);
    /* flush cache if we want to reuse it */
    if (!clear_cache && my_b_flush_io_cache(info,1))
      DBUG_RETURN(1);
    info->pos_in_file=seek_offset;
    /* Better to do always do a seek */
    info->seek_not_done=1;
    info->request_pos=info->read_pos=info->write_pos=info->buffer;
    if (type == READ_CACHE)
    {
      info->read_end=info->buffer;		/* Nothing in cache */
    }
    else
    {
      info->write_end=(info->buffer + info->buffer_length -
		       (seek_offset & (IO_SIZE-1)));
      info->end_of_file= ~(my_off_t) 0;
    }
  }
  info->type=type;
  info->error= info->hard_write_error_in_the_past= 0;
  init_functions(info);

#ifdef HAVE_AIOWAIT
  if (use_async_io && ! my_disable_async_io &&
      ((ulong) info->buffer_length <
       (ulong) (info->end_of_file - seek_offset)))
  {
    info->read_length=info->buffer_length/2;
    info->read_function=_my_b_async_read;
  }
  info->inited=0;
#endif
  DBUG_RETURN(0);
} /* reinit_io_cache */


/* FUNCTIONS TO DO READS FROM THE CACHE */


/*
  Read buffered.

  SYNOPSIS
    _my_b_read()
      info                      IO_CACHE pointer
      Buffer                    Buffer to retrieve count bytes from file
      Count                     Number of bytes to read into Buffer

  NOTE
    This function is only called from the my_b_read() macro when there
    isn't enough characters in the buffer to satisfy the request.

  WARNING

    When changing this function, be careful with handling file offsets
    (end-of_file, pos_in_file). Do not cast them to possibly smaller
    types than my_off_t unless you can be sure that their value fits.
    Same applies to differences of file offsets.

    When changing this function, check _my_b_read_r(). It might need the
    same change.

  RETURN
    0      we succeeded in reading all data
    1      Error: can't read requested characters
*/

int _my_b_read(register IO_CACHE *info, uchar *Buffer, size_t Count)
{
  size_t length,diff_length,left_length, max_length;
  my_off_t pos_in_file;
  DBUG_ENTER("_my_b_read");

  if ((left_length= (size_t) (info->read_end-info->read_pos)))
  {
    DBUG_ASSERT(Count >= left_length);	/* User is not using my_b_read() */
    memcpy(Buffer,info->read_pos, left_length);
    Buffer+=left_length;
    Count-=left_length;
  }

  /* pos_in_file always point on where info->buffer was read */
  pos_in_file=info->pos_in_file+ (size_t) (info->read_end - info->buffer);

  /* 
    Whenever a function which operates on IO_CACHE flushes/writes
    some part of the IO_CACHE to disk it will set the property
    "seek_not_done" to indicate this to other functions operating
    on the IO_CACHE.
  */
  if (info->seek_not_done)
  {
    if ((my_seek(info->file,pos_in_file,MY_SEEK_SET,MYF(0)) 
        != MY_FILEPOS_ERROR))
    {
      /* No error, reset seek_not_done flag. */
      info->seek_not_done= 0;
    }
    else
    {
      /*
        If the seek failed and the error number is ESPIPE, it is because
        info->file is a pipe or socket or FIFO.  We never should have tried
        to seek on that.  See Bugs#25807 and #22828 for more info.
      */
      DBUG_ASSERT(my_errno != ESPIPE);
      info->error= -1;
      DBUG_RETURN(1);
    }
  }

  diff_length= (size_t) (pos_in_file & (IO_SIZE-1));
  if (Count >= (size_t) (IO_SIZE+(IO_SIZE-diff_length)))
  {					/* Fill first intern buffer */
    size_t read_length;
    if (info->end_of_file <= pos_in_file)
    {					/* End of file */
      info->error= (int) left_length;
      DBUG_RETURN(1);
    }
    length=(Count & (size_t) ~(IO_SIZE-1))-diff_length;
    if ((read_length= my_read(info->file,Buffer, length, info->myflags))
	!= length)
    {
      info->error= (read_length == (size_t) -1 ? -1 :
		    (int) (read_length+left_length));
      DBUG_RETURN(1);
    }
    Count-=length;
    Buffer+=length;
    pos_in_file+=length;
    left_length+=length;
    diff_length=0;
  }

  max_length= info->read_length-diff_length;
  if (info->type != READ_FIFO &&
      max_length > (info->end_of_file - pos_in_file))
    max_length= (size_t) (info->end_of_file - pos_in_file);
  if (!max_length)
  {
    if (Count)
    {
      info->error= (int) left_length; /* We only got this many char */
      DBUG_RETURN(1);
    }
    length=0;				/* Didn't read any chars */
  }
  else if ((length= my_read(info->file,info->buffer, max_length,
                            info->myflags)) < Count ||
	   length == (size_t) -1)
  {
    if (length != (size_t) -1)
      memcpy(Buffer, info->buffer, length);
    info->pos_in_file= pos_in_file;
    info->error= length == (size_t) -1 ? -1 : (int) (length+left_length);
    info->read_pos=info->read_end=info->buffer;
    DBUG_RETURN(1);
  }
  info->read_pos=info->buffer+Count;
  info->read_end=info->buffer+length;
  info->pos_in_file=pos_in_file;
  memcpy(Buffer, info->buffer, Count);
  DBUG_RETURN(0);
}


#ifdef THREAD
/*
  Prepare IO_CACHE for shared use.

  SYNOPSIS
    init_io_cache_share()
      read_cache                A read cache. This will be copied for
                                every thread after setup.
      cshare                    The share.
      write_cache               If non-NULL a write cache that is to be
                                synchronized with the read caches.
      num_threads               Number of threads sharing the cache
                                including the write thread if any.

  DESCRIPTION

    The shared cache is used so: One IO_CACHE is initialized with
    init_io_cache(). This includes the allocation of a buffer. Then a
    share is allocated and init_io_cache_share() is called with the io
    cache and the share. Then the io cache is copied for each thread. So
    every thread has its own copy of IO_CACHE. But the allocated buffer
    is shared because cache->buffer is the same for all caches.

    One thread reads data from the file into the buffer. All threads
    read from the buffer, but every thread maintains its own set of
    pointers into the buffer. When all threads have used up the buffer
    contents, one of the threads reads the next block of data into the
    buffer. To accomplish this, each thread enters the cache lock before
    accessing the buffer. They wait in lock_io_cache() until all threads
    joined the lock. The last thread entering the lock is in charge of
    reading from file to buffer. It wakes all threads when done.

    Synchronizing a write cache to the read caches works so: Whenever
    the write buffer needs a flush, the write thread enters the lock and
    waits for all other threads to enter the lock too. They do this when
    they have used up the read buffer. When all threads are in the lock,
    the write thread copies the write buffer to the read buffer and
    wakes all threads.

    share->running_threads is the number of threads not being in the
    cache lock. When entering lock_io_cache() the number is decreased.
    When the thread that fills the buffer enters unlock_io_cache() the
    number is reset to the number of threads. The condition
    running_threads == 0 means that all threads are in the lock. Bumping
    up the number to the full count is non-intuitive. But increasing the
    number by one for each thread that leaves the lock could lead to a
    solo run of one thread. The last thread to join a lock reads from
    file to buffer, wakes the other threads, processes the data in the
    cache and enters the lock again. If no other thread left the lock
    meanwhile, it would think it's the last one again and read the next
    block...

    The share has copies of 'error', 'buffer', 'read_end', and
    'pos_in_file' from the thread that filled the buffer. We may not be
    able to access this information directly from its cache because the
    thread may be removed from the share before the variables could be
    copied by all other threads. Or, if a write buffer is synchronized,
    it would change its 'pos_in_file' after waking the other threads,
    possibly before they could copy its value.

    However, the 'buffer' variable in the share is for a synchronized
    write cache. It needs to know where to put the data. Otherwise it
    would need access to the read cache of one of the threads that is
    not yet removed from the share.

  RETURN
    void
*/

void init_io_cache_share(IO_CACHE *read_cache, IO_CACHE_SHARE *cshare,
                         IO_CACHE *write_cache, uint num_threads)
{
  DBUG_ENTER("init_io_cache_share");
  DBUG_PRINT("io_cache_share", ("read_cache: %p  share: %p  "
                                "write_cache: %p  threads: %u",
                                read_cache, cshare,
                                write_cache, num_threads));

  DBUG_ASSERT(num_threads > 1);
  DBUG_ASSERT(read_cache->type == READ_CACHE);
  DBUG_ASSERT(!write_cache || (write_cache->type == WRITE_CACHE));

  pthread_mutex_init(&cshare->mutex, MY_MUTEX_INIT_FAST);
  pthread_cond_init(&cshare->cond, 0);
  pthread_cond_init(&cshare->cond_writer, 0);

  cshare->running_threads= num_threads;
  cshare->total_threads=   num_threads;
  cshare->error=           0;    /* Initialize. */
  cshare->buffer=          read_cache->buffer;
  cshare->read_end=        NULL; /* See function comment of lock_io_cache(). */
  cshare->pos_in_file=     0;    /* See function comment of lock_io_cache(). */
  cshare->source_cache=    write_cache; /* Can be NULL. */

  read_cache->share=         cshare;
  read_cache->read_function= _my_b_read_r;
  read_cache->current_pos=   NULL;
  read_cache->current_end=   NULL;

  if (write_cache)
    write_cache->share= cshare;

  DBUG_VOID_RETURN;
}


/*
  Remove a thread from shared access to IO_CACHE.

  SYNOPSIS
    remove_io_thread()
      cache                     The IO_CACHE to be removed from the share.

  NOTE

    Every thread must do that on exit for not to deadlock other threads.

    The last thread destroys the pthread resources.

    A writer flushes its cache first.

  RETURN
    void
*/

void remove_io_thread(IO_CACHE *cache)
{
  IO_CACHE_SHARE *cshare= cache->share;
  uint total;
  DBUG_ENTER("remove_io_thread");

  /* If the writer goes, it needs to flush the write cache. */
  if (cache == cshare->source_cache)
    flush_io_cache(cache);

  pthread_mutex_lock(&cshare->mutex);
  DBUG_PRINT("io_cache_share", ("%s: %p",
                                (cache == cshare->source_cache) ?
                                "writer" : "reader", cache));

  /* Remove from share. */
  total= --cshare->total_threads;
  DBUG_PRINT("io_cache_share", ("remaining threads: %u", total));

  /* Detach from share. */
  cache->share= NULL;

  /* If the writer goes, let the readers know. */
  if (cache == cshare->source_cache)
  {
    DBUG_PRINT("io_cache_share", ("writer leaves"));
    cshare->source_cache= NULL;
  }

  /* If all threads are waiting for me to join the lock, wake them. */
  if (!--cshare->running_threads)
  {
    DBUG_PRINT("io_cache_share", ("the last running thread leaves, wake all"));
    pthread_cond_signal(&cshare->cond_writer);
    pthread_cond_broadcast(&cshare->cond);
  }

  pthread_mutex_unlock(&cshare->mutex);

  if (!total)
  {
    DBUG_PRINT("io_cache_share", ("last thread removed, destroy share"));
    pthread_cond_destroy (&cshare->cond_writer);
    pthread_cond_destroy (&cshare->cond);
    pthread_mutex_destroy(&cshare->mutex);
  }

  DBUG_VOID_RETURN;
}


/*
  Lock IO cache and wait for all other threads to join.

  SYNOPSIS
    lock_io_cache()
      cache                     The cache of the thread entering the lock.
      pos                       File position of the block to read.
                                Unused for the write thread.

  DESCRIPTION

    Wait for all threads to finish with the current buffer. We want
    all threads to proceed in concert. The last thread to join
    lock_io_cache() will read the block from file and all threads start
    to use it. Then they will join again for reading the next block.

    The waiting threads detect a fresh buffer by comparing
    cshare->pos_in_file with the position they want to process next.
    Since the first block may start at position 0, we take
    cshare->read_end as an additional condition. This variable is
    initialized to NULL and will be set after a block of data is written
    to the buffer.

  RETURN
    1           OK, lock in place, go ahead and read.
    0           OK, unlocked, another thread did the read.
*/

static int lock_io_cache(IO_CACHE *cache, my_off_t pos)
{
  IO_CACHE_SHARE *cshare= cache->share;
  DBUG_ENTER("lock_io_cache");

  /* Enter the lock. */
  pthread_mutex_lock(&cshare->mutex);
  cshare->running_threads--;
  DBUG_PRINT("io_cache_share", ("%s: %p  pos: %lu  running: %u",
                                (cache == cshare->source_cache) ?
                                "writer" : "reader", cache, (ulong) pos,
                                cshare->running_threads));

  if (cshare->source_cache)
  {
    /* A write cache is synchronized to the read caches. */

    if (cache == cshare->source_cache)
    {
      /* The writer waits until all readers are here. */
      while (cshare->running_threads)
      {
        DBUG_PRINT("io_cache_share", ("writer waits in lock"));
        pthread_cond_wait(&cshare->cond_writer, &cshare->mutex);
      }
      DBUG_PRINT("io_cache_share", ("writer awoke, going to copy"));

      /* Stay locked. Leave the lock later by unlock_io_cache(). */
      DBUG_RETURN(1);
    }

    /* The last thread wakes the writer. */
    if (!cshare->running_threads)
    {
      DBUG_PRINT("io_cache_share", ("waking writer"));
      pthread_cond_signal(&cshare->cond_writer);
    }

    /*
      Readers wait until the data is copied from the writer. Another
      reason to stop waiting is the removal of the write thread. If this
      happens, we leave the lock with old data in the buffer.
    */
    while ((!cshare->read_end || (cshare->pos_in_file < pos)) &&
           cshare->source_cache)
    {
      DBUG_PRINT("io_cache_share", ("reader waits in lock"));
      pthread_cond_wait(&cshare->cond, &cshare->mutex);
    }

    /*
      If the writer was removed from the share while this thread was
      asleep, we need to simulate an EOF condition. The writer cannot
      reset the share variables as they might still be in use by readers
      of the last block. When we awake here then because the last
      joining thread signalled us. If the writer is not the last, it
      will not signal. So it is safe to clear the buffer here.
    */
    if (!cshare->read_end || (cshare->pos_in_file < pos))
    {
      DBUG_PRINT("io_cache_share", ("reader found writer removed. EOF"));
      cshare->read_end= cshare->buffer; /* Empty buffer. */
      cshare->error= 0; /* EOF is not an error. */
    }
  }
  else
  {
    /*
      There are read caches only. The last thread arriving in
      lock_io_cache() continues with a locked cache and reads the block.
    */
    if (!cshare->running_threads)
    {
      DBUG_PRINT("io_cache_share", ("last thread joined, going to read"));
      /* Stay locked. Leave the lock later by unlock_io_cache(). */
      DBUG_RETURN(1);
    }

    /*
      All other threads wait until the requested block is read by the
      last thread arriving. Another reason to stop waiting is the
      removal of a thread. If this leads to all threads being in the
      lock, we have to continue also. The first of the awaken threads
      will then do the read.
    */
    while ((!cshare->read_end || (cshare->pos_in_file < pos)) &&
           cshare->running_threads)
    {
      DBUG_PRINT("io_cache_share", ("reader waits in lock"));
      pthread_cond_wait(&cshare->cond, &cshare->mutex);
    }

    /* If the block is not yet read, continue with a locked cache and read. */
    if (!cshare->read_end || (cshare->pos_in_file < pos))
    {
      DBUG_PRINT("io_cache_share", ("reader awoke, going to read"));
      /* Stay locked. Leave the lock later by unlock_io_cache(). */
      DBUG_RETURN(1);
    }

    /* Another thread did read the block already. */
  }
  DBUG_PRINT("io_cache_share", ("reader awoke, going to process %u bytes",
                                (uint) (cshare->read_end ? (size_t)
                                        (cshare->read_end - cshare->buffer) :
                                        0)));

  /*
    Leave the lock. Do not call unlock_io_cache() later. The thread that
    filled the buffer did this and marked all threads as running.
  */
  pthread_mutex_unlock(&cshare->mutex);
  DBUG_RETURN(0);
}


/*
  Unlock IO cache.

  SYNOPSIS
    unlock_io_cache()
      cache                     The cache of the thread leaving the lock.

  NOTE
    This is called by the thread that filled the buffer. It marks all
    threads as running and awakes them. This must not be done by any
    other thread.

    Do not signal cond_writer. Either there is no writer or the writer
    is the only one who can call this function.

    The reason for resetting running_threads to total_threads before
    waking all other threads is that it could be possible that this
    thread is so fast with processing the buffer that it enters the lock
    before even one other thread has left it. If every awoken thread
    would increase running_threads by one, this thread could think that
    he is again the last to join and would not wait for the other
    threads to process the data.

  RETURN
    void
*/

static void unlock_io_cache(IO_CACHE *cache)
{
  IO_CACHE_SHARE *cshare= cache->share;
  DBUG_ENTER("unlock_io_cache");
  DBUG_PRINT("io_cache_share", ("%s: %p  pos: %lu  running: %u",
                                (cache == cshare->source_cache) ?
                                "writer" : "reader",
                                cache, (ulong) cshare->pos_in_file,
                                cshare->total_threads));

  cshare->running_threads= cshare->total_threads;
  pthread_cond_broadcast(&cshare->cond);
  pthread_mutex_unlock(&cshare->mutex);
  DBUG_VOID_RETURN;
}


/*
  Read from IO_CACHE when it is shared between several threads.

  SYNOPSIS
    _my_b_read_r()
      cache                     IO_CACHE pointer
      Buffer                    Buffer to retrieve count bytes from file
      Count                     Number of bytes to read into Buffer

  NOTE
    This function is only called from the my_b_read() macro when there
    isn't enough characters in the buffer to satisfy the request.

  IMPLEMENTATION

    It works as follows: when a thread tries to read from a file (that
    is, after using all the data from the (shared) buffer), it just
    hangs on lock_io_cache(), waiting for other threads. When the very
    last thread attempts a read, lock_io_cache() returns 1, the thread
    does actual IO and unlock_io_cache(), which signals all the waiting
    threads that data is in the buffer.

  WARNING

    When changing this function, be careful with handling file offsets
    (end-of_file, pos_in_file). Do not cast them to possibly smaller
    types than my_off_t unless you can be sure that their value fits.
    Same applies to differences of file offsets. (Bug #11527)

    When changing this function, check _my_b_read(). It might need the
    same change.

  RETURN
    0      we succeeded in reading all data
    1      Error: can't read requested characters
*/

int _my_b_read_r(register IO_CACHE *cache, uchar *Buffer, size_t Count)
{
  my_off_t pos_in_file;
  size_t length, diff_length, left_length;
  IO_CACHE_SHARE *cshare= cache->share;
  DBUG_ENTER("_my_b_read_r");

  if ((left_length= (size_t) (cache->read_end - cache->read_pos)))
  {
    DBUG_ASSERT(Count >= left_length);	/* User is not using my_b_read() */
    memcpy(Buffer, cache->read_pos, left_length);
    Buffer+= left_length;
    Count-= left_length;
  }
  while (Count)
  {
    size_t cnt, len;

    pos_in_file= cache->pos_in_file + (cache->read_end - cache->buffer);
    diff_length= (size_t) (pos_in_file & (IO_SIZE-1));
    length=IO_ROUND_UP(Count+diff_length)-diff_length;
    length= ((length <= cache->read_length) ?
             length + IO_ROUND_DN(cache->read_length - length) :
             length - IO_ROUND_UP(length - cache->read_length));
    if (cache->type != READ_FIFO &&
	(length > (cache->end_of_file - pos_in_file)))
      length= (size_t) (cache->end_of_file - pos_in_file);
    if (length == 0)
    {
      cache->error= (int) left_length;
      DBUG_RETURN(1);
    }
    if (lock_io_cache(cache, pos_in_file))
    {
      /* With a synchronized write/read cache we won't come here... */
      DBUG_ASSERT(!cshare->source_cache);
      /*
        ... unless the writer has gone before this thread entered the
        lock. Simulate EOF in this case. It can be distinguished by
        cache->file.
      */
      if (cache->file < 0)
        len= 0;
      else
      {
        /*
          Whenever a function which operates on IO_CACHE flushes/writes
          some part of the IO_CACHE to disk it will set the property
          "seek_not_done" to indicate this to other functions operating
          on the IO_CACHE.
        */
        if (cache->seek_not_done)
        {
          if (my_seek(cache->file, pos_in_file, MY_SEEK_SET, MYF(0))
              == MY_FILEPOS_ERROR)
          {
            cache->error= -1;
            unlock_io_cache(cache);
            DBUG_RETURN(1);
          }
        }
        len= my_read(cache->file, cache->buffer, length, cache->myflags);
      }
      DBUG_PRINT("io_cache_share", ("read %lu bytes", (ulong) len));

      cache->read_end=    cache->buffer + (len == (size_t) -1 ? 0 : len);
      cache->error=       (len == length ? 0 : (int) len);
      cache->pos_in_file= pos_in_file;

      /* Copy important values to the share. */
      cshare->error=       cache->error;
      cshare->read_end=    cache->read_end;
      cshare->pos_in_file= pos_in_file;

      /* Mark all threads as running and wake them. */
      unlock_io_cache(cache);
    }
    else
    {
      /*
        With a synchronized write/read cache readers always come here.
        Copy important values from the share.
      */
      cache->error=       cshare->error;
      cache->read_end=    cshare->read_end;
      cache->pos_in_file= cshare->pos_in_file;

      len= ((cache->error == -1) ? (size_t) -1 :
            (size_t) (cache->read_end - cache->buffer));
    }
    cache->read_pos=      cache->buffer;
    cache->seek_not_done= 0;
    if (len == 0 || len == (size_t) -1)
    {
      DBUG_PRINT("io_cache_share", ("reader error. len %lu  left %lu",
                                    (ulong) len, (ulong) left_length));
      cache->error= (int) left_length;
      DBUG_RETURN(1);
    }
    cnt= (len > Count) ? Count : len;
    memcpy(Buffer, cache->read_pos, cnt);
    Count -= cnt;
    Buffer+= cnt;
    left_length+= cnt;
    cache->read_pos+= cnt;
  }
  DBUG_RETURN(0);
}


/*
  Copy data from write cache to read cache.

  SYNOPSIS
    copy_to_read_buffer()
      write_cache               The write cache.
      write_buffer              The source of data, mostly the cache buffer.
      write_length              The number of bytes to copy.

  NOTE
    The write thread will wait for all read threads to join the cache
    lock. Then it copies the data over and wakes the read threads.

  RETURN
    void
*/

static void copy_to_read_buffer(IO_CACHE *write_cache,
                                const uchar *write_buffer, size_t write_length)
{
  IO_CACHE_SHARE *cshare= write_cache->share;

  DBUG_ASSERT(cshare->source_cache == write_cache);
  /*
    write_length is usually less or equal to buffer_length.
    It can be bigger if _my_b_write() is called with a big length.
  */
  while (write_length)
  {
    size_t copy_length= min(write_length, write_cache->buffer_length);
    int  __attribute__((unused)) rc;

    rc= lock_io_cache(write_cache, write_cache->pos_in_file);
    /* The writing thread does always have the lock when it awakes. */
    DBUG_ASSERT(rc);

    memcpy(cshare->buffer, write_buffer, copy_length);

    cshare->error=       0;
    cshare->read_end=    cshare->buffer + copy_length;
    cshare->pos_in_file= write_cache->pos_in_file;

    /* Mark all threads as running and wake them. */
    unlock_io_cache(write_cache);

    write_buffer+= copy_length;
    write_length-= copy_length;
  }
}
#endif /*THREAD*/


/*
  Do sequential read from the SEQ_READ_APPEND cache.
  
  We do this in three stages:
   - first read from info->buffer
   - then if there are still data to read, try the file descriptor
   - afterwards, if there are still data to read, try append buffer

  RETURNS
    0  Success
    1  Failed to read
*/

int _my_b_seq_read(register IO_CACHE *info, uchar *Buffer, size_t Count)
{
  size_t length, diff_length, left_length, save_count, max_length;
  my_off_t pos_in_file;
  save_count=Count;

  /* first, read the regular buffer */
  if ((left_length=(size_t) (info->read_end-info->read_pos)))
  {
    DBUG_ASSERT(Count > left_length);	/* User is not using my_b_read() */
    memcpy(Buffer,info->read_pos, left_length);
    Buffer+=left_length;
    Count-=left_length;
  }
  lock_append_buffer(info);

  /* pos_in_file always point on where info->buffer was read */
  if ((pos_in_file=info->pos_in_file +
       (size_t) (info->read_end - info->buffer)) >= info->end_of_file)
    goto read_append_buffer;

  /*
    With read-append cache we must always do a seek before we read,
    because the write could have moved the file pointer astray
  */
  if (my_seek(info->file,pos_in_file,MY_SEEK_SET,MYF(0)) == MY_FILEPOS_ERROR)
  {
   info->error= -1;
   unlock_append_buffer(info);
   return (1);
  }
  info->seek_not_done=0;

  diff_length= (size_t) (pos_in_file & (IO_SIZE-1));

  /* now the second stage begins - read from file descriptor */
  if (Count >= (size_t) (IO_SIZE+(IO_SIZE-diff_length)))
  {
    /* Fill first intern buffer */
    size_t read_length;

    length=(Count & (size_t) ~(IO_SIZE-1))-diff_length;
    if ((read_length= my_read(info->file,Buffer, length,
                              info->myflags)) == (size_t) -1)
    {
      info->error= -1;
      unlock_append_buffer(info);
      return 1;
    }
    Count-=read_length;
    Buffer+=read_length;
    pos_in_file+=read_length;

    if (read_length != length)
    {
      /*
	We only got part of data;  Read the rest of the data from the
	write buffer
      */
      goto read_append_buffer;
    }
    left_length+=length;
    diff_length=0;
  }

  max_length= info->read_length-diff_length;
  if (max_length > (info->end_of_file - pos_in_file))
    max_length= (size_t) (info->end_of_file - pos_in_file);
  if (!max_length)
  {
    if (Count)
      goto read_append_buffer;
    length=0;				/* Didn't read any more chars */
  }
  else
  {
    length= my_read(info->file,info->buffer, max_length, info->myflags);
    if (length == (size_t) -1)
    {
      info->error= -1;
      unlock_append_buffer(info);
      return 1;
    }
    if (length < Count)
    {
      memcpy(Buffer, info->buffer, length);
      Count -= length;
      Buffer += length;

      /*
	 added the line below to make
	 DBUG_ASSERT(pos_in_file==info->end_of_file) pass.
	 otherwise this does not appear to be needed
      */
      pos_in_file += length;
      goto read_append_buffer;
    }
  }
  unlock_append_buffer(info);
  info->read_pos=info->buffer+Count;
  info->read_end=info->buffer+length;
  info->pos_in_file=pos_in_file;
  memcpy(Buffer,info->buffer,(size_t) Count);
  return 0;

read_append_buffer:

  /*
     Read data from the current write buffer.
     Count should never be == 0 here (The code will work even if count is 0)
  */

  {
    /* First copy the data to Count */
    size_t len_in_buff = (size_t) (info->write_pos - info->append_read_pos);
    size_t copy_len;
    size_t transfer_len;

    DBUG_ASSERT(info->append_read_pos <= info->write_pos);
    /*
      TODO: figure out if the assert below is needed or correct.
    */
    DBUG_ASSERT(pos_in_file == info->end_of_file);
    copy_len=min(Count, len_in_buff);
    memcpy(Buffer, info->append_read_pos, copy_len);
    info->append_read_pos += copy_len;
    Count -= copy_len;
    if (Count)
      info->error= (int) (save_count - Count);

    /* Fill read buffer with data from write buffer */
    memcpy(info->buffer, info->append_read_pos,
	   (size_t) (transfer_len=len_in_buff - copy_len));
    info->read_pos= info->buffer;
    info->read_end= info->buffer+transfer_len;
    info->append_read_pos=info->write_pos;
    info->pos_in_file=pos_in_file+copy_len;
    info->end_of_file+=len_in_buff;
  }
  unlock_append_buffer(info);
  return Count ? 1 : 0;
}


#ifdef HAVE_AIOWAIT

/*
  Read from the IO_CACHE into a buffer and feed asynchronously
  from disk when needed.

  SYNOPSIS
    _my_b_async_read()
      info                      IO_CACHE pointer
      Buffer                    Buffer to retrieve count bytes from file
      Count                     Number of bytes to read into Buffer

  RETURN VALUE
    -1          An error has occurred; my_errno is set.
     0          Success
     1          An error has occurred; IO_CACHE to error state.
*/

int _my_b_async_read(register IO_CACHE *info, uchar *Buffer, size_t Count)
{
  size_t length,read_length,diff_length,left_length,use_length,org_Count;
  size_t max_length;
  my_off_t next_pos_in_file;
  uchar *read_buffer;

  memcpy(Buffer,info->read_pos,
	 (left_length= (size_t) (info->read_end-info->read_pos)));
  Buffer+=left_length;
  org_Count=Count;
  Count-=left_length;

  if (info->inited)
  {						/* wait for read block */
    info->inited=0;				/* No more block to read */
    my_aiowait(&info->aio_result);		/* Wait for outstanding req */
    if (info->aio_result.result.aio_errno)
    {
      if (info->myflags & MY_WME)
	my_error(EE_READ, MYF(ME_BELL+ME_WAITTANG),
		 my_filename(info->file),
		 info->aio_result.result.aio_errno);
      my_errno=info->aio_result.result.aio_errno;
      info->error= -1;
      return(1);
    }
    if (! (read_length= (size_t) info->aio_result.result.aio_return) ||
	read_length == (size_t) -1)
    {
      my_errno=0;				/* For testing */
      info->error= (read_length == (size_t) -1 ? -1 :
		    (int) (read_length+left_length));
      return(1);
    }
    info->pos_in_file+= (size_t) (info->read_end - info->request_pos);

    if (info->request_pos != info->buffer)
      info->request_pos=info->buffer;
    else
      info->request_pos=info->buffer+info->read_length;
    info->read_pos=info->request_pos;
    next_pos_in_file=info->aio_read_pos+read_length;

	/* Check if pos_in_file is changed
	   (_ni_read_cache may have skipped some bytes) */

    if (info->aio_read_pos < info->pos_in_file)
    {						/* Fix if skipped bytes */
      if (info->aio_read_pos + read_length < info->pos_in_file)
      {
	read_length=0;				/* Skip block */
	next_pos_in_file=info->pos_in_file;
      }
      else
      {
	my_off_t offset= (info->pos_in_file - info->aio_read_pos);
	info->pos_in_file=info->aio_read_pos; /* Whe are here */
	info->read_pos=info->request_pos+offset;
	read_length-=offset;			/* Bytes left from read_pos */
      }
    }
#ifndef DBUG_OFF
    if (info->aio_read_pos > info->pos_in_file)
    {
      my_errno=EINVAL;
      return(info->read_length= (size_t) -1);
    }
#endif
	/* Copy found bytes to buffer */
    length=min(Count,read_length);
    memcpy(Buffer,info->read_pos,(size_t) length);
    Buffer+=length;
    Count-=length;
    left_length+=length;
    info->read_end=info->rc_pos+read_length;
    info->read_pos+=length;
  }
  else
    next_pos_in_file=(info->pos_in_file+ (size_t)
		      (info->read_end - info->request_pos));

	/* If reading large blocks, or first read or read with skip */
  if (Count)
  {
    if (next_pos_in_file == info->end_of_file)
    {
      info->error=(int) (read_length+left_length);
      return 1;
    }
    
    if (my_seek(info->file,next_pos_in_file,MY_SEEK_SET,MYF(0))
        == MY_FILEPOS_ERROR)
    {
      info->error= -1;
      return (1);
    }

    read_length=IO_SIZE*2- (size_t) (next_pos_in_file & (IO_SIZE-1));
    if (Count < read_length)
    {					/* Small block, read to cache */
      if ((read_length=my_read(info->file,info->request_pos,
			       read_length, info->myflags)) == (size_t) -1)
        return info->error= -1;
      use_length=min(Count,read_length);
      memcpy(Buffer,info->request_pos,(size_t) use_length);
      info->read_pos=info->request_pos+Count;
      info->read_end=info->request_pos+read_length;
      info->pos_in_file=next_pos_in_file;	/* Start of block in cache */
      next_pos_in_file+=read_length;

      if (Count != use_length)
      {					/* Didn't find hole block */
	if (info->myflags & (MY_WME | MY_FAE | MY_FNABP) && Count != org_Count)
	  my_error(EE_EOFERR, MYF(ME_BELL+ME_WAITTANG),
		   my_filename(info->file),my_errno);
	info->error=(int) (read_length+left_length);
	return 1;
      }
    }
    else
    {						/* Big block, don't cache it */
      if ((read_length= my_read(info->file,Buffer, Count,info->myflags))
	  != Count)
      {
	info->error= read_length == (size_t) -1 ? -1 : read_length+left_length;
	return 1;
      }
      info->read_pos=info->read_end=info->request_pos;
      info->pos_in_file=(next_pos_in_file+=Count);
    }
  }

  /* Read next block with asyncronic io */
  diff_length=(next_pos_in_file & (IO_SIZE-1));
  max_length= info->read_length - diff_length;
  if (max_length > info->end_of_file - next_pos_in_file)
    max_length= (size_t) (info->end_of_file - next_pos_in_file);

  if (info->request_pos != info->buffer)
    read_buffer=info->buffer;
  else
    read_buffer=info->buffer+info->read_length;
  info->aio_read_pos=next_pos_in_file;
  if (max_length)
  {
    info->aio_result.result.aio_errno=AIO_INPROGRESS;	/* Marker for test */
    DBUG_PRINT("aioread",("filepos: %ld  length: %lu",
			  (ulong) next_pos_in_file, (ulong) max_length));
    if (aioread(info->file,read_buffer, max_length,
		(my_off_t) next_pos_in_file,MY_SEEK_SET,
		&info->aio_result.result))
    {						/* Skip async io */
      my_errno=errno;
      DBUG_PRINT("error",("got error: %d, aio_result: %d from aioread, async skipped",
			  errno, info->aio_result.result.aio_errno));
      if (info->request_pos != info->buffer)
      {
	bmove(info->buffer,info->request_pos,
	      (size_t) (info->read_end - info->read_pos));
	info->request_pos=info->buffer;
	info->read_pos-=info->read_length;
	info->read_end-=info->read_length;
      }
      info->read_length=info->buffer_length;	/* Use hole buffer */
      info->read_function=_my_b_read;		/* Use normal IO_READ next */
    }
    else
      info->inited=info->aio_result.pending=1;
  }
  return 0;					/* Block read, async in use */
} /* _my_b_async_read */
#endif


/* Read one byte when buffer is empty */

int _my_b_get(IO_CACHE *info)
{
  uchar buff;
  IO_CACHE_CALLBACK pre_read,post_read;
  if ((pre_read = info->pre_read))
    (*pre_read)(info, NULL, 0, 0);
  if ((*(info)->read_function)(info,&buff,1))
    return my_b_EOF;
  if ((post_read = info->post_read))
    (*post_read)(info, NULL, 0, 0);
  return (int) (uchar) buff;
}

/* FUNCTIONS TO DO WRITES TO THE CACHE */

#define set_hard_write_error(CACHE)                         \
  ((CACHE)->error= (CACHE)->hard_write_error_in_the_past= -1)

/* 
   Write a byte buffer to IO_CACHE and flush to disk
   if IO_CACHE is full.

   RETURN VALUE
    1 On error on write
    0 On success
   -1 On error; my_errno contains error code.
*/

int _my_b_write(register IO_CACHE *info, const uchar *Buffer, size_t Count)
{
  size_t rest_length,length;

  if (info->pos_in_file+info->buffer_length > info->end_of_file)
  {
    my_errno=errno=EFBIG;
    return set_hard_write_error(info);
  }

  rest_length= (size_t) (info->write_end - info->write_pos);
  memcpy(info->write_pos,Buffer,(size_t) rest_length);
  Buffer+=rest_length;
  Count-=rest_length;
  info->write_pos+=rest_length;

  if (my_b_flush_io_cache(info,1))
    return 1;
  if (Count >= IO_SIZE)
  {					/* Fill first intern buffer */
    length=Count & (size_t) ~(IO_SIZE-1);
    if (info->seek_not_done)
    {
      /*
        Whenever a function which operates on IO_CACHE flushes/writes
        some part of the IO_CACHE to disk it will set the property
        "seek_not_done" to indicate this to other functions operating
        on the IO_CACHE.
      */
      if (my_seek(info->file,info->pos_in_file,MY_SEEK_SET,MYF(0)))
      {
        set_hard_write_error(info);
        return (1);
      }
      info->seek_not_done=0;
    }
    if (my_write(info->file, Buffer, length, info->myflags | MY_NABP))
      return set_hard_write_error(info);
    if (info->post_write)
      (*(info->post_write))(info, Buffer, length, info->pos_in_file);

#ifdef THREAD
    /*
      In case of a shared I/O cache with a writer we normally do direct
      write cache to read cache copy. Simulate this here by direct
      caller buffer to read cache copy. Do it after the write so that
      the cache readers actions on the flushed part can go in parallel
      with the write of the extra stuff. copy_to_read_buffer()
      synchronizes writer and readers so that after this call the
      readers can act on the extra stuff while the writer can go ahead
      and prepare the next output. copy_to_read_buffer() relies on
      info->pos_in_file.
    */
    if (info->share)
      copy_to_read_buffer(info, Buffer, length);
#endif

    Count-=length;
    Buffer+=length;
    info->pos_in_file+=length;
  }
  memcpy(info->write_pos,Buffer,(size_t) Count);
  info->write_pos+=Count;
  return 0;
}


/*
  Append a block to the write buffer.
  This is done with the buffer locked to ensure that we don't read from
  the write buffer before we are ready with it.
*/

int my_b_append(register IO_CACHE *info, const uchar *Buffer, size_t Count)
{
  size_t rest_length,length;

#ifdef THREAD
  /*
    Assert that we cannot come here with a shared cache. If we do one
    day, we might need to add a call to copy_to_read_buffer().
  */
  DBUG_ASSERT(!info->share);
#endif
  DBUG_ASSERT(info->post_write == NULL); /* unsupported */
  lock_append_buffer(info);
  rest_length= (size_t) (info->write_end - info->write_pos);
  if (Count <= rest_length)
    goto end;
  memcpy(info->write_pos, Buffer, rest_length);
  Buffer+=rest_length;
  Count-=rest_length;
  info->write_pos+=rest_length;
  if (my_b_flush_io_cache(info,0))
  {
    unlock_append_buffer(info);
    return 1;
  }
  if (Count >= IO_SIZE)
  {					/* Fill first intern buffer */
    length=Count & (size_t) ~(IO_SIZE-1);
    if (my_write(info->file,Buffer, length, info->myflags | MY_NABP))
    {
      unlock_append_buffer(info);
      return set_hard_write_error(info);
    }
    Count-=length;
    Buffer+=length;
    info->end_of_file+=length;
  }

end:
  memcpy(info->write_pos,Buffer,(size_t) Count);
  info->write_pos+=Count;
  unlock_append_buffer(info);
  return 0;
}


int my_b_safe_write(IO_CACHE *info, const uchar *Buffer, size_t Count)
{
  /*
    Sasha: We are not writing this with the ? operator to avoid hitting
    a possible compiler bug. At least gcc 2.95 cannot deal with 
    several layers of ternary operators that evaluated comma(,) operator
    expressions inside - I do have a test case if somebody wants it
  */
  if (info->type == SEQ_READ_APPEND)
    return my_b_append(info, Buffer, Count);
  return my_b_write(info, Buffer, Count);
}


/*
  Write a block to disk where part of the data may be inside the record
  buffer.  As all write calls to the data goes through the cache,
  we will never get a seek over the end of the buffer
*/

int my_block_write(register IO_CACHE *info, const uchar *Buffer, size_t Count,
		   my_off_t pos)
{
  size_t length;
  int error=0;

#ifdef THREAD
  /*
    Assert that we cannot come here with a shared cache. If we do one
    day, we might need to add a call to copy_to_read_buffer().
  */
  DBUG_ASSERT(!info->share);
#endif

  if (pos < info->pos_in_file)
  {
    /* Of no overlap, write everything without buffering */
    if (pos + Count <= info->pos_in_file)
    {
      int ret= my_pwrite(info->file, Buffer, Count, pos,
                         info->myflags | MY_NABP);
      if (unlikely(ret))
        set_hard_write_error(info);
      if (info->post_write)
        (*(info->post_write))(info, Buffer, Count, pos);
      return ret;
    }
    /* Write the part of the block that is before buffer */
    length= (uint) (info->pos_in_file - pos);
    if (my_pwrite(info->file, Buffer, length, pos, info->myflags | MY_NABP))
      error= set_hard_write_error(info);
    if (info->post_write)
      (*(info->post_write))(info, Buffer, length, pos);
    Buffer+=length;
    pos+=  length;
    Count-= length;
#ifndef HAVE_PREAD
    info->seek_not_done=1;
#endif
  }

  /* Check if we want to write inside the used part of the buffer.*/
  length= (size_t) (info->write_end - info->buffer);
  if (pos < info->pos_in_file + length)
  {
    size_t offset= (size_t) (pos - info->pos_in_file);
    length-=offset;
    if (length > Count)
      length=Count;
    memcpy(info->buffer+offset, Buffer, length);
    Buffer+=length;
    Count-= length;
    /* Fix length of buffer if the new data was larger */
    if (info->buffer+length > info->write_pos)
      info->write_pos=info->buffer+length;
    if (!Count)
      return (error);
  }
  /* Write at the end of the current buffer; This is the normal case */
  if (_my_b_write(info, Buffer, Count))
    error= -1;
  return error;
}


	/* Flush write cache */

#ifdef THREAD
#define LOCK_APPEND_BUFFER if (need_append_buffer_lock) \
  lock_append_buffer(info);
#define UNLOCK_APPEND_BUFFER if (need_append_buffer_lock) \
  unlock_append_buffer(info);
#else
#define LOCK_APPEND_BUFFER
#define UNLOCK_APPEND_BUFFER
#endif


int my_b_flush_io_cache(IO_CACHE *info, int need_append_buffer_lock)
{
  size_t length;
  my_bool append_cache;
  my_off_t pos_in_file;
  DBUG_ENTER("my_b_flush_io_cache");
  DBUG_PRINT("enter", ("cache: 0x%lx", (long) info));

  if (!(append_cache = (info->type == SEQ_READ_APPEND)))
    need_append_buffer_lock=0;

  if (info->type == WRITE_CACHE || append_cache)
  {
    if (info->file == -1)
    {
      if (real_open_cached_file(info))
	DBUG_RETURN(set_hard_write_error(info));
    }
    LOCK_APPEND_BUFFER;

    if ((length=(size_t) (info->write_pos - info->write_buffer)))
    {
#ifdef THREAD
      /*
        In case of a shared I/O cache with a writer we do direct write
        cache to read cache copy. Do it before the write here so that
        the readers can work in parallel with the write.
        copy_to_read_buffer() relies on info->pos_in_file.
      */
      if (info->share)
        copy_to_read_buffer(info, info->write_buffer, length);
#endif

      pos_in_file=info->pos_in_file;
      /*
	If we have append cache, we always open the file with
	O_APPEND which moves the pos to EOF automatically on every write
      */
      if (!append_cache && info->seek_not_done)
      {					/* File touched, do seek */
	if (my_seek(info->file,pos_in_file,MY_SEEK_SET,MYF(0)) ==
	    MY_FILEPOS_ERROR)
	{
	  UNLOCK_APPEND_BUFFER;
	  DBUG_RETURN(set_hard_write_error(info));
	}
	if (!append_cache)
	  info->seek_not_done=0;
      }
      info->write_end= (info->write_buffer+info->buffer_length-
			((pos_in_file+length) & (IO_SIZE-1)));

      if (my_write(info->file,info->write_buffer,length,
		   info->myflags | MY_NABP))
	set_hard_write_error(info);
      else
	info->error= 0;
      if (!append_cache)
      {
        /*
          This post_write is really POST-write; callers depend on this! So
          always call it after writing to the file, not before.
        */
        if (info->post_write)
          (*(info->post_write))(info, info->write_buffer,
                                length, info->pos_in_file);
        /*
          The addition below will make the info->pos_in_file be the end of
          written block; whereas the value we needed in post_write is the
          value before the addition. That's why we called post_write before
          this.
        */
	info->pos_in_file+=length;
        set_if_bigger(info->end_of_file,(pos_in_file+length));
      }
      else
      {
	info->end_of_file+=(info->write_pos-info->append_read_pos);
	DBUG_ASSERT(info->end_of_file == my_tell(info->file,MYF(0)));
        DBUG_ASSERT(info->post_write == NULL); /* unsupported */
      }

      info->append_read_pos=info->write_pos=info->write_buffer;
      ++info->disk_writes;
      UNLOCK_APPEND_BUFFER;
      DBUG_RETURN(info->error);
    }
  }
#ifdef HAVE_AIOWAIT
  else if (info->type != READ_NET)
  {
    my_aiowait(&info->aio_result);		/* Wait for outstanding req */
    info->inited=0;
  }
#endif
  UNLOCK_APPEND_BUFFER;
  DBUG_RETURN(0);
}

/*
  Free an IO_CACHE object

  SYNOPSOS
    end_io_cache()
    info		IO_CACHE Handle to free

  NOTES
    It's currently safe to call this if one has called init_io_cache()
    on the 'info' object, even if init_io_cache() failed.
    This function is also safe to call twice with the same handle.

  RETURN
   0  ok
   #  Error
*/

int end_io_cache(IO_CACHE *info)
{
  int error=0;
  IO_CACHE_CALLBACK pre_close;
  DBUG_ENTER("end_io_cache");
  DBUG_PRINT("enter",("cache: %p", info));

#ifdef THREAD
  /*
    Every thread must call remove_io_thread(). The last one destroys
    the share elements.
  */
  DBUG_ASSERT(!info->share || !info->share->total_threads);
#endif

  if ((pre_close=info->pre_close))
  {
    (*pre_close)(info, NULL, 0, 0);
    info->pre_close= 0;
  }
  if (info->alloced_buffer)
  {
    info->alloced_buffer=0;
    if (info->file != -1)			/* File doesn't exist */
      error= my_b_flush_io_cache(info,1);
    my_free((uchar*) info->buffer,MYF(MY_WME));
    info->buffer=info->read_pos=(uchar*) 0;
  }
  if (info->type == SEQ_READ_APPEND)
  {
    /* Destroy allocated mutex */
    info->type= TYPE_NOT_SET;
#ifdef THREAD
    pthread_mutex_destroy(&info->append_buffer_lock);
#endif
  }
#ifdef THREAD
  info->share= 0;
#endif
  DBUG_RETURN(error);
} /* end_io_cache */


/**********************************************************************
 Testing of MF_IOCACHE
**********************************************************************/

#ifdef MAIN

#include <libmysql_my_dir.h>

void die(const char* fmt, ...)
{
  va_list va_args;
  va_start(va_args,fmt);
  fprintf(stderr,"Error:");
  vfprintf(stderr, fmt,va_args);
  fprintf(stderr,", errno=%d\n", errno);
  exit(1);
}

int open_file(const char* fname, IO_CACHE* info, int cache_size)
{
  int fd;
  if ((fd=my_open(fname,O_CREAT | O_RDWR,MYF(MY_WME))) < 0)
    die("Could not open %s", fname);
  if (init_io_cache(info, fd, cache_size, SEQ_READ_APPEND, 0,0,MYF(MY_WME)))
    die("failed in init_io_cache()");
  return fd;
}

void close_file(IO_CACHE* info)
{
  end_io_cache(info);
  my_close(info->file, MYF(MY_WME));
}

int main(int argc, char** argv)
{
  IO_CACHE sra_cache; /* SEQ_READ_APPEND */
  MY_STAT status;
  const char* fname="/tmp/iocache.test";
  int cache_size=16384;
  char llstr_buf[22];
  int max_block,total_bytes=0;
  int i,num_loops=100,error=0;
  char *p;
  char* block, *block_end;
  MY_INIT(argv[0]);
  max_block = cache_size*3;
  if (!(block=(char*)my_malloc(max_block,MYF(MY_WME))))
    die("Not enough memory to allocate test block");
  block_end = block + max_block;
  for (p = block,i=0; p < block_end;i++)
  {
    *p++ = (char)i;
  }
  if (my_stat(fname,&status, MYF(0)) &&
      my_delete(fname,MYF(MY_WME)))
    {
      die("Delete of %s failed, aborting", fname);
    }
  open_file(fname,&sra_cache, cache_size);
  for (i = 0; i < num_loops; i++)
  {
    char buf[4];
    int block_size = abs(rand() % max_block);
    int4store(buf, block_size);
    if (my_b_append(&sra_cache,buf,4) ||
	my_b_append(&sra_cache, block, block_size))
      die("write failed");
    total_bytes += 4+block_size;
  }
  close_file(&sra_cache);
  my_free(block,MYF(MY_WME));
  if (!my_stat(fname,&status,MYF(MY_WME)))
    die("%s failed to stat, but I had just closed it,\
 wonder how that happened");
  printf("Final size of %s is %s, wrote %d bytes\n",fname,
	 llstr(status.st_size,llstr_buf),
	 total_bytes);
  my_delete(fname, MYF(MY_WME));
  /* check correctness of tests */
  if (total_bytes != status.st_size)
  {
    fprintf(stderr,"Not the same number of bytes acutally  in file as bytes \
supposedly written\n");
    error=1;
  }
  exit(error);
  return 0;
}
#endif
