/*****************************************************************************\
*                                                                             *
*  FILENAME:    time_api.c                                                    *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION: Implementation of several time handling functions.            *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  COPYRIGHT:    (c) 2025 Dipl.-Ing. Klaus Lux (Aachen, Germany)              *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  ORIGIN:       https://github/klux21/limitless_times                        *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
* Civil Usage Public License, Version 1.1, January 2024                       *
*                                                                             *
* Redistribution and use in source and binary forms, with or without          *
* modification, are permitted provided that the following conditions are met: *
*                                                                             *
* 1. Redistributions of source code must retain the above copyright           *
*    notice, this list of conditions, the explanation of terms                *
*    and the following disclaimer.                                            *
*                                                                             *
* 2. Redistributions in binary form must reproduce the above copyright        *
*    notice, this list of conditions and the following disclaimer in the      *
*    documentation or other materials provided with the distribution.         *
*                                                                             *
* 3. All modified files must carry prominent notices stating that the         *
*    files have been changed.                                                 *
*                                                                             *
* 4. The source code and binary forms and any derivative works are not        *
*    stored or executed in systems or devices which are designed or           *
*    intended to harm, to kill or to forcibly immobilize people.              *
*                                                                             *
* 5. The source code and binary forms and any derivative works are not        *
*    stored or executed in systems or devices which are intended to           *
*    monitor, to track, to change or to control the behavior, the             *
*    constitution, the location or the communication of any people or         *
*    their property without the explicit and prior agreement of those         *
*    people except those devices and systems are solely designed for          *
*    saving or protecting peoples life or health.                             *
*                                                                             *
* 6. The source code and binary forms and any derivative works are not        *
*    stored or executed in any systems or devices that are intended           *
*    for the production of any of the systems or devices that                 *
*    have been stated before except the ones for saving or protecting         *
*    peoples life or health only.                                             *
*                                                                             *
* The term 'systems' in all clauses shall include all types and combinations  *
* of physical, virtualized or simulated hardware and software and any kind    *
* of data storage.                                                            *
*                                                                             *
* The term 'devices' shall include any kind of local or non-local control     *
* system of the stated devices as part of that device als well. Any assembly  *
* of more than one device is one and the same device regarding this license.  *
*                                                                             *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" *
* AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   *
* IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  *
* ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   *
* LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         *
* CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        *
* SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    *
* INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     *
* CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     *
* ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  *
* POSSIBILITY OF SUCH DAMAGE.                                                 *
*                                                                             *
\*****************************************************************************/

#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>     /* open */
#include <stdio.h>     /* sprintf */
#include <string.h>
#include <stdlib.h>

#include <stddef.h>
#include <time.h>      /* struct tm and localtime_r */
#include <sys/types.h>

#ifdef _WIN32
#pragma warning(disable : 4100 4127)
#include <io.h>
#else
#include <unistd.h>
#include <sys/types.h>
#include <sys/times.h>

#ifdef __CYGWIN__
#include <windows.h>
#undef _WIN32
#endif
#endif/* #ifdef _WIN32 */

#ifdef _WIN32
#include <winsock2.h>  /* required for timeval struct */
#include <WS2tcpip.h>  /* for IPv6 related stuff */
#include <windows.h>
#include <winbase.h>

#pragma warning(disable : 4204)
#endif /* _WIN32 */

#ifdef __CYGWIN__
#include <windows.h>  /* required for struct timeval and LPFILETIME */
#endif


#include <time_api.h>


#if defined (_WIN32) || defined (__CYGWIN__)

/* ------------------------------------------------------------------------- *\
   unix_time returns the Unix time stamp in microsecond.
   (UTC time since 01/01/1970) The precision depends on the system.
\* ------------------------------------------------------------------------- */

static void (WINAPI * vGetSystemTimePreciseAsFileTime)(LPFILETIME lpSystemTimeAsFileTime);
static HMODULE hmKernel32Dll = (HMODULE) -1;

int64_t unix_time()
{
   int64_t iRet;
   FILETIME CurrentTime;

   if(vGetSystemTimePreciseAsFileTime)
   {
      vGetSystemTimePreciseAsFileTime(&CurrentTime);
   }
   else if(hmKernel32Dll == (HMODULE) -1)
   { /* 1rst call */
      hmKernel32Dll = LoadLibrary("Kernel32.dll");
      if(hmKernel32Dll)
         vGetSystemTimePreciseAsFileTime = (void (WINAPI * )(LPFILETIME)) GetProcAddress(hmKernel32Dll, "GetSystemTimePreciseAsFileTime");

      if(vGetSystemTimePreciseAsFileTime)
         vGetSystemTimePreciseAsFileTime(&CurrentTime);
      else
         GetSystemTimeAsFileTime(&CurrentTime);
   }
   else
   {
      GetSystemTimeAsFileTime(&CurrentTime);
   }

   iRet  = ((int64_t) CurrentTime.dwHighDateTime << 32);
   iRet += (int64_t)  CurrentTime.dwLowDateTime;
   iRet -= (int64_t)  116444736 * 1000000 * 1000; /* offset of Windows FileTime to start of Unix time */
   return (iRet / 10);
}/* int64_t unix_time() */

#else

int64_t unix_time()
{
   int64_t tRet;
   struct timeval tv;

   gettimeofday(&tv, NULL);

   tRet = (int64_t) tv.tv_sec;

   /* Try to turn the year 2038 problem into a year 2106 problem. */
   if((sizeof(tv.tv_sec) <= 4) && (tv.tv_sec < 0))
      tRet += (int64_t) 0x80000000ul + (int64_t) 0x80000000ul;

   tRet *= 1000000ul;
   tRet += tv.tv_usec;
   return (tRet);
}/* int64_t unix_time() */
#endif


/* ========================================================================= *\
   Thread safety helpers for the time functions
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   Optional thread lock stuff for ensuring thread safety in multi-threaded
   programs. The lock callbacks must be set before calling
   update_time_zone_info, new_mktime, get_local_zone_info or new_localtime_r.
   The context is a user defined pointer which is uses as argument of the
   callbacks. It can be a pointer to a global program mutex for instance.
   The provided pfn_lock and pfn_unlock functions are called for guarding
   the updates of the internal time zone information updates in
   multi-threaded programs but may slow down those functions according to the
   time the lock or unlock functions require. For disabling subsequent calls
   of the lock and unlock function e.g. before the program termination call
   set_time_api_lock with null pointer arguments instead of callback functions.
   The used mutex needs to be callable recursively.
   Because the function pointers are remembered in static variables you need
   to ensure to call this function in all of your program modules which don't
   share the same statics and ensure the usage of the same mutex in all of
   those modules.
   Be aware that the other standard C time functions beside of those
   functions aren't required to be thread safe implemented while changing the
   global time settings e.g. if changing the TZ environment variable of your
   process!
\* ------------------------------------------------------------------------- */

