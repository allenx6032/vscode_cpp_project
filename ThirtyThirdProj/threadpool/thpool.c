/*
 *                 GNU GENERAL PUBLIC LICENSE
 *                  Version 3, 29 June 2007
 *                 Copyright (C) 2016 XD.Yang
 *   Everyone is permitted to copy and distribute verbatim copies
 *   of this license document, but changing it is not allowed.
 */
#include <stdint.h>
#include <stdio.h>

#include "m_vector.h"
#include "thpool.h"

void m_sleep( long tms )
{
#ifdef WIN32
    Sleep(tms);
#else
    usleep(tms*1000);
#endif
}

void*   lanuch(void *arg)
{
    mthread *thread = (mthread*)arg;

    //each thread has a custom task queue given by main thread task manager
    while ( 1 )
    {
        if ( thread->_task->size == 0 )
        {
            m_sleep(100);
        }else{
            int i = 0, count = 0;
            Task    *task;

            pthread_mutex_lock(&thread->_lock);
            thread->_busy = 1;
            count = thread->_task->size;
            for ( ; i < count; i++ )
            {
                task = List_Read_Back(Task*,thread->_task);
                task->fn( task->arg );
                List_Erase_Back(thread->_task);
            }
            thread->_busy = 0;
            pthread_mutex_unlock(&thread->_lock);
        }
    }
}

void*   loop(void*arg)
{
    thPool  *pool = (thPool*)arg;
    int i = 0 , thcount = pool->_threads->_size;

    //main thread task manager
    while (1)
    {
        i = 0;
        if ( pool->_tasks->size == 0){
            m_sleep(100);
            continue;
        }
        for (;i<thcount;i++)
        {
            Task *task;
            mthread *thread = Line_Get(pool->_threads,i);
            //idle thread can get the task
            pthread_mutex_lock(&thread->_lock);
            if ( thread->_busy )
            {
                pthread_mutex_unlock(&thread->_lock);
                continue;
            }
            pthread_mutex_unlock(&thread->_lock);

            pthread_mutex_lock(&pool->_lock);
            //distribute tasks to idle thread
            if( pool->_tasks->size > 0 )
            {
                task = List_Read_Back(Task*,pool->_tasks);
                List_Push_Back(Task*,thread->_task,task);
                //remove distributed task
                List_Erase_Back(pool->_tasks);
            }
            pthread_mutex_unlock(&pool->_lock);
        }
    }
}

thPool  *thpool_create( int num )
{
    int     i = 0;
    thPool *pool    = (thPool*)malloc(sizeof(thPool));
    pthread_t       actor;

    pthread_mutex_init(&pool->_lock,NULL);

    pool->_threads  = Linear_New();
    Linear(pool->_threads,num);

    pool->_tasks    =   List_Create(Task*);

    //initialize thread-pool
    for ( ; i < num; ++i )
    {
        mthread *thd = (mthread*)calloc(1,sizeof(mthread));
        thd->_task = List_Create(Task*);
        pthread_mutex_init(&thd->_lock,NULL);
        Line_Append(pool->_threads,thd);
        pthread_create(&thd->_thread,NULL,lanuch,thd);
    }
    //start up task manager thread
    pthread_create(&actor,NULL,loop,pool);
    m_sleep(100);
    return pool;
}

void  add_Task(thPool *pool,Task *task)
{
    pthread_mutex_lock(&pool->_lock);
    List_Push_Back(Task*,pool->_tasks,task);
    pthread_mutex_unlock(&pool->_lock);
}

void thpool_release(thPool *pool)
{
    int i = 0 , num = 0;
    pthread_mutex_lock(&pool->_lock);
    num = pool->_threads->_size;
    for ( ; i < num; ++i )
    {
        mthread *thd = Line_Get(pool->_threads,i);
        List_Free(thd->_task);
        free(thd);

    }
    List_Free(pool->_tasks);
    free(pool);
    pthread_mutex_unlock(&pool->_lock);
}
