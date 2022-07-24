/**
 * @file SoftTimer.c
 * @author https://github.com/VKM96
 * @brief SoftTimer implementation 
 * @version 0.1
 * @date 2022-07-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */
///////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <assert.h>
#include "SoftTimer.h"
///////////////////////////////////////////////////////////////////////////////////////////////////////////

static sSoftTimer_t gSoftTimerTable[MAX_TIMERS] = {} ; ///< Array of type @ref sSoftTimer_t that holds all active timer

static bool gAreAllTimerStartIssued = false ; ///< Global that can be used to start or stop all timer. @see softTimer_PeriodicTask

/**
 * @brief Getter for @ref gAreAllTimerStartIssued
 * 
 * @return true 
 * @return false 
 */
static bool softTimer_GetStartStatus()
{
    return gAreAllTimerStartIssued;
} 

/**
 * @brief Utility function to check if timerID is valid 
 * 
 * @param timerId : unique ID set for each timer. @ref sSoftTimer_t
 * @return true 
 * @return false 
 */
static inline bool softTimer_isValidId(timerId_t timerId)
{
    return (timerId < MAX_TIMERS);
}

/**
 * @brief utility function to convert time in MS to ticks. @ref TIMER_RESOLUTION_MS
 * 
 * @param timeInMs : Input time in ms to be converted to ticks 
 * @return size_t : Ticks corresponding to timeInMs
 */
static inline size_t softTimer_GetTicksFromTime(size_t timeInMs)
{
    return (timeInMs/TIMER_RESOLUTION_MS);
}

/**
 * @brief utility function to return a timerID from gSoftTimerTable
 * 
 * @return timerId_t 
 */
static timerId_t softTimer_GetFreeSlot()
{
    timerId_t currentTimerId = TIMER_UNINITIALIZED_ID ;
    for(uint8_t i = 0 ; i < MAX_TIMERS ; i++)
    {
        if(false == gSoftTimerTable[i].isRegistered)
        {
            currentTimerId = i ;
            gSoftTimerTable[i].isRegistered = true; 
            break; 
        }
    }

    return currentTimerId;
}

/**
 * @brief utility function to unInitialise the timer associated with timerId
 * 
 * @param timerId : timer to unInitialise
 * @return eSoftTimerStatus_t : always success @todo : Update section
 */
static eSoftTimerStatus_t softTimer_unInitialize(timerId_t timerId)
{
    eSoftTimerStatus_t status = e_TIMER_STATUS_SUCCESS ;

    gSoftTimerTable[timerId].isOneShot = false ;
    gSoftTimerTable[timerId].isArmed = false ;
    gSoftTimerTable[timerId].isRegistered = false;

    gSoftTimerTable[timerId].timeOutInTicks = 0 ;
    gSoftTimerTable[timerId].timerID = 0 ;
    gSoftTimerTable[timerId].timeLeftInTicks = 0 ;
    gSoftTimerTable[timerId].registeredCallBackFunction = NULL ;

    return status;
}

/**
 * @brief Function to rearm timer in @ref gSoftTimerTable
 * If the timer is one shot timer, it is not rearmed 
 * Else the timer is rearmed and the time left is updated with timeout again
 * 
 * @param pMe : pointer to timer structure from @ref gSoftTimerTable. @see sSoftTimer_t
 */
