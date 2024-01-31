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
*  COPYRIGHT:    (c) 2024 Dipl.-Ing. Klaus Lux (Aachen, Germany)              *
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
#include <inttypes.h>  /* int64_t */
#include <string.h>
#include <stdlib.h>

#ifdef _WIN32
#pragma warning(disable : 4100 4127)
#else
#include <unistd.h>
#include <sys/types.h>

#ifdef __CYGWIN__
#include <windows.h>
#undef _WIN32
#endif
#endif/* #ifdef _WIN32 */

#include <time_api.h>


/* ========================================================================= *\
   Routines for calculating calendar week of a given date
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   calendar_week_of_year returns the calender week of the year for a struct tm
\* ------------------------------------------------------------------------- */
int calendar_week_of_year(struct tm * ptm)
{
   int wday_01_01;
   int kw_ret = 0;

   if(!ptm)
      goto Exit;

   /* we have to subtract one because the calendar week starts
      on Monday instead on Sunday as in struct ptm specified. */
   wday_01_01 = (700 + ptm->tm_wday - ptm->tm_yday - 1) % 7;

   if(wday_01_01 >= 4)
      wday_01_01 -= 7; /* subtract a week because the 01/01 belongs to KW53 of last year. */

   if(ptm->tm_year < -1900)
      ptm->tm_year -= (((ptm->tm_year + 1900) / 400) - 1) * 400; /* handle the years before year 0  */

   kw_ret = (ptm->tm_yday + wday_01_01 + 7) / 7;

   if (kw_ret == 53)
   { /* KW53 is KW1 of next year if the 12/31 isn't a Thursday */
      long wday_12_31;

      if(!(ptm->tm_year & 3) && ((ptm->tm_year % 100) || !((ptm->tm_year + 1900) % 400)))
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
   time_t timestamp;

   memset(&stm, 0, sizeof(stm)); /* clear data */

   if((month < 1) || (month > 12))
      month = 1;

   if((day < 1) || (day > 31))
      day = 1;

   stm.tm_year = year - 1900;
   stm.tm_mon  = month - 1;
   stm.tm_mday = day;
   stm.tm_hour = 11;

   timestamp = new_mkgmtime(&stm);
   new_gmtime_r(&timestamp, &stm);

   kw_ret = calendar_week_of_year(&stm);

   return (kw_ret);
}/* week_of_year(...) */


/* ------------------------------------------------------------------------- *\
   calendar_week_of_time returns the calender week of the year for a time_t
\* ------------------------------------------------------------------------- */
int calendar_week_of_time(time_t tt)
{
   int kw_ret = 0;
   struct tm stm;

   memset(&stm, 0, sizeof(stm));
   new_gmtime_r(&tt, &stm);
   kw_ret = calendar_week_of_year(&stm);

   return (kw_ret);
} /* int calendar_week_of_time(time_t tt) */


/* ========================================================================= *\
   Wrappers for some Unix functions that may be missing in Windows
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   new_mkgmtime is a mkgmtime implementation
\* ------------------------------------------------------------------------- */
time_t new_mkgmtime(struct tm * ptm)
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

   tt = epoch * ((int64_t) 146097 * 86400); /* time of the 400 year epochs */

   if (year >= 100)
   {
      if (year >= 300)
      {
         tt += (int64_t) 86400 * (36525 + 36524 + 36524);
         year -= 300;
      }
      else if (year >= 200)
      {
         tt += (int64_t) 86400 * (36525 + 36524);
         year -= 200;
      }
      else
      {
         tt += (int64_t) 86400 * 36525;
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

   if(!leap_year)
   {                                        /* month 1   2   3   4   5     6    7    8    9   10   11   12 */
      static const int32_t days_of_month[12]     = {31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31 }; /* number of the days in the month */
      static const int32_t startday_of_month[12] = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }; /* offset of the first day of a month to the begin of the year that is not a leap year */

      if (ptm->tm_mday > days_of_month[ptm->tm_mon])
      {
         errno = ERANGE;
         goto Exit;
      }

      time_of_year = (startday_of_month[ptm->tm_mon] + (ptm->tm_mday - 1)) * 86400;
   }
   else
   {
      static const int32_t days_of_month[12]     = {31, 29, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31 }; /* number of the days in the month */
      static const int32_t startday_of_month[12] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }; /* offset of the first day of a month from the begin of the year in a leap year */

      if (ptm->tm_mday > days_of_month[ptm->tm_mon])
      {
         errno = ERANGE;
         goto Exit;
      }

      time_of_year = (startday_of_month[ptm->tm_mon] + (ptm->tm_mday - 1)) * 86400;
   }

   time_of_year += ptm->tm_sec;
   time_of_year += ptm->tm_min * 60;
   time_of_year += ptm->tm_hour * 3600;

   tt += (int64_t) time_of_year;
   tt -= (int64_t) 719528 * 86400; /* subtract the time from 1/1/0000 to 1/1/1970 */

   if(tt != (time_t) tt)
   { /* handle overflow of 32 bit time_t values */
      tt = -1;
      errno = ERANGE;
      goto Exit;
   }

   Exit:;
   return ((time_t) tt);
} /* time_t new_mkgmtime(struct tm * ptm) */



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
struct tm * new_gmtime_r(time_t * pt, struct tm * ptm)
{
   int64_t time = 0; /* unix time in micro seconds */
   int64_t day;
   int64_t tmp;
   int64_t year;
   int32_t time_of_day;
   int     leap_year = 0;
   int     ignore_leap_year = 0;