static TIME_API_LOCK pta_lock         = NULL;
static TIME_API_LOCK pta_unlock       = NULL;
static void *        pv_lock_context = NULL;

void init_time_api_lock(TIME_API_LOCK pfn_lock,   /* pointer to a user provided mutex lock callback function */
                        TIME_API_LOCK pfn_unlock, /* pointer to a user provided mutex unlock callback function */
                        void *        pv_context) /* user provided context, e.g. pointer to the mutex. */
{
   TIME_API_LOCK old_unlock;
   void *        old_context;

   if(pfn_lock)
      pfn_lock(pv_context);

   if(pta_lock)
      pta_lock(pv_lock_context);

   old_context     = pv_lock_context;
   old_unlock      = pta_unlock;

   pv_lock_context = pv_context;
   pta_lock        = pfn_lock;
   pta_unlock      = pfn_unlock;

   if(old_unlock)
      old_unlock(old_context);

   if(pfn_unlock)
      pfn_unlock(pv_context);
} /* void set_time_api_lock(...) */


/* ========================================================================= *\
   Routines for calculating calendar week of a given date
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   calendar_week_of_year returns the calender week of the year for a struct tm
\* ------------------------------------------------------------------------- */
int calendar_week_of_year(const struct tm * ptm)
{
   int wday_01_01;
   int kw_ret = 0;
   int epoch  = 0;
   int year   = 0;

   if(!ptm)
      goto Exit;

   year = ptm->tm_year + 1900;

   epoch = year / 400;
   if (year < 0)
      --epoch;
   year -= epoch * 400; /* year is between 0 and 399 now (every 400 years epoch epoch starts on a Saturday) */

   /* we have to subtract one because the calendar week starts
      on Monday instead on Sunday as in struct ptm specified. */
   wday_01_01 = (700 + ptm->tm_wday - ptm->tm_yday - 1) % 7;

   if(wday_01_01 >= 4)
      wday_01_01 -= 7; /* subtract a week because the 01/01 belongs to KW53 of last year. */

   kw_ret = (ptm->tm_yday + wday_01_01 + 7) / 7;

   if (kw_ret == 53)
   { /* KW53 is KW1 of next year if the 12/31 isn't a Thursday */
      long wday_12_31;

      if(!(year & 3) && ((year % 100) || !year))
          wday_12_31 = (wday_01_01 + 366) % 7; /* leap year */
      else
          wday_12_31 = (wday_01_01 + 365) % 7; /* normal year */

      if(wday_12_31 < 4)
         kw_ret = 1;
   }

   if(!kw_ret)
      kw_ret = 53;  /* KW0 indicates KW53 of previous year. */

   Exit:;
      return (kw_ret);
}/* calendar_week_of_year(...) */


/* ------------------------------------------------------------------------- *\
   week_of_year returns the calendar week of a given date
\* ------------------------------------------------------------------------- */
int week_of_year(int year, int month, int day)
{
   int kw_ret = 0;
   struct tm stm;
   time64_t timestamp;

   memset(&stm, 0, sizeof(stm)); /* clear data */

   if((month < 1) || (month > 12))
      month = 1;

   if((day < 1) || (day > 31))
      day = 1;

   stm.tm_year = year - 1900;
   stm.tm_mon  = month - 1;
   stm.tm_mday = day;
   stm.tm_hour = 11;

   timestamp = new_timegm(&stm);
   new_gmtime_r(timestamp, &stm);

   kw_ret = calendar_week_of_year(&stm);

   return (kw_ret);
}/* week_of_year(...) */


/* ------------------------------------------------------------------------- *\
   calendar_week_of_time returns the calender week of the year for a time_t
\* ------------------------------------------------------------------------- */
int calendar_week_of_time(time64_t tt)
{
   int kw_ret = 0;
   struct tm stm;

   memset(&stm, 0, sizeof(stm));
   new_gmtime_r(tt, &stm);
   kw_ret = calendar_week_of_year(&stm);

   return (kw_ret);
} /* int calendar_week_of_time(time64_t tt) */

/* ------------------------------------------------------------------------- *\
   Helper arrays for faster calculations
\* ------------------------------------------------------------------------- */

                                                 /* month 1   2   3   4   5     6    7    8    9   10   11   12 */
static const uint8_t  days_of_month_array[12]        = { 31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31 }; /* number of the days in the month */
static const uint16_t startday_of_month_array[12]    = {  0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }; /* offset of the first day of a month to the begin of the year that is not a leap year */
static const uint8_t  weekday_of_month_start[12]     = {  0,  3,  3,  6,   1,   4,   6,   2,   5,   0,   3,   5 }; /* offset of the weekday that the month starts with if not a leap year */

static const uint8_t  days_of_month_array_ly[12]     = { 31, 29, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31 }; /* number of the days in the month in a leap year */
static const uint16_t startday_of_month_array_ly[12] = {  0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }; /* offset of the first day of a month from the begin of the year in a leap year */
static const uint8_t  weekday_of_month_start_ly[12]  = {  0,  3,  4,  0,   2,   5,   0,   3,   6,   1,   4,   6 }; /* offset of the weekday that the month starts with in a leap year */


