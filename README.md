SoftTimer                      {#mainPage}
============
    SoftTimer implementation suitable for resource constrained environments like embedded systems 

**Status**
1. Work in Progress 

**Code Structure**
1. Source files are present in @ref Src
2. Interface files are present in @ref inc
3. PLease refer to the generated HTML documentation in the Doc/html subfolder for implementation details
4. The function entry point is @ref Main.c

**Usage**
1. @ref softTimer_Register with timeout and call back to be invoked. Captured the returned variable as the ID that can be used to Disarm or Deregister the Timer
2. Update @ref MAX_TIMERS with the maximum timers needed. The memory is statically allocated though global table @ref gSoftTimerTable
2. @ref softTimer_PeriodicTask should be called by a hardware timer at the required resolution. All timers will be scheduled in multiples of this periodicity. Update Macro @ref TIMER_RESOLUTION_MS with this value
3. @ref softTimer_StartAll and @ref softTimer_StopAll can be used to start or stop the timers 

**Permissions**
1. PLease refer to the LICENSE file

**Contact Me**
1. You can reach out to me on LinkedIn here
https://www.linkedin.com/in/vishal-keshava-murthy-8a2ba1a7/

"# SoftTimer" 