   static char mday[366] = /* day of a month of in a leap year */
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

   static char mon[366] = /* month of a day in a leap year */
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

   if(pt)
      time = (int64_t) *pt;

   memset(ptm, 0, sizeof(*ptm));

   /* ptm->tm_isdst = 0; -> ignore summer time flag for UTC */
   time += ((int64_t) 719528 * 86400); /* add the time from 1/1/0000 to 1/1/1970 */

   if (time < 0)
   {
      day = time / 86400; /* number of full days from 1/1/0000 till time */
      ptm->tm_wday = 6 + (int) (day % 7); /* day of the weak the year starts with 0=Sunday ... 6=Saturday. (6 is offset at 1.1.0000) */
      year = day / 146097 - 1;  /* day of the start of the 400 year epoche before the given time */
      time -= year * ((int64_t) 146097 * 86400); /* because year is negative this leads to a posive time */
      year *= 400;
      day  = time / 86400; /* number of days within the 400 year epoch */
      time_of_day = (int32_t) (time - (day * 86400));
   }
   else
   {
      day  = time / 86400;  /* number of full days from 1/1/0000 till time */
      time_of_day = (int32_t) (time - (day * 86400));
      ptm->tm_wday = (int) (day + 6 /* 6 is offset at 1.1.0000 */) % 7; /* day of the weak the year starts with 0=Sunday ... 6=Saturday */
      year = day  / 146097; /* number of full 400 years epochs after 1/1/0000 */
      day -= year * 146097; /* subtract the time of those epochs from the days */
      year *= 400;
   }

   if (day >= 36525)
   { /* if the time is more than 100 years after a full 400 year epoch */
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
         ignore_leap_year = 1; /* we have to ignore the leap year within the first four years of a century */

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
   }
   else if (day >= 731)
   { /* 3rd year of the 4 year block */
      year += 2;
      day -= 731;
   }
   else if (day >= 366)
   { /* 2nd year of the 4 year block */
      year += 1;
      day -= 366;
   }
   else if (!ignore_leap_year)
   {  /* 1rst year of the 4 year block */
      leap_year = 1;
   }
   /* leap_year = !(year & 3) && ((year % 100) || !(year % 400)); */

   ptm->tm_year = (int) (year - 1900);
   ptm->tm_yday = (int) day;

   if(!leap_year && (day >= 59))
      ++day; /* we have to skip the 29. of February in our tables */

   ptm->tm_mon  = mon[(size_t) day];
   ptm->tm_mday = mday[(size_t) day];