/* ------------------------------------------------------------------------- *\
   new_timegm is a timegm (mkgmtime) implementation that does not adjust
   any members of the input struct as timegm (mkgmtime) does.
\* ------------------------------------------------------------------------- */
time64_t new_timegm(const struct tm * ptm)
{
   int64_t tt = -1;
   int64_t year;
   int64_t epoch;
   int32_t time_of_year;
   int     leap_year = 0;

   if(!ptm)
   {
      errno = EINVAL;
      goto Exit;
   }

   if (   ((ptm->tm_sec  < 0) || (ptm->tm_sec  > 60)) /* allow specification  of a positive leap second */
       || ((ptm->tm_min  < 0) || (ptm->tm_min  > 59))
       || ((ptm->tm_hour < 0) || (ptm->tm_hour > 23))
       ||  (ptm->tm_mday < 1)
       || ((ptm->tm_mon  < 0) || (ptm->tm_mon  > 11)))
   {
      errno = ERANGE;
      goto Exit;
   }

   year = (int64_t) ptm->tm_year + 1900;
   epoch = year / 400;

   if (year < 0)
       --epoch;

   year -= epoch * 400; /* year is between 0 and 399 now */

   if (year >= 100)
   {
      if (year >= 300)
      {
         tt = (int64_t) 86400 * (36525 + 36524 + 36524);
         year -= 300;
      }
      else if (year >= 200)
      {
         tt = (int64_t) 86400 * (36525 + 36524);
         year -= 200;
      }
      else
      {
         tt = (int64_t) 86400 * 36525;
         year -= 100;
      }

      if (year >= 4)
      {
         tt += (int64_t) 86400 * (365 * 4); /* add the time of the first 4 years of the century */
         year -= 4;

         tt += (year >> 2) * (1461 * 86400); /* time of full four year epochs */
         year &= 3;

         if(year == 3)
            tt += (366 + 365 + 365) * 86400; /* add time till the year */
         else if(year == 2)
            tt += (366 + 365) * 86400; /* add time till the year */
         else if(year == 1)
            tt += 366 * 86400; /* add time of the leap year */
         else
            leap_year = 1;
      }
      else
      {
         tt += year * (86400 * 365); /* add the time of the full years */
      }
   }
   else
   {
      tt = (year >> 2) * (1461 * 86400); /* time of full four year epochs */
      year &= 3;

      if(year == 3)
         tt += (366 + 365 + 365) * 86400; /* add time till the year */
      else if(year == 2)
         tt += (366 + 365) * 86400; /* add time till the year */
      else if(year == 1)
         tt += 366 * 86400; /* add time of the leap year */
      else
         leap_year = 1;
   }

   if(!leap_year)
   {
      if (ptm->tm_mday > days_of_month_array[ptm->tm_mon])
      {
         errno = ERANGE;
         goto Exit;
      }

      time_of_year = (startday_of_month_array[ptm->tm_mon] + (ptm->tm_mday - 1)) * 86400;
   }
   else
   {
      if (ptm->tm_mday > days_of_month_array_ly[ptm->tm_mon])
      {
         errno = ERANGE;
         goto Exit;
      }

      time_of_year = (startday_of_month_array_ly[ptm->tm_mon] + (ptm->tm_mday - 1)) * 86400;
   }

   time_of_year += ptm->tm_sec;
   time_of_year += ptm->tm_min * 60;
   time_of_year += ptm->tm_hour * 3600;

   tt += epoch * ((int64_t) 146097 * 86400); /* time of the 400 year epochs */
   tt += (int64_t) time_of_year;
   tt -= (int64_t) 719528 * 86400; /* subtract the time from 1/1/0000 until 1/1/1970 */

   Exit:;

   return ((time64_t) tt);
} /* time64_t new_timegm(struct tm * ptm) */


/* ------------------------------------------------------------------------- *\
   std_timegm is a timegm (mkgmtime) implementation that adjusts the members
   of the input struct as the C standard requires.
\* ------------------------------------------------------------------------- */

time64_t std_timegm(struct tm * ptm)
{
   time64_t t_ret = new_timegm(ptm);

   if(t_ret != (time64_t) -1)
   {
      new_gmtime_r(t_ret, ptm);
   }
   else
   { /* We must not call new_gmtime_r after a conversion error and
        preserve errno if there was no conversion error. */

      int err = errno;
      errno = 0;

      t_ret = new_timegm(ptm);

      if(!errno)
         new_gmtime_r(t_ret, ptm);
      else
         errno = err;
   }

   return( t_ret);
} /* time64_t std_timegm(struct tm * ptm) */


/* ------------------------------------------------------------------------- *\
   new_gmtime_r an own implementation of gmtime_r

   Note: new_gmtime_r returns the atronomical date that has a year 0.
         If you need the historical date you can do this as following
   ...
   new_gmtime_r(&t, ptm);
   if(ptm->year <= -1900)
      --ptm->year;
   printf ( "The historical year was %s%i%s", ptm->year > -1900 ? "AD" : "",
            ptm->year + 1900, ptm->year < -1900 ? " BC" : "");
   ...
\* ------------------------------------------------------------------------- */
struct tm * new_gmtime_r(time64_t t, struct tm * ptm)
{
   int64_t  time = t; /* unix time in micro seconds */
   int64_t  year;
   uint32_t day;
   uint32_t time_of_day;
   uint32_t tmp;
   int      leap_year = 1;

   static uint8_t mday[366] = /* day of a month of in a leap year */
   {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31};

   static uint8_t mon[366] = /* month of a day in a leap year */
   { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
     3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
     4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
     5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
     6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
     7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
     8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8, 8,
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,10,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11};

   if(!ptm)
   {
      errno = EINVAL;
      goto Exit; /* destination missing */
   }

   memset(ptm, 0, sizeof(*ptm));

   /* ptm->tm_isdst = 0; -> ignore summer time flag for UTC */
   time += ((int64_t) 719528 * 86400); /* add the time from 1/1/0000 to 1/1/1970 */

   if (time < 0)
      year = time / ((int64_t) 146097 * 86400) - 1; /* calculate the number of full 400 year epochs BC  */
   else
      year = time / ((int64_t) 146097 * 86400); /* calculate the number of full 400 year epochs */

   time -= year * ((int64_t) 146097 * 86400); /* subtract the 400 year epochs from time */
   year *= 400;  /* year contains the Gregorian 400 year epoch of the time now e.g 400 for 753 AD */
   day   = (uint32_t) (time / 86400); /* calculate the days within the epoch */
   time_of_day = (uint32_t) (time - (int64_t) day * 86400); /* time of the day in seconds */
   ptm->tm_wday = (day + 6 /* 6 is offset at 1.1.0000 */) % 7; /* day of the week the year starts with 0=Sunday ... 6=Saturday */

   if (day >= 36525)
   { /* if the time is more than 100 years after the start of a 400 years epoch */
      day -= 36525;
      year += 100;

      if (day >= 36524)
      {
         day -= 36524;
         year += 100;

         if (day >= 36524)
         {
            day -= 36524;
            year += 100;
         }
      }

      /* handle the first non leap years at begin of the century and ensure all remaining 4 year epochs start with a leap year */
      if (day >= 1460)
      {
         year += 4;
         day -= 1460;
      }
      else
      {
         leap_year = 0;

         while (day >= 365)
         {
            ++year;
            day -= 365;
         }
      }
   }

   /* now we handle the rest of the years */
   tmp = (day / 1461); /* calculate the number of full 4 year epochs that start with a leap year */
   year += tmp * 4;
   day -= tmp * 1461;

   if (day >= 1096)
   { /* last year of the remaining 4 year block */
      year += 3;
      day -= 1096;
      leap_year = 0;
   }
   else if (day >= 731)
   { /* 3rd year of the 4 year block */
      year += 2;
      day -= 731;
      leap_year = 0;
   }
   else if (day >= 366)
   { /* 2nd year of the 4 year block */
      year += 1;
      day -= 366;
      leap_year = 0;
   }

   year -=  1900;

   ptm->tm_year = (int) year;
   ptm->tm_yday = (int) day;

