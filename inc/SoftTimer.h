/**
 * @file SoftTimer.h
 * @author https://github.com/VKM96
 * @brief SoftTimer interface
 * @version 0.1
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#ifndef SOFT_TIMER_H
#define SOFT_TIMER_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#define MAX_TIMERS 10 ///< Maximum Timers that the library supports. Update as needed 
#define TIMER_RESOLUTION_MS 10 ///< timer resolution, this should match with the periodicity of function @ref softTimer_PeriodicTask
#define TIMER_UNINITIALIZED_ID MAX_TIMERS ///< Use this value for timerID initial values 
///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void (*pf_callBack)(void); ///< call back function to be invoked on callback after timer expiry 
typedef uint8_t timerId_t ; 
///////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief status enumeration 
 * 
 */
typedef enum
{
    e_TIMER_STATUS_SUCCESS = 0 ,
    e_TIMER_INVALID_ID,
    e_TIMER_SLOT_FULL,
    e_TIMER_STATUS_MAX,
}eSoftTimerStatus_t;

/**
 * @brief Timer object. The @ref gSoftTimerTable is an array of these structures 
 * 
 */
typedef struct 
{
    bool isRegistered; ///< Set to true is softTimer is registered/Initialized 
    bool isOneShot; ///< true if timer is non recurring.
    bool isArmed; ///< true is this timer is set to go off 
    timerId_t timerID; ///< Unique ID set to between 0 and @ref MAX_TIMERS excluding @ref MAX_TIMERS
    size_t timeOutInTicks; ///< Timeout for this timer saved as ticks. This value is computed at initialization through @ ref softTimer_GetTicksFromTime
    size_t timeLeftInTicks; ///< Ticks left till timer expiry, updated periodically in softTimer_PeriodicTask
    pf_callBack registeredCallBackFunction; ///< callback function invoked upton timer expiry 
}sSoftTimer_t;

///////////////////////////////////////////////////////////////////////////////////////////////////////////
timerId_t softTimer_Register(bool isOneShot, size_t timeOut, pf_callBack pCallbackFunction);
eSoftTimerStatus_t softTimer_Deregister(timerId_t timerId);
eSoftTimerStatus_t softTimer_Disarm(timerId_t timerId);
void softTimer_StartAll();
void softTimer_StopAll();
void softTimer_PeriodicTask();
///////////////////////////////////////////////////////////////////////////////////////////////////////////

#endif