   ptm->tm_hour = (time_of_day / 3600);
   ptm->tm_min  = (time_of_day % (3600)) / 60;
   ptm->tm_sec  = (time_of_day % (60));

Exit:;
   return (ptm);
}/* new_gmtime_r */


/* ------------------------------------------------------------------------- *\
   udate_time_zone_info initializes or reinitializes the timezone information
   that is used for new_mktime and new_localtime_r according to the current
   system settings. The function is not yet thread safe because of our usage
   of static timezone information!
\* ------------------------------------------------------------------------- */

typedef struct _TIME_ZONE_RULE TIME_ZONE_RULE;
struct _TIME_ZONE_RULE
{
   int32_t bias;     /* UTC = local time + bias */

   int32_t mode;     /* 0 = at given week of the month  1 = time frome begin of the year (leap day 02/29 ignored)  2 = time from begin of the year (leap day not ignored) */
   int32_t year_day; /* absolute or relative day since begin of the year */
   int32_t month;    /* month of the year  0 = January */
   int32_t mweek;    /* week of the month  1 .. 5  (5 == last week the wday occurs ) */
   int32_t wday;     /* day of the week the rule applies starting with 0 = Sunday */
   int32_t time;     /* local time of the day the the time starts in seconds */

   char    zone_name[72]; /* name of that time zone e.g. "UTC" or "CET" */
};

typedef struct _TIME_ZONE_INFO TIME_ZONE_INFO;
struct _TIME_ZONE_INFO
{
   TIME_ZONE_RULE standard; /* standard time rules */
   TIME_ZONE_RULE daylight; /* daylight saving time rules */
   int32_t        year;     /* for type 3 and 4 only: the year that the final rule starts */
   int32_t        type;     /* 0 = uninitialized  1 = standard time only  2 = day light saving */
};

static TIME_ZONE_INFO ti;   /* static time zone information as returned by the system functions */


/* ------------------------------------------------------------------------- *\
   b_get_TZ_zone_data is a helper of b_read_TZ for reading time zone name
   and time offsets
\* ------------------------------------------------------------------------- */

static int b_read_TZ_zone_data (TIME_ZONE_RULE * ptr, int is_dst, char * psrc, char ** ppend)
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

      ps = ptr->zone_name;
      while(size--)
         *ps++ = *psrc++; /* copy zone name string */
      *ps = '\0';        /* terminate zone name string */
   }

   return (bRet);
}  /* b_read_TZ_zone_data */


/* ------------------------------------------------------------------------- *\
   b_read_TZ_zone_data is a helper of b_read_TZ for reading the
   daylight saving rules
\* ------------------------------------------------------------------------- */

static int b_read_TZ_rules (TIME_ZONE_RULE * ptr, char * psrc, char ** ppend)
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

      if(year_day > 364)
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
      ++ps;

      if ((*ps < '0') || (*ps > '9'))
         goto Exit; /* TZ format error :o( */

      hour = *ps++ - '0';

      if ((*ps >= '0') && (*ps <= '9'))
          hour = (hour * 10) + (*ps++ - '0');

      if(hour > 23)
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

            if(minutes > 59)
               goto Exit;
         }
      }
   }

   ptr->mode     = mode;
   ptr->year_day = year_day;
   ptr->month    = month;
   ptr->mweek    = mweek;
   ptr->wday     = wday;
   ptr->time     = (hour * 3600) + (minutes * 60) + seconds;

   *ppend = ps;

   bRet = 1;
   Exit:;

   return (bRet);
}  /* b_read_TZ_rules */


/* ------------------------------------------------------------------------- *\
   b_read_TZ parses TZ evironment variable for the time zone rules
\* ------------------------------------------------------------------------- */