   if (!leap_year && (day >= 59))
      ++day; /* we have to skip the 29th of February in our tables */

   ptm->tm_mon  = mon[day];
   ptm->tm_mday = mday[day];

   ptm->tm_hour = (time_of_day / 3600);
   time_of_day %= 3600;
   ptm->tm_min  = time_of_day / 60;
   ptm->tm_sec  = time_of_day % 60;

   ptm->tm_isdst  = 0;

#if defined __TM_ZONE || (defined (_POSIX_VERSION) && (_POSIX_VERSION  >= 202405))
   ptm->tm_gmtoff = 0;
   ptm->tm_zone   = "UTC";
#endif

   if (year != (int) year)
   {
#ifdef EOVERFLOW
      errno = EOVERFLOW;
#else
      errno = ERANGE;
#endif
      ptm = NULL;
      goto Exit;
   }

Exit:;
   return (ptm);
}/* struct tm * new_gmtime_r(time64_t t, struct tm * ptm) */


static TIME_ZONE_INFO ti;   /* static time zone information as returned by the system functions */


/* ------------------------------------------------------------------------- *\
   get_rule_offset is a helper function that calculates the time in seconds
   that a given daylight saving rule applies after the begin of the year.
   Beside of specific time zone rule it needs the information whether that
   year is a leap year and which day of the week that year begins.
\* ------------------------------------------------------------------------- */

static int32_t get_rule_offset(const TIME_ZONE_RULE * ptz, int leap_year, int wday_year_start)
{
   int32_t switchday; /* day after begin of the year when the rule applies */

   if (ptz->mode > 0)
   { /* This modes are part of the Posix standard only but currently not used in the timezone database */
      switchday = ptz->year_day;

      if ((ptz->mode == 1) && leap_year && (switchday >= 59))
         ++switchday; /* we have to ignore the 29th of February */
   }
   else
   { /* This part calculates the start day according to the common daylight saving rules. */
      int32_t days_of_month;
      int32_t startday_of_month;
      int32_t wday_month_start;

      int32_t month         = ptz->month; /* index of the month */
      int32_t week_of_month = ptz->mweek;

      if (!leap_year)
      {
         days_of_month     = days_of_month_array[month];
         startday_of_month = startday_of_month_array[month];
         wday_month_start  = weekday_of_month_start[month] + wday_year_start;
      }
      else
      {
         days_of_month     = days_of_month_array_ly[month];
         startday_of_month = startday_of_month_array_ly[month];
         wday_month_start  = weekday_of_month_start_ly[month] + wday_year_start;
      }

      if (wday_month_start >= 7)
         wday_month_start -= 7; /* (wday_year_start + weekday_of_month_start[month]) % 7; */

      if (wday_month_start > (int32_t)ptz->wday)
         switchday = ptz->wday + 7 - wday_month_start; /* set switchday to the index of the first matching day of week within the month */
      else
         switchday = ptz->wday - wday_month_start; /* set switchday to the index of the first matching day of week within the month */

      days_of_month -=  7; /* maximum number of days because the switchday increased by 7 needs to remain inside of that month */

      while ((--week_of_month) && (switchday < days_of_month))
         switchday += 7;

      switchday += startday_of_month; /* start day of daylight saving within the year */
   }

   return ((switchday * 86400) + ptz->time);
} /* int32_t get_rule_offset(...) */


/* ------------------------------------------------------------------------- *\
   init_tz_rule_offsets is precalculates the 14 offsets in seconds after begin of
   a year that a rules applies to speedup the time calculations afterwards.
\* ------------------------------------------------------------------------- */

static void init_tz_rule_offsets(TIME_ZONE_RULE * ptz)
{
   int32_t * poff = ptz->start;
   int       wday = 0;

   do
   {
      poff[0] = get_rule_offset(ptz, 0, wday);
      poff[7] = get_rule_offset(ptz, 1, wday);
      ++poff;
   }
   while(++wday < 7);
} /* void init_tz_rule_offsets(TIME_ZONE_RULE * ptz) */


/* ------------------------------------------------------------------------- *\
   read_TZ_zone_data is a helper of read_TZ for reading time zone name and
   time offsets
\* ------------------------------------------------------------------------- */

static int read_TZ_zone_data (TIME_ZONE_RULE * ptr, int is_dst, char * psrc, char ** ppend)
{
   int bRet = 0;
   size_t   size    = 0;
   int32_t  offset  = 1;
   uint32_t hours   = 0;
   uint32_t minutes = 0;
   uint32_t seconds = 0;
   char * ps = psrc;

   if (*ps == '<')
   {
       while(*ps && (*ps != '>'))
          ++ps;

       if(*ps++ != '>')
           goto Exit; /* invalid format because the required termination of the name string is missing */
   }
   else
   {
      while(*ps && (*ps != ',') && (*ps != ':') && (*ps != '-') && (*ps != '+') && ((*ps < '0') || (*ps > '9')))
         ++ps;
   }

   if(!*ps || ((ps - psrc) < 3))
       goto Exit; /* time zone name too short or either time specification or rules missing */

   size = ps - psrc; /* time zone name */

   if((*ps != '-') && (*ps != '+') && ((*ps < '0') || (*ps > '9')))
   { /* no time specified */
      offset = 0x7fffffff;
      bRet   = is_dst;  /* in case of the summer time the specification of an offset is optional */
      goto Exit;
   }

   if(*ps == '+')
   {
      offset = 1;
      ++ps;
   }
   else if(*ps == '-')
   {
      offset = -1;
      ++ps;
   }

   if((*ps < '0') || (*ps > '9'))
      goto Exit; /* invalid format */

   hours = *ps++ - '0';

   if ((*ps >= '0') && (*ps <= '9'))
      hours = (hours * 10) + (*ps++ - '0');

   if(hours > 24)
       goto Exit; /* TZ format error :o( */

   if(*ps == ':')
   { /* minutes of the offset specified */
      ++ps;

      if ((*ps < '0') || (*ps > '9'))
         goto Exit; /* TZ format error :o( */

      minutes = *ps++ - '0';

      if ((*ps >= '0') && (*ps <= '9'))
         minutes = (minutes * 10) + (*ps++ - '0');

      if(minutes >= 59)
         goto Exit; /* TZ format error :o( */

      if(*ps == ':')
      { /* seconds of the offset specified for what ever */
         ++ps;

         if ((*ps < '0') || (*ps > '9'))
            goto Exit; /* TZ format error :o( */

         seconds = *ps++ - '0';

         if ((*ps >= '0') && (*ps <= '9'))
            seconds = (seconds * 10) + (*ps++ - '0');

         if(seconds > 59)
            goto Exit; /* TZ format error :o( */
      }
   }

   offset *= hours * 3600 + minutes * 60 + seconds;

   bRet = 1;
   Exit:;

   if(bRet)
   {
      *ppend  = ps;

      ptr->bias = offset;

      if(size >= sizeof(ptr->zone_name))
         size = sizeof(ptr->zone_name) -1; /* limit the zone name to the size of our buffer */

      if(*psrc == '<')
      { /* we have to ignore the optional quotes of the tine zone name */
         ++psrc;
         size -= 2;
      }

      ps = ptr->zone_name;
      while(size--)
         *ps++ = *psrc++; /* copy zone name string */
      *ps = '\0';        /* terminate zone name string */
   }

   return (bRet);
}  /* read_TZ_zone_data */