static void softTimer_rearm(sSoftTimer_t* const pMe)
{
    assert(NULL != pMe);

    bool isOneShot = pMe->isOneShot ;
    if(isOneShot)
    {
        pMe->isArmed = false;
        pMe->timeLeftInTicks = 0 ;
    }
    else
    {
        pMe->isArmed = true;
        pMe->timeLeftInTicks = pMe->timeOutInTicks ;
    }

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Starts all timer by setting gAreAllTimerStartIssued. @see softTimer_PeriodicTask
 * 
 */
void softTimer_StartAll()
{
    gAreAllTimerStartIssued = true; 
}

/**
 * @brief stops all timer by setting gAreAllTimerStartIssued. @see softTimer_PeriodicTask
 * 
 */
void softTimer_StopAll()
{
    gAreAllTimerStartIssued = false; 
}

/**
 * @brief Initialization function for softTimer entries 
 * 
 * @param isOneShot : true if non recurring timer is needed 
 * @param timeOutInMs : required timeout in ms. Internally converted to ticks based on @ref TIMER_RESOLUTION_MS
 * @param pCallbackFunction : call back function to be invoked on timer expiry
 * @return timerId_t : unique ID between 0 and @ref MAX_TIMERS, to control the timer 
 */
timerId_t softTimer_Register(bool isOneShot, size_t timeOutInMs, pf_callBack pCallbackFunction)
{
    assert(timeOutInMs >= TIMER_RESOLUTION_MS);

    timerId_t assignedID= softTimer_GetFreeSlot();
    if(MAX_TIMERS != assignedID)
    {
        gSoftTimerTable[assignedID].isOneShot = isOneShot ;
        gSoftTimerTable[assignedID].isArmed = true ;
        gSoftTimerTable[assignedID].isRegistered = true;

        gSoftTimerTable[assignedID].timerID = assignedID ;
        gSoftTimerTable[assignedID].timeOutInTicks = softTimer_GetTicksFromTime(timeOutInMs) ;
        gSoftTimerTable[assignedID].timeLeftInTicks = softTimer_GetTicksFromTime(timeOutInMs) ;
        gSoftTimerTable[assignedID].registeredCallBackFunction = pCallbackFunction ;
    }
    return assignedID ;
}

/**
 * @brief Removes timer from @ref gSoftTimerTable and frees up the timerID
 * 
 * @param timerId : To identify the timer 
 * @return eSoftTimerStatus_t 
 */
eSoftTimerStatus_t softTimer_Deregister(timerId_t timerId)
{
    eSoftTimerStatus_t status = e_TIMER_INVALID_ID ;
    if(true == softTimer_isValidId(timerId))
    {
        status = softTimer_unInitialize(timerId);
    }
    return status; 
}

/**
 * @brief This function needs to be called periodically at @ref TIMER_RESOLUTION_MS through a hardware timer 
 * 
 */
void softTimer_PeriodicTask()
{
    ///< Continue only if @ref softTimer_StartAll has been issued
    if(true == softTimer_GetStartStatus())
    {
        bool isRegistered = false; 
        bool isArmed = false ;
        bool isCallBackPresent = false ;

        ///< Iterate over all entries in gSoftTimerTable
        for(uint8_t i = 0 ; i < MAX_TIMERS ; i++)
        {
            isRegistered = gSoftTimerTable[i].isRegistered ;
            isArmed = gSoftTimerTable[i].isArmed ;
            isCallBackPresent = (NULL != gSoftTimerTable[i].registeredCallBackFunction ) ;

            ///< Ensure that the timer exists, is armed amd has a callback registered
            if( (true == isRegistered) && (true == isArmed) && (true == isCallBackPresent) )
            {
                ///< Decrement time left by 1 tick
                if(0 == --gSoftTimerTable[i].timeLeftInTicks )
                {
                    ///< Rearm timer and invoke registered call back 
                    softTimer_rearm(&(gSoftTimerTable[i]));
                    gSoftTimerTable[i].registeredCallBackFunction();
                }
            }
        }
    }
}

/**
 * @brief Start timer identified by timerId
 * 
 * @param timerId 
 * @return eSoftTimerStatus_t : e_TIMER_STATUS_SUCCESS if timer exists
 */
eSoftTimerStatus_t softTimer_Start(timerId_t timerId)
{
    eSoftTimerStatus_t status = e_TIMER_INVALID_ID ;
    if(true == softTimer_isValidId(timerId))
    {
        status = e_TIMER_STATUS_SUCCESS ;
        gSoftTimerTable[timerId].isArmed = true;
    }
    return status ;
}

/**
 * @brief Stop timer identified by timerId
 * 
 * @param timerId 
 * @return eSoftTimerStatus_t :  : e_TIMER_STATUS_SUCCESS if timer exists
 */
eSoftTimerStatus_t softTimer_Stop(timerId_t timerId)
{
    eSoftTimerStatus_t status = e_TIMER_INVALID_ID ;
    if(true == softTimer_isValidId(timerId))
    {
        status = e_TIMER_STATUS_SUCCESS ;
        gSoftTimerTable[timerId].isArmed = false;
    }
    return status ;
}