static int b_read_TZ (TIME_ZONE_INFO *pzi, const char * pTZ)
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

   if(!b_read_TZ_zone_data (&zi.standard, 0 /* STD time */, ps, &ps))
       goto Exit; /* TZ format error :o( */

   zi.type = 1; /* standard time if there is no following data */

   if(b_read_TZ_zone_data (&zi.daylight, 1 /* STD time */, ps, &ps))
   {
       if(zi.daylight.bias == 0x7fffffff)
          zi.daylight.bias = zi.standard.bias - 3600;

       if(!b_read_TZ_rules (&zi.daylight, ps, &ps))
           goto Exit; /* TZ format error :o( */

       if(!b_read_TZ_rules (&zi.standard, ps, &ps))
           goto Exit; /* TZ format error :o( */

       zi.type = 2; /* standard time if there is no following data */
   }

   pzi->type = 0;
   *pzi = zi;
   bRet = 1;

   Exit:;
   return(bRet);
} /* b_read_TZ() */


/* ------------------------------------------------------------------------- *\
   udate_time_zone_info initializes or reinitializes the timezone information
   that is used for new_mktime and new_localtime_r according to the current
   system settings. The function is not thread safe regarding our usage of
   static timezone information and requires TZ being set as specified in the
   Unix standard.
\* ------------------------------------------------------------------------- */

void udate_time_zone_info()
{/* Get time zone information from system */
   static char last_TZ[512];
   char * pTZ = getenv("TZ");
   struct stat st;

   if(!pTZ)
   {
      tzset(); /* mktime should call that. */
      pTZ = getenv("TZ");
   }

   if(pTZ && !strncmp(pTZ, last_TZ, sizeof(last_TZ) - 1))
      goto Exit; /* timezone unchanged */

   strncpy(last_TZ, pTZ, sizeof(last_TZ) - 1);

   if (b_read_TZ(&ti, pTZ))
      goto Exit;

#ifndef _WIN32
   if(!pTZ)
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
               if (b_read_TZ(&ti, pz))
                  goto Exit;
            }
         }
      }
   }

   /* TZ not set or invalid. Try to find system specific infos. */
#if defined(_WIN32) || defined (__CYGWIN__)
   {
      TIME_ZONE_INFORMATION tzi;
      memset (&ti, 0, sizeof(ti));

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

         ti.type = (ti.daylight.bias == ti.standard.bias) ? 1 : 2;
      }
      else
      {
         ti.standard.bias = tzi.Bias * -60;
         ti.daylight.bias = ti.standard.bias;
         ti.type = 1; /* standard time only */
      }
   }

#else

   /* default to UTC without any daylight saving nor time offsets */
   memset(&ti, 0, sizeof(ti));
   strcpy(ti.standard.zone_name, "UTC");
   ti.type = 1;
#endif /* _WIN32 */

   Exit:;
} /* void udate_time_zone_info() */


/* ------------------------------------------------------------------------- *\
   Helper variables for faster calculations
\* ------------------------------------------------------------------------- */

static const uint8_t mod_7[] = {0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6, 0,1,2,3,4,5,6,
                                0,1,2,3,4,5 };
                                                      /* month 1   2   3   4   5     6    7    8    9   10   11   12 */
static const int32_t days_of_month_array[12]               = {31, 28, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31 }; /* number of the days in the month */
static const int32_t startday_of_month_array[12]           = { 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334 }; /* offset of the first day of a month to the begin of the year that is not a leap year */
static const int32_t days_of_month_array_leap_year[12]     = {31, 29, 31, 30,  31,  30,  31,  31,  30,  31,  30,  31 }; /* number of the days in the month */
static const int32_t startday_of_month_array_leap_year[12] = { 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335 }; /* offset of the first day of a month from the begin of the year in a leap year */