/* ------------------------------------------------------------------------- *\
   read_TZ_rules is a helper of b_read_TZ for reading the daylight saving
   rules
\* ------------------------------------------------------------------------- */

static int read_TZ_rules (TIME_ZONE_RULE * ptr, char * psrc, char ** ppend)
{
   int     bRet     = 0;
   char *  ps       = psrc;
   int32_t mode     = 0;
   int32_t month    = 0;
   int32_t mweek    = 0;
   int32_t wday     = 0;
   int32_t year_day = 0;
   int32_t hour     = 2; /* default is 02:00:00 */
   int32_t minutes  = 0;
   int32_t seconds  = 0;

   if(*ps++ != ',')
       goto Exit;

   if(*ps == 'M')
   {
      ++ps;

      if ((*ps < '0') || (*ps > '9'))
         goto Exit; /* TZ format error :o( */

      month = *ps++ - '0';

      if ((*ps >= '0') && (*ps <= '9'))
          month = (month * 10) + (*ps++ - '0');

      if((month < 1) || (month > 12))
         goto Exit; /* TZ format error :o( */

      month--; /* January is 0 */

      if(*ps++ != '.')
         goto Exit; /* TZ format error :o( */

      if ((*ps < '1') || (*ps > '5'))
         goto Exit; /* TZ format error :o( */

      mweek = *ps++ - '0';

      if(*ps++ != '.')
         goto Exit; /* TZ format error :o( */

      if ((*ps < '0') || (*ps > '6'))
         goto Exit; /* TZ format error :o( */

      wday = *ps++ - '0';

      mode = 0;
   }
   else if(*ps == 'J')
   {
      ++ps;

      if ((*ps < '0') || (*ps > '9'))
         goto Exit; /* TZ format error :o( */

      year_day = *ps++ - '0';

      if ((*ps >= '0') && (*ps <= '9'))
          year_day = (year_day * 10) + (*ps++ - '0');

      if ((*ps >= '0') && (*ps <= '9'))
          year_day = (year_day * 10) + (*ps++ - '0');

      if(year_day > 365)  /* 364 is maximum but J365/23 is used for termination of daylight saving or a previous rule as a day that never comes */
         goto Exit; /* TZ format error :o( */

      mode = 1;
   }
   else
   {
      if ((*ps < '0') || (*ps > '9'))
         goto Exit; /* TZ format error :o( */

      year_day = *ps++ - '0';

      if ((*ps >= '0') && (*ps <= '9'))
          year_day = (year_day * 10) + (*ps++ - '0');

      if ((*ps >= '0') && (*ps <= '9'))
          year_day = (year_day * 10) + (*ps++ - '0');

      if(year_day > 365)
         goto Exit; /* TZ format error :o( */

      mode = 2;
   }

   if(*ps == '/')
   { /* activation time specified */
      int32_t sign = 0;

      ++ps;

      if(*ps == '-')
      {
         sign = 1;
         ++ps;
      }

      if ((*ps < '0') || (*ps > '9'))
         goto Exit; /* TZ format error :o( */

      hour = *ps++ - '0';

      if ((*ps >= '0') && (*ps <= '9'))
         hour = (hour * 10) + (*ps++ - '0');

      if ((*ps >= '0') && (*ps <= '9'))
         hour = (hour * 10) + (*ps++ - '0');

      if(hour > 167)
         goto Exit;

      if(*ps == ':')
      { /* minutes specified */
         ++ps;

         if ((*ps < '0') || (*ps > '9'))
            goto Exit; /* TZ format error :o( */

         minutes = *ps++ - '0';

         if ((*ps >= '0') && (*ps <= '9'))
            minutes = (minutes * 10) + (*ps++ - '0');

         if(minutes > 59)
            goto Exit;

         if(*ps == ':')
         { /* seconds specified */
            ++ps;

            if ((*ps < '0') || (*ps > '9'))
               goto Exit; /* TZ format error :o( */

            seconds = *ps++ - '0';

            if ((*ps >= '0') && (*ps <= '9'))
               seconds = (seconds * 10) + (*ps++ - '0');

            if(seconds > 59)
               goto Exit;
         }
      }

      if(sign)
      {
         hour    *= -1;
         seconds *= -1;
         minutes *= -1;
      }
   }

   ptr->mode     = mode;
   ptr->year_day = year_day;
   ptr->month    = month;
   ptr->mweek    = mweek;
   ptr->wday     = wday;
   ptr->time     = (hour * 3600) + (minutes * 60) + seconds;

   init_tz_rule_offsets(ptr);

   *ppend = ps;

   bRet = 1;
   Exit:;

   return (bRet);
}  /* read_TZ_rules */


/* ------------------------------------------------------------------------- *\
   read_TZ parses a Unix conform TZ evironment variable conform string for
   the time zone rules and stores this rules in success case in a struct
   TIME_ZONE_INFO. The function returns nonzero in success case only.
   If the function fails because of an invalid string then the storage that
   ptzi points to is unchanged.
\* ------------------------------------------------------------------------- */

int read_TZ (TIME_ZONE_INFO * pzi, const char * pTZ)
{
   int     bRet = 0;
   TIME_ZONE_INFO zi;
   char * ps = (char *) pTZ;

   memset(&zi, 0, sizeof(zi));

   if (!pzi || !ps)
      goto Exit; /* internal program error :o( */

   if (*ps == ':')
      goto Exit; /* unknown user defined TZ format according to the Unix standard :o( */

   /* Valid TZ format sample for Central European Time "CET-1CEST,M3.5.0,M10.5.0/3"
      Please check the unix standard for the format description. */

   if(!read_TZ_zone_data (&zi.standard, 0 /* STD time */, ps, &ps))
       goto Exit; /* TZ format error :o( */

   zi.type = 1; /* standard time if there is no following data */

   if(read_TZ_zone_data (&zi.daylight, 1 /* STD time */, ps, &ps))
   {
       if(zi.daylight.bias == 0x7fffffff)
          zi.daylight.bias = zi.standard.bias - 3600;

       if(!read_TZ_rules (&zi.daylight, ps, &ps))
           goto Exit; /* TZ format error :o( */

       if(!read_TZ_rules (&zi.standard, ps, &ps))
           goto Exit; /* TZ format error :o( */

       zi.type = 2; /* standard time if there is no following data */
   }

   pzi->type = 0;
   *pzi = zi;
   bRet = 1;

   Exit:;
   return(bRet);
} /* read_TZ() */


