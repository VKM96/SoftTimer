/**
 * @file Main.c
 * @author https://github.com/VKM96
 * @brief 
 * @version 0.1
 * @date 2022-07-17
 * 
 * 
 * @copyright Copyright (c) 2022
 * 
 */

///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "version.h"
#include "SoftTimer.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Dummy call back function. Use your own here 
 * 
 */
static void timer1_callBack()
{
    puts("\nTimer1_Callback");
}

/**
 * @brief Dummy call back function. Use your own here 
 * 
 */
static void timer2_callBack()
{
    puts("\nTimer2_Callback");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function entry point
 * 
 * 
 * @return int 
 */
int main()
{
    char buf[50] ; 
    Version_GetVersion(buf,sizeof(buf));
    printf("%s",buf);

    //< Register two timers
    timerId_t timer1 = TIMER_UNINITIALIZED_ID, timer2 = TIMER_UNINITIALIZED_ID;
    timer1 = softTimer_Register(false, 100, &timer1_callBack);
    timer2 = softTimer_Register(true, 150, &timer2_callBack);

    softTimer_StartAll();

    //< This function needs to be called using a hardware timer at every @ref TIMER_RESOLUTION_MS ms
    softTimer_PeriodicTask();
}