/* ------------------------------------------------------------------------- *\
   new_mktime is a mktime implementation for Windows
\* ------------------------------------------------------------------------- */
time_t new_mktime(struct tm * ptm)
{
   int64_t tt = -1;
   int     leap_year = 0;
   int     isDaylightSaving;
   int64_t year;
   int64_t epoch;
   int32_t time_of_year;
   const int32_t * days_of_month;
   const int32_t * startday_of_month;

   if(!ptm)
   {
      errno = EINVAL;
      goto Exit;
   }

   udate_time_zone_info();

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

   tt = epoch * ((int64_t) 146097 * 86400); /* time of the 400 year epochs */

   if (year >= 100)
   {
      if (year >= 300)
      {
         tt += (int64_t) 86400 * (36525 + 36524 + 36524);
         year -= 300;
      }
      else if (year >= 200)
      {
         tt += (int64_t) 86400 * (36525 + 36524);
         year -= 200;
      }
      else
      {
         tt += (int64_t) 86400 * 36525;
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

   if(!leap_year)
   {
      days_of_month     = days_of_month_array;
      startday_of_month = startday_of_month_array;
   }
   else
   {
      days_of_month     = days_of_month_array_leap_year;
      startday_of_month = startday_of_month_array_leap_year;
   }

   if (ptm->tm_mday > days_of_month[ptm->tm_mon])
   {
      errno = ERANGE;
      goto Exit;
   }

   time_of_year = (startday_of_month[ptm->tm_mon] + (ptm->tm_mday - 1)) * 86400;

   time_of_year += ptm->tm_sec;
   time_of_year += ptm->tm_min * 60;
   time_of_year += ptm->tm_hour * 3600;

   isDaylightSaving = ptm->tm_isdst;

   if((isDaylightSaving < 0) && (ti.type > 1))
   {
      TIME_ZONE_RULE * ptz;
      int32_t wday_year_start;
      int32_t daylight_start; /* begin of day light saving in seconds after begin of the year */
      int32_t standard_start; /* begin of standard time in seconds after begin of the year */
      int32_t month; /* index of the month */
      int32_t month_start_day;
      int32_t wday_month_start;
      int32_t week_of_month;
      int32_t switchday;

      /* The day of week calculation works well for years before 0 as well because every 400 year epoch starts with the same day of week */
      wday_year_start = (int32_t)(((tt / 86400) + 6 /* 6 is offset at 1/1/0000 */) % 7); /* day of the weak the year starts with 0=Sunday 1= Monday ... */

      /* ------------------------------------------------------------------------- */

      /* calculate the begin of the daylight saving after the start of the year in seconds */

      ptz              = &ti.daylight;
      month            = ptz->month; /* index of the month */
      month_start_day  = startday_of_month[month];
      wday_month_start = mod_7[wday_year_start + month_start_day]; /* (wday_year_start + month_start_day) % 7; */
      week_of_month    = ptz->mweek;

      if(wday_month_start > (int32_t) ptz->wday)
         switchday = ptz->wday + 7 - wday_month_start; /* set switchday to the index of the first matching day of week within the month */
      else
         switchday = ptz->wday - wday_month_start; /* set switchday to the index of the first matching day of week within the month */

      month = days_of_month[month] - 7; /* days if switchday increased by 7 needs to be inside of that month */

      while((--week_of_month) && (switchday < month))
         switchday += 7;

      daylight_start = month_start_day + switchday; /* start day of daylight saving within the year */
      daylight_start = (daylight_start * 86400) + ptz->time + ti.standard.bias; /* time offset of begin of the daylight saving within the year in seconds */

      /* ------------------------------------------------------------------------- */

      /* calculate the return to the standard time after the start of the year in seconds */
      ptz              = &ti.standard;
      month            = ptz->month; /* index of the month */
      month_start_day  = startday_of_month[month];
      wday_month_start = mod_7[wday_year_start + month_start_day]; /* (wday_year_start + month_start_day) % 7; */
      week_of_month    = ptz->mweek;

      if(wday_month_start > (int32_t) ptz->wday)
         switchday = ptz->wday + 7 - wday_month_start; /* set switchday to the index of the first matching day of week within the month */
      else
         switchday = ptz->wday - wday_month_start; /* set switchday to the index of the first matching day of week within the month */

      month = days_of_month[month] - 7; /* days if switchday increased by 7 needs to be inside of that month */

      while((--week_of_month) && (switchday < month))
         switchday += 7;

      standard_start = month_start_day + switchday; /* start day of standard time within the year */
      standard_start = (standard_start * 86400) + ptz->time + ti.daylight.bias;  /* time offset of returning to the standard time in the year in seconds */

      /* ------------------------------------------------------------------------- */

      if (daylight_start > standard_start)
      {  /* southern hemisphere */
         if((time_of_year >= standard_start) && (time_of_year < daylight_start))
            tt += ti.standard.bias;
         else
            tt += ti.daylight.bias;
      }
      else
      {  /* northern hemisphere */
         if((time_of_year >= daylight_start) && (time_of_year < standard_start))
            tt += ti.daylight.bias;
         else
            tt += ti.standard.bias;
      }
   }
   else
   {
      if(isDaylightSaving && (ti.type > 1))
         tt += ti.daylight.bias;
      else
         tt += ti.standard.bias;
   }

   tt += (int64_t) time_of_year;
   tt -= (int64_t) 719528 * 86400; /* subtract the time from 1/1/0000 to 1/1/1970 */

   if(tt != (time_t) tt)
   { /* handle overflow of 32 bit time_t values */
      tt = -1;
      errno = ERANGE;
      goto Exit;
   }

   Exit:;
   return ((time_t) tt);
} /* time_t new_mktime(struct tm * ptm) */


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
\* ------------------------------------------------------------------------- */
struct tm * new_localtime_r(time_t * pt, struct tm * ptm)
{
   time_t  utc_time = 0;
   int64_t time;
   int     isDaylightSaving = 0;

   if(!ti.type)
      udate_time_zone_info();

   if(pt)
      utc_time = *pt;

   if (ti.type > 1)
   {
      int32_t daylight_start; /* begin of day light saving in seconds after begin of the year */
      int32_t standard_start; /* begin of standard time in seconds after begin of the year */

      TIME_ZONE_RULE * ptz;
      int32_t month; /* index of the month */
      int32_t month_start_day;
      int32_t wday_month_start;
      int32_t week_of_month;
      int32_t switchday;
      int32_t time_of_day;
      const int32_t * days_of_month;
      const int32_t * startday_of_month;

      int32_t time_of_year;
      int32_t wday_year_start;

      int     leap_year = 0;
      int     ignore_leap_year = 0;

      int64_t tmp;
      int64_t day;
      int64_t year;

      time = utc_time + ((int64_t) 719528 * 86400); /* add the time from 1/1/0000 to 1/1/1970 */
      if (time < 0)
      {
         year = (time / 86400 ) / 146097 - 1;       /* day of the start of the 400 year epoche before that time */
         time -= year * ((int64_t) 146097 * 86400); /* because year is negative this leads to a posive time */
         year *= 400;
         day  = time / 86400; /* number of days within the 400 year epoch */
         time_of_day = (int32_t) (time - (day * 86400));
      }
      else
      {
         day  = time / 86400;  /* number of full days from 1/1/0000 till time */
         time_of_day = (int32_t) (time - (day * 86400));
         year = day  / 146097; /* number of full 400 years epochs after 1/1/0000 */
         day -= year * 146097; /* subtract the time of those epochs from the days */
         year *= 400;
      }

      if (day >= 36526)
      {  /* the time is more than 100 years after a full 400 year epoch */
         day -= 36526;
         year += 100;

         if (day >= 36525)
         {
            day -= 36525;
            year += 100;

            if (day >= 36525)
            {
               day -= 36525;
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
            ignore_leap_year = 1; /* we have to ignore the leap year within the first four years of a century */

            while (day >= 365)
            {
               ++year;
               day -= 365;
            }
         }
      }

      /* now we handle the rest of the years */
      tmp = (day / 1461);  /* calculate the number of full 4 year epochs that start with a leap year */
      year += tmp * 4;
      day  -= tmp * 1461;

      if (day >= 1096)
      { /* last year of the remaining 4 year block */
         year += 3;
         day -= 1096;
      }
      else if (day >= 731)
      { /* 3rd year of the 4 year block */
         year += 2;
         day -= 731;
      }
      else if (day >= 366)
      { /* 2n year of the 4 year block */
         year += 1;
         day -= 366;
      }
      else if (!ignore_leap_year)
      {  /* 1rst year of the 4 year block */
         leap_year = 1;
      }

      /* leap_year = !(year & 3) && ((year % 100) || !(year % 400)); */

      time_of_year = ((int32_t) day * 86400) /* time between the begin of the day and the begin of the year */
                     + time_of_day;          /* time since begin of the day */

      /* The day of week calculation works well for years before 0 as well because every 400 year epoch starts with the same day of week */
      wday_year_start = (int32_t)((((time - time_of_year) / 86400) + 6 /* 6 is offset at 1/1/0000 */) % 7); /* day of the weak the year starts with 0=Sunday 1= Monday ... */

      if(!leap_year)
      {
         days_of_month     = days_of_month_array;
         startday_of_month = startday_of_month_array;
      }
      else
      {
         days_of_month     = days_of_month_array_leap_year;
         startday_of_month = startday_of_month_array_leap_year;
      }

      /* ------------------------------------------------------------------------- */

      /* calculate the begin of the daylight saving after the start of the year in seconds */
      ptz              = &ti.daylight;
      month            = ptz->month; /* index of the month */
      month_start_day  = startday_of_month[month];
      wday_month_start = mod_7[wday_year_start + month_start_day]; /* (wday_year_start + month_start_day) % 7; */
      week_of_month    = ptz->mweek;

      if(wday_month_start > (int32_t) ptz->wday)
         switchday = ptz->wday + 7 - wday_month_start; /* set switchday to the index of the first matching day of week within the month */
      else
         switchday = ptz->wday - wday_month_start; /* set switchday to the index of the first matching day of week within the month */

      month = days_of_month[month] - 7; /* days if switchday increased by 7 needs to be inside of that month */

      while((--week_of_month) && (switchday < month))
         switchday += 7;

      daylight_start = month_start_day + switchday; /* start day of daylight saving within the year */
      daylight_start = (daylight_start * 86400) + ptz->time + ti.standard.bias; /* time offset of begin of the daylight saving within the year in seconds */

      /* ------------------------------------------------------------------------- */

      /* calculate the return to the standard time after the start of the year in seconds */
      ptz              = &ti.standard;
      month            = ptz->month; /* index of the month */
      month_start_day  = startday_of_month[month];
      wday_month_start = mod_7[wday_year_start + month_start_day]; /* (wday_year_start + month_start_day) % 7; */
      week_of_month    = ptz->mweek;

      if(wday_month_start > (int32_t) ptz->wday)
         switchday = ptz->wday + 7 - wday_month_start; /* set switchday to the index of the first matching day of week within the month */
      else
         switchday = ptz->wday - wday_month_start; /* set switchday to the index of the first matching day of week within the month */

      month = days_of_month[month] - 7; /* days if switchday increased by 7 needs to be inside of that month */

      while((--week_of_month) && (switchday < month))
         switchday += 7;

      standard_start = month_start_day + switchday; /* start day of standard time within the year */
      standard_start = (standard_start * 86400) + ptz->time + ti.daylight.bias;  /* time offset of returning to the standard time in the year in seconds */

      /* ------------------------------------------------------------------------- */

      if (daylight_start > standard_start)
      {  /* southern hemisphere */
         if((time_of_year >= standard_start) && (time_of_year < daylight_start))
         {
            utc_time -= ti.standard.bias;
         }
         else
         {
            utc_time -= ti.daylight.bias;
            isDaylightSaving = 1;
         }
      }
      else
      {  /* northern hemisphere */
         if((time_of_year >= daylight_start) && (time_of_year < standard_start))
         {
            utc_time -= ti.daylight.bias;
            isDaylightSaving = 1;
         }
         else
         {
            utc_time -= ti.standard.bias;
         }
      }
   }
   else
   {
      utc_time -= ti.standard.bias;
   }

   if(ptm)
   {
      new_gmtime_r(&utc_time, ptm);
      ptm->tm_isdst = isDaylightSaving; /* set summer time flag */
   }

   return (ptm);
}/* new_localtime_r */


/* ========================================================================= *\
   END OF FILE
\* ========================================================================= */