/* ------------------------------------------------------------------------- *\
   update_time_zone_info initializes or reinitializes the timezone information
   that is used for new_mktime and new_localtime_r according to the current
   system settings. The function is not thread safe regarding our usage of
   static timezone information and requires TZ being set as specified in the
   Unix standard.
\* ------------------------------------------------------------------------- */

void update_time_zone_info()
{/* Get time zone information from system */
   static char last_TZ[128]="###";
   char * pTZ = getenv("TZ");
   struct stat st;

   if(pta_lock)
      pta_lock(pv_lock_context);

   if(!pTZ)
   {
      tzset(); /* mktime should call that. */
      pTZ = getenv("TZ");
   }

   if(pTZ)
   {
      if(!strncmp(pTZ, last_TZ, sizeof(last_TZ) - 1))
          goto Exit; /* timezone unchanged */

      strncpy(last_TZ, pTZ, sizeof(last_TZ) - 1);

      if (read_TZ(&ti, pTZ))
         goto Exit;
   }

#ifndef _WIN32
   if(!pTZ || !*pTZ)
   {
      pTZ = "/etc/localtime";
      if(stat(pTZ, &st) || !(st.st_mode & S_IFREG))
          pTZ = NULL;
   }
#endif

   if(pTZ && (*pTZ != ':'))
   { /* Check whether the specified timezone can be found in timezone database */
      char buf[0x2000];

      if((*pTZ == '/') || (*pTZ == '.'))
         sprintf(buf, "%.*s", (int)(sizeof(buf) - 1), pTZ); /* assume pTZ contains a path to the file that contains the timezone information */
      else
#ifndef _WIN32
         sprintf(buf, "/usr/share/zoneinfo/%.*s", (int)(sizeof(buf) - 32), pTZ);
#else
         sprintf(buf, "C:/cygwin64/usr/share/zoneinfo/%.*s", (int)(sizeof(buf) - 40), pTZ);
#endif

      if(!stat(buf, &st) && (st.st_mode & S_IFREG) && (st.st_size < sizeof(buf)))
      {
         int fd;
         do
         {
#ifndef O_BINARY
#define O_BINARY 0
#endif
            fd = open(buf, O_RDONLY | O_BINARY, 0);
         } while ((fd == -1) && (errno == EINTR));

         if(fd != -1)
         {
            ptrdiff_t size;
            char * ps = buf;
            char * pz = buf;
            char * pe = buf;

            do
            {
               size = read(fd, buf, (unsigned int) sizeof(buf));
            } while((size == -1) && (errno == EINTR));

            while(size-- > 0)
            {
               if(*ps++ == '\n')
               {
                  pz = pe;
                  pe = ps;
               }
            }

            do
            {
               size = close(fd);
            } while((size == -1) && (errno == EINTR));

            if((pz != buf) && (pz != pe))
            {
               *(pe - 1) = '\0';
               if (read_TZ(&ti, pz))
                  goto Exit;
            }
         }
      }
   }

   /* TZ not set or invalid. Try to find system specific infos. */
#if defined(_WIN32) || defined (__CYGWIN__)
   {
      TIME_ZONE_INFORMATION tzi;
      WCHAR * pwn = tzi.StandardName;
      CHAR *  pn  = ti.standard.zone_name;
      size_t  count = sizeof(ti.standard.zone_name) - 1;

      memset (&ti,  0, sizeof(ti));
      memset (&tzi, 0, sizeof(tzi));

      if (GetTimeZoneInformation(&tzi))
      {
         ti.standard.bias = (tzi.Bias + tzi.StandardBias) * 60;
         ti.daylight.bias = (tzi.Bias + tzi.DaylightBias) * 60;

         ti.standard.mode  = 0;
         ti.standard.month = tzi.StandardDate.wMonth - 1;
         ti.standard.mweek = tzi.StandardDate.wDay;
         ti.standard.wday  = tzi.StandardDate.wDayOfWeek;
         ti.standard.time  = tzi.StandardDate.wHour * 3600;

         ti.daylight.mode  = 0;
         ti.daylight.month = tzi.DaylightDate.wMonth - 1;
         ti.daylight.mweek = tzi.DaylightDate.wDay;
         ti.daylight.wday  = tzi.DaylightDate.wDayOfWeek;
         ti.daylight.time  = tzi.DaylightDate.wHour * 3600;

         if(ti.daylight.bias == ti.standard.bias)
         {
            ti.type = 1;
         }
         else
         {
            ti.type = 2;

            init_tz_rule_offsets(&ti.standard);
            init_tz_rule_offsets(&ti.daylight);
         }
      }
      else
      {
         ti.standard.bias = tzi.Bias * -60;
         ti.daylight.bias = ti.standard.bias;
         ti.type = 1; /* standard time only */
      }

      while(count--)
      {
         *pn = (char) *pwn;
         if(!*pn || (*pwn > 255))
            break;
         ++pn;
         ++pwn;
      }
      *pn ='\0'; /* ensure string termination */

      pwn   = tzi.DaylightName;
      pn    = ti.daylight.zone_name;
      count = sizeof(ti.daylight.zone_name) - 1;

      while(count--)
      {
         *pn = (char) *pwn;
         if(!*pn || (*pwn > 255))
            break;
         ++pn;
         ++pwn;
      }
      *pn ='\0'; /* ensure string termination */
   }
#else

   /* default to UTC without any daylight saving nor time offsets */
   memset(&ti, 0, sizeof(ti));
   strcpy(ti.standard.zone_name, "UTC");
   ti.type = 1;
#endif /* _WIN32 */

   Exit:;

   if(pta_unlock)
      pta_unlock(pv_lock_context);
} /* void update_time_zone_info() */


