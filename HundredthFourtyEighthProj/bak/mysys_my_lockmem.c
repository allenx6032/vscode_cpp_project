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

/* Alloc a block of locked memory */

#include "mysys_priv.h"
#include "libmysql_mysys_err.h"
#include <libmysql_my_list.h>

#ifdef HAVE_MLOCK
#include <sys/mman.h>

struct st_mem_list
{
  LIST list;
  uchar *page;
  uint size;
};

LIST *mem_list;

void *my_malloc_lock(size_t size,myf MyFlags)
{
  int success;
  uint pagesize=sysconf(_SC_PAGESIZE);
  uchar *ptr;
  struct st_mem_list *element;
  DBUG_ENTER("my_malloc_lock");

  size=((size-1) & ~(pagesize-1))+pagesize;
#if HAVE_MEMALIGN
  if (!(ptr=memalign(pagesize,size)))
#else
  if (!(ptr=valloc(size)))
#endif
  {
    if (MyFlags & (MY_FAE+MY_WME))
      my_error(EE_OUTOFMEMORY, MYF(ME_BELL+ME_WAITTANG),size);
    DBUG_RETURN(0);
  }
  success = mlock((uchar*) ptr,size);
  if (success != 0 && geteuid() == 0)
  {
    DBUG_PRINT("warning",("Failed to lock memory. errno %d\n",
			  errno));
    fprintf(stderr, "Warning: Failed to lock memory. errno %d\n",
	    errno);
  }
  else
  {
    /* Add block in a list for munlock */
    if (!(element=(struct st_mem_list*) my_malloc(sizeof(*element),MyFlags)))
    {
      (void) munlock((uchar*) ptr,size);
      free(ptr);
      DBUG_RETURN(0);
    }
    element->list.data=(uchar*) element;
    element->page=ptr;
    element->size=size;
    pthread_mutex_lock(&THR_LOCK_malloc);
    mem_list=list_add(mem_list,&element->list);
    pthread_mutex_unlock(&THR_LOCK_malloc);
  }
  DBUG_RETURN(ptr);
}


void my_free_lock(void *ptr,myf Myflags __attribute__((unused)))
{
  LIST *list;
  struct st_mem_list *element=0;

  pthread_mutex_lock(&THR_LOCK_malloc);
  for (list=mem_list ; list ; list=list->next)
  {
    element=(struct st_mem_list*) list->data;
    if (ptr == element->page)
    {						/* Found locked mem */
      (void) munlock((uchar*) ptr,element->size);
      mem_list=list_delete(mem_list,list);
      break;
    }
  }
  pthread_mutex_unlock(&THR_LOCK_malloc);
  if (element)
    my_free((uchar*) element,MYF(0));
  free(ptr);					/* Free even if not locked */
}

#endif /* HAVE_MLOCK */
