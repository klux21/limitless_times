/*****************************************************************************\
*                                                                             *
*  FILENAME:     time_api.h                                                   *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:  Header for times.c that contains time handling functions     *
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

#ifndef TIME_API_H
#define TIME_API_H

#include <stddef.h>
#include <time.h>      /* struct tm and localtime_r */
#include <sys/types.h>

#ifdef _WIN32
#include <winsock2.h>  /* required for timeval struct */
#include <WS2tcpip.h>  /* for IPv6 related stuff */
#include <windows.h>
#include <winbase.h>

#if !defined(_TIME_T_DEFINED) && !defined(time_t)
typedef int64_t time_t;  /* time value */
#define time_t  time_t   /* time value */
#define _TIME_T_DEFINED  /* avoid multiple def's of time_t */
#endif

#pragma warning(disable : 4204)
#endif /* _WIN32 */

#ifdef __CYGWIN__
#include <windows.h>  /* required for struct timeval and LPFILETIME */
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* ------------------------------------------------------------------------- *\
   week_of_year returns the calendar week of a given date
\* ------------------------------------------------------------------------- */
int week_of_year(int year, int month, int day);

/* ------------------------------------------------------------------------- *\
   calendar_week_of_year returns the calender week of the year for a struct tm
\* ------------------------------------------------------------------------- */
int calendar_week_of_year(const struct tm * ptm);

/* ------------------------------------------------------------------------- *\
   calendar_week_of_time returns the calender week of the year for a time_t
\* ------------------------------------------------------------------------- */
int calendar_week_of_time(time_t tt);


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
struct tm * new_gmtime_r(const time_t * pt, struct tm * ptm);

#ifdef gmtime_r
#undef gmtime_r
#endif

#define gmtime_r   new_gmtime_r

/* ------------------------------------------------------------------------- *\
   new_mkgmtime is a mkgmtime (timegm) implementation
\* ------------------------------------------------------------------------- */
time_t new_mkgmtime(const struct tm * ptm);

#ifdef mkgmtime
#undef mkgmtime
#endif

#define mkgmtime   new_mkgmtime

#ifdef timegm
#undef timegm
#endif

#define timegm   new_mkgmtime


/* ------------------------------------------------------------------------- *\
   update_time_zone_info initializes or reinitializes the timezone information
   that is used for new_mktime and new_localtime_r according to the current
   system settings. The function is not yet thread safe implemented
   and requires TZ being defined as specified in the Unix standard.
   The function is called by new_mktime() and calls tzset() internally
   if the TZ variable isn't yet set already.
\* ------------------------------------------------------------------------- */
void update_time_zone_info();

/* ------------------------------------------------------------------------- *\
   new_mktime is a mktime implementation
\* ------------------------------------------------------------------------- */
time_t new_mktime(const struct tm * ptm);

#ifdef mktime
#undef mktime
#endif

#define mktime    new_mktime

/* ------------------------------------------------------------------------- *\
   new_localtime_r multithreading safe version of localtime

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
struct tm * new_localtime_r(const time_t * pt, struct tm * ptm);

#ifdef localtime_r
#undef localtime_r
#endif

#define localtime_r   new_localtime_r


/* ========================================================================= *\
   Time zone related stuff that simplifies the comparison and conversion
   of times of different timezones
\* ========================================================================= */

/* ------------------------------------------------------------------------- *\
   udate_time_zone_info initializes or reinitializes the timezone information
   that is used for new_mktime and new_localtime_r according to the current
   system settings. The function is not yet thread safe because of our usage
   of static timezone information!
\* ------------------------------------------------------------------------- */

typedef struct TIME_ZONE_RULE_S TIME_ZONE_RULE;
struct TIME_ZONE_RULE_S
{
   int32_t bias;     /* UTC = local time + bias */

   int32_t mode;     /* 0 = at given week of the month  1 = time frome begin of the year (leap day 02/29 ignored)  2 = time from begin of the year (leap day not ignored) */
   int32_t year_day; /* absolute or relative day since begin of the year */
   int32_t month;    /* month of the year  0 = January */
   int32_t mweek;    /* week of the month  1 .. 5  (5 == last week the wday occurs ) */
   int32_t wday;     /* day of the week the rule applies starting with 0 = Sunday */
   int32_t time;     /* local time of the day the the time starts in seconds */

   char    zone_name[72]; /* name of that time zone */
};

typedef struct TIME_ZONE_INFO_S TIME_ZONE_INFO;
struct TIME_ZONE_INFO_S
{
   TIME_ZONE_RULE standard; /* standard time rules */
   TIME_ZONE_RULE daylight; /* daylight saving time rules */
   int32_t        year;     /* for type 3 and 4 only: the year that the final rule starts */
   int32_t        type;     /* 0 = uninitialized  1 = standard time only  2 = day light saving */
};


/* ------------------------------------------------------------------------- *\
   b_read_TZ parses a Unix conform TZ evironment variable conform string for
   the time zone rules and stores this rules in success case in a struct
   TIME_ZONE_INFO. The function returns nonzero in success case only.
   If the function fails because of an invalid string then the strage that
   ptzi points to is unchanged.
\* ------------------------------------------------------------------------- */
int b_read_TZ (TIME_ZONE_INFO * pzi, const char * pTZ);


/* ------------------------------------------------------------------------- *\
   mktime_of_zone is a thread safe mktime implementation for any timezone
   where the daylight saving rules are given in a struct TIME_ZONE_INFO
\* ------------------------------------------------------------------------- */
time_t mktime_of_zone(const struct tm * ptm, const TIME_ZONE_INFO * ptzi);


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
\* ------------------------------------------------------------------------- */
struct tm * localtime_of_zone(time_t t, struct tm * ptm, const TIME_ZONE_INFO * ptzi);

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* TIME_API_H */

/* ========================================================================= *\
   END OF FILE
\* ========================================================================= */