/* ------------------------------------------------------------------------- *\
   mktime_of_zone is a thread safe mktime implementation for any timezone
   where the daylight saving rules are given in a struct TIME_ZONE_INFO
\* ------------------------------------------------------------------------- */
time64_t mktime_of_zone(const struct tm * ptm, const TIME_ZONE_INFO * ptzi)
{
   int64_t tt = -1;
   int32_t leap_year = 0;
   int32_t isDaylightSaving;
   int64_t year;
   int64_t epoch;
   int32_t time_of_year;
   int32_t startday_of_month;
   int32_t days_of_month;

   if(!ptm)
   {
      errno = EINVAL;
      goto Exit;
   }

   if (   ((ptm->tm_sec  < 0) || (ptm->tm_sec  > 60))
       || ((ptm->tm_min  < 0) || (ptm->tm_min  > 59))
       || ((ptm->tm_hour < 0) || (ptm->tm_hour > 23))
       ||  (ptm->tm_mday < 1)
       || ((ptm->tm_mon  < 0) || (ptm->tm_mon  > 11)))
   {
      errno = ERANGE;
      goto Exit;
   }

   year = (int64_t) ptm->tm_year + 1900;
   epoch = year / 400;

   if (year < 0)
       --epoch;

   year -= epoch * 400; /* year is between 0 and 399 now */

   if (year >= 100)
   {
      if (year >= 300)
      {
         tt = (int64_t) 86400 * (36525 + 36524 + 36524);
         year -= 300;
      }
      else if (year >= 200)
      {
         tt = (int64_t) 86400 * (36525 + 36524);
         year -= 200;
      }
      else
      {
         tt = (int64_t) 86400 * 36525;
         year -= 100;
      }

      if (year >= 4)
      {
         tt += (int64_t) 86400 * (365 * 4); /* add the time of the first 4 years of the century */
         year -= 4;

         tt += (year >> 2) * (1461 * 86400); /* time of full four year epochs */
         year &= 3;

         if(year == 3)
            tt += (366 + 365 + 365) * 86400; /* add time till the year */
         else if(year == 2)
            tt += (366 + 365) * 86400; /* add time till the year */
         else if(year == 1)
            tt += 366 * 86400; /* add time of the leap year */
         else
            leap_year = 1;
      }
      else
      {
         tt += year * (86400 * 365); /* add the time of the full years */
      }
   }
   else
   {
      tt = (year >> 2) * (1461 * 86400); /* time of full four year epochs */
      year &= 3;

      if(year == 3)
         tt += (366 + 365 + 365) * 86400; /* add time till the year */
      else if(year == 2)
         tt += (366 + 365) * 86400; /* add time till the year */
      else if(year == 1)
         tt += 366 * 86400; /* add time of the leap year */
      else
         leap_year = 1;
   }

   if(!leap_year)
   {
      days_of_month     = days_of_month_array[ptm->tm_mon];
      startday_of_month = startday_of_month_array[ptm->tm_mon];
   }
   else
   {
      days_of_month     = days_of_month_array_ly[ptm->tm_mon];
      startday_of_month = startday_of_month_array_ly[ptm->tm_mon];
   }

   if (ptm->tm_mday > days_of_month)
   {
      errno = ERANGE;
      goto Exit;
   }

   time_of_year = (startday_of_month + (ptm->tm_mday - 1)) * 86400;

   time_of_year += ptm->tm_sec;
   time_of_year += ptm->tm_min * 60;
   time_of_year += ptm->tm_hour * 3600;

   isDaylightSaving = ptm->tm_isdst;

   if((isDaylightSaving < 0) && (ptzi->type > 1))
   {
      /* The day of week calculation works well for years before 0 as well because every 400 year epoch starts with the same day of week */
      int32_t wday_year_start = (int32_t)(((tt / 86400) + 6 /* 6 is offset at 1/1/0000 */) % 7); /* day of the week the year starts with 0=Sunday 1= Monday ... */
      int32_t daylight_start = ptzi->daylight.start[wday_year_start + (leap_year * 7)]; /* time offset of begin of the daylight saving within the year in seconds */
      int32_t standard_start = ptzi->standard.start[wday_year_start + (leap_year * 7)] + (ptzi->daylight.bias - ptzi->standard.bias); /* time offset of returning to the standard time in the year in seconds */

      if (daylight_start > standard_start)
      {  /* southern hemisphere */
         if((time_of_year >= standard_start) && (time_of_year < daylight_start))
            tt += ptzi->standard.bias;
         else
            tt += ptzi->daylight.bias;
      }
      else
      {  /* northern hemisphere */
         if((time_of_year >= daylight_start) && (time_of_year < standard_start))
            tt += ptzi->daylight.bias;
         else
            tt += ptzi->standard.bias;
      }
   }
   else
   {
      if(isDaylightSaving && (ptzi->type > 1))
         tt += ptzi->daylight.bias;
      else
         tt += ptzi->standard.bias;
   }

   tt += epoch * ((int64_t) 146097 * 86400); /* time of the 400 year epochs */
   tt += (int64_t) time_of_year;
   tt -= (int64_t) 719528 * 86400; /* subtract the time from 1/1/0000 to 1/1/1970 */

   Exit:;
   return ((time64_t) tt);
} /* time64_t mktime_of_zone(const struct tm * ptm, const TIME_ZONE_INFO * ptzi) */


/* ------------------------------------------------------------------------- *\
   new_mktime is a mktime implementation that does not adjust any members of
   the input struct as mktime does.
\* ------------------------------------------------------------------------- */
time64_t new_mktime(const struct tm * ptm)
{
   time64_t t_ret;

   if(pta_lock)
      pta_lock(pv_lock_context);

   if(!ti.type)
      update_time_zone_info();

   t_ret = mktime_of_zone(ptm, &ti);

   if(pta_unlock)
      pta_unlock(pv_lock_context);

   return(t_ret);
} /* time64_t new_mktime(struct tm * ptm) */


/* ------------------------------------------------------------------------- *\
   std_mktime is a mktime implementation that adjust the members of the
   input struct as the C standard requires.
\* ------------------------------------------------------------------------- */
time64_t std_mktime(struct tm * ptm)
{
   time64_t t_ret;

   if(pta_lock)
      pta_lock(pv_lock_context);

   if(!ti.type)
      update_time_zone_info();

   t_ret = mktime_of_zone(ptm, &ti);

   if(t_ret != (time64_t) -1)
   {
      localtime_of_zone(t_ret, ptm, &ti);
   }
   else
   { /* We must not call localtime_of_zoner after a conversion error and
        preserve errno if there was no conversion error. */

      int err = errno;
      errno = 0;

      t_ret = mktime_of_zone(ptm, &ti);

      if(!errno)
         localtime_of_zone(t_ret, ptm, &ti);
      else
         errno = err;
   }

   if(pta_unlock)
      pta_unlock(pv_lock_context);

   return(t_ret);
} /* time64_t std_mktime(struct tm * ptm) */


