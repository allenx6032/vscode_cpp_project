/*
 * Copyright (c) scott.cgi All Rights Reserved.
 *
 * This source code belongs to project Mojoc, which is a pure C Game Engine hosted on GitHub.
 * The Mojoc Game Engine is licensed under the MIT License, and will continue to be iterated with coding passion.
 *
 * License  : https://github.com/scottcgi/Mojoc/blob/master/LICENSE
 * GitHub   : https://github.com/scottcgi/Mojoc
 * CodeStyle: https://github.com/scottcgi/Mojoc/blob/master/Docs/CodeStyle.md
 *
 * Since    : 2014-1-16
 * Update   : 2019-1-25
 * Author   : scott.cgi
 */


#ifndef SCHEDULER_H
#define SCHEDULER_H


#include <stdbool.h>
#include "MojoC_Define.h"
#include "MojoC_UserData.h"


typedef struct Scheduler Scheduler;


/**
 * Callback by interval time.
 *
 * scheduler   : caller
 * deltaSeconds: frame delta seconds
 */
typedef void (*SchedulerUpdate)(Scheduler* scheduler, float deltaSeconds);


/**
 * Schedule update function callback by interval time.
 */
struct Scheduler
{
    UserData        userData[1];

    /**
     * Schedule interval time for update method called.
     */
    float           intervalTime;

    /**
     * Whether cancel scheduler, default false.
     * once isCancel set true scheduler will be removed from callback list.
     */
    bool            isCancel;

    /**
     * Current schedule time.
     */
    float           currentTime;

    /**
     * Schedule callback.
     */
    SchedulerUpdate Update;
};


/**
 * Control Scheduler.
 */
struct AScheduler
{
    /**
     * Scheduler to be scheduled by intervalTime.
     */
    Scheduler* (*Schedule)    (SchedulerUpdate Update, float intervalTime);

    /**
     * Only schedule once by intervalTime.
     */
    Scheduler* (*ScheduleOnce)(SchedulerUpdate Update, float intervalTime);

    /**
     * Every frame update by loop.
     */
    void       (*Update)      (float deltaSeconds);
};


extern struct AScheduler AScheduler[1];


#endif