/* ------------------------------------------------------------------------- *\
   localtime_of_zone is just multithreading safe version of localtime
   according to the time zone and daylight saving rules that are given
   in a struct TIME_ZONE_INFO

   Note: new_localtime_r returns the atronomical date that has a year 0.
         If you need the historical date you can do this as following
   ...
   new_localtime_r(&t, ptm);
   if(ptm->year <= -1900)
      --ptm->year;
   printf ( "The historical year was %s%i%s", ptm->year > -1900 ? "AD" : "",
            ptm->year + 1900, ptm->year < -1900 ? " BC" : "");
   ...

   In Posix 2024 systems ptm->tm_zone points to storage in the TIME_ZONE_INFO
   struct. It becomes invalid once the storage of ptzi is released or adjusted.
\* ------------------------------------------------------------------------- */
struct tm * localtime_of_zone(time64_t utc_time, struct tm * ptm, const TIME_ZONE_INFO * ptzi)
{
   const   TIME_ZONE_RULE * ptz;
   int32_t isDaylightSaving = 0;

   if (ptzi->type > 1)
   {
      int32_t  daylight_start; /* begin of day light saving in seconds after begin of the year */
      int32_t  standard_start; /* begin of standard time in seconds after begin of the year */
      int32_t  time_of_day;
      uint32_t day;
      int32_t  time_of_year;
      int32_t  wday_year_start;
      int32_t  leap_year = 1;

      int64_t  time = utc_time + ((int64_t) 719528 * 86400); /* add the time from 1/1/0000 to 1/1/1970 */
      int64_t  year;

      if (time < 0)
         year = time / ((int64_t) 146097 * 86400) - 1; /* calculate the 400 year epoche before that time */
      else
         year = time / ((int64_t) 146097 * 86400); /* calculate the 400 year epoche before that time */

      time -= year * ((int64_t) 146097 * 86400); /* time is between 0 an 400 AD now */
      day  = (uint32_t) (time / 86400); /* number of days within the 400 year epoch */
      time_of_day = (int32_t) (time - ((int64_t) day * 86400));

      if (day >= 36526)
      {  /* the time is more than 100 years after a full 400 year epoch */
         day -= 36526;

         if (day >= 36525)
         {
            day -= 36525;

            if (day >= 36525)
               day -= 36525;
         }

         /* handle the first non leap years at begin of the century and ensure all remaining 4 year epochs start with a leap year */
         if (day >= 1460)
         {
            day -= 1460;
         }
         else
         {
            leap_year = 0; /* we have to ignore the leap year within the first four years of a century */

            while (day >= 365)
               day -= 365;
         }
      }

      day %= 1461; /* days within 4 year epochs that start with a leap year */

      if (day >= 1096)
      { /* last year of the remaining 4 year block */
         day -= 1096;
         leap_year = 0;
      }
      else if (day >= 731)
      { /* 3rd year of the 4 year block */
         day -= 731;
         leap_year = 0;
      }
      else if (day >= 366)
      { /* 2nd year of the 4 year block */
         day -= 366;
         leap_year = 0;
      }

      time_of_year = ((int32_t) day * 86400) /* time between the begin of the day and the begin of the year */
                     + time_of_day;          /* time since begin of the day */

      /* The day of week calculation works well for years before 0 as well because every 400 year epoch starts with the same day of week */
      wday_year_start = (int32_t)((((time - time_of_year) / 86400) + 6 /* 6 is offset at 1/1/0000 */) % 7); /* day of the week the year starts with 0=Sunday 1= Monday ... */
      daylight_start  = ptzi->daylight.start[wday_year_start + (leap_year * 7)] + ptzi->standard.bias; /* time offset of begin of the daylight saving within the year in seconds */
      standard_start  = ptzi->standard.start[wday_year_start + (leap_year * 7)] + ptzi->daylight.bias; /* time offset of returning to the standard time in the year in seconds */

      if (daylight_start > standard_start)
      {  /* southern hemisphere */
         if((time_of_year >= standard_start) && (time_of_year < daylight_start))
         {
            ptz = &ptzi->standard;
         }
         else
         {
            ptz = &ptzi->daylight;
            isDaylightSaving = 1;
         }
      }
      else
      {  /* northern hemisphere */
         if((time_of_year >= daylight_start) && (time_of_year < standard_start))
         {
            ptz = &ptzi->daylight;
            isDaylightSaving = 1;
         }
         else
         {
            ptz = &ptzi->standard;
         }
      }
   }
   else
   {
      ptz = &ptzi->standard;
   }

   utc_time -= ptz->bias;

   if(ptm)
   {
      struct tm * ptm_ret = new_gmtime_r(utc_time, ptm);
      ptm->tm_isdst = isDaylightSaving; /* set summer time flag */

#if defined __TM_ZONE || (defined (_POSIX_VERSION) && (_POSIX_VERSION  >= 202405))
      ptm->tm_gmtoff = -ptz->bias;
      ptm->tm_zone   = ptz->zone_name;
#endif

      ptm = ptm_ret;
   }

   return (ptm);
}/* struct tm * localtime_of_zone(time64_t t, struct tm * ptm, const TIME_ZONE_INFO * ptzi) */


/* ------------------------------------------------------------------------- *\
   new_localtime_r an own implementation of localtime_r

   Note: new_localtime_r returns the atronomical date that has a year 0.
         If you need the historical date you can do this as following
   ...
   new_localtime_r(&t, ptm);
   if(ptm->year <= -1900)
      --ptm->year;
   printf ( "The historical year was %s%i%s", ptm->year > -1900 ? "AD" : "",
            ptm->year + 1900, ptm->year < -1900 ? " BC" : "");
   ...

   In Posix 2024 systems ptm->tm_zone points to storage in a static
   TIME_ZONE_INFO struct. It may change once TZ becomes adjusted after the
   call of new_localtime_r.
\* ------------------------------------------------------------------------- */
struct tm * new_localtime_r(time64_t t, struct tm * ptm)
{
   struct tm * ptm_ret;

   if(pta_lock)
      pta_lock(pv_lock_context);

   if(!ti.type)
      update_time_zone_info();

   ptm_ret = localtime_of_zone(t, ptm, &ti);

   if(pta_unlock)
      pta_unlock(pv_lock_context);

   return (ptm_ret);
} /* struct tm * new_localtime_r(time64_t t, struct tm * ptm) */


/* ------------------------------------------------------------------------- *\
   get_local_zone_info stores the local time zone information of the system
   in a user provided struct TIME_ZONE_INFO. It returns nonzero in success
   case.
\* ------------------------------------------------------------------------- */
int get_local_zone_info(TIME_ZONE_INFO * ptzi)
{
   int iret = 0;

   if(!ptzi)
       goto Exit;

   if(pta_lock)
      pta_lock(pv_lock_context);

   if(!ti.type)
      update_time_zone_info();

   *ptzi = ti;

   if(pta_unlock)
      pta_unlock(pv_lock_context);

   iret = 1;
   Exit:;

   return (iret);
} /* int get_local_zone_info(TIME_ZONE_INFO * ptzi) */

/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
