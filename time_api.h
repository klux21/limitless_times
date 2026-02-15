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
*  COPYRIGHT:    (c) 2026 Dipl.-Ing. Klaus Lux (Aachen, Germany)              *
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

#include <stdint.h> /* int64_t etc. */
#include <time.h>   /* struct tm and localtime_r */

#ifdef __cplusplus
extern "C" {
#endif

#ifndef time64_t
/* define a 64bit time_t value */
typedef int64_t  time64_t;
#define time64_t time64_t
#endif

/* ------------------------------------------------------------------------- *\
   unix_time returns the Unix time in microseconds.
   (UTC time since 01/01/1970) The precision depends on the system.
\* ------------------------------------------------------------------------- */
int64_t unix_time();

/* ------------------------------------------------------------------------- *\
   unix_time_ns returns the Unix time in nanoseconds.
   (UTC time since 01/01/1970) The precision depends on the system.
\* ------------------------------------------------------------------------- */
int64_t unix_time_ns();

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
int calendar_week_of_time(time64_t tt);

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
struct tm * new_gmtime_r(time64_t t, struct tm * ptm);

#ifdef gmtime_r
#undef gmtime_r
#endif

#define gmtime_r(t, ptm)   new_gmtime_r(*(t), ptm)

/* ------------------------------------------------------------------------- *\
   new_mkgmtime is a timegm (mkgmtime) implementation that does not adjust
   any members of the input struct as timegm (mkgmtime) does.
\* ------------------------------------------------------------------------- */
time64_t new_timegm(const struct tm * ptm);

/* ------------------------------------------------------------------------- *\
   std_timegm is a timegm (mkgmtime) implementation that adjusts the members
   of the input struct as the C standard requires.
\* ------------------------------------------------------------------------- */
time64_t std_timegm(struct tm * ptm);

#ifdef mkgmtime
#undef mkgmtime
#endif

#define mkgmtime   std_timegm

#ifdef timegm
#undef timegm
#endif

#define timegm   std_timegm


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
   new_mktime is a mktime implementation that does not adjust any members of
   the input struct as mktime does.
\* ------------------------------------------------------------------------- */
time64_t new_mktime(const struct tm * ptm);

/* ------------------------------------------------------------------------- *\
   std_mktime is a mktime implementation that adjusts the members of the
   input struct as the C standard requires.
\* ------------------------------------------------------------------------- */
time64_t std_mktime(struct tm * ptm);

#ifdef mktime
#undef mktime
#endif

#define mktime    std_mktime

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

   In Posix 2024 systems ptm->tm_zone points to storage in a static
   TIME_ZONE_INFO struct. It may change once TZ becomes adjusted after the
   call of new_localtime_r.
\* ------------------------------------------------------------------------- */
struct tm * new_localtime_r(time64_t t, struct tm * ptm);

#ifdef localtime_r
#undef localtime_r
#endif

#define localtime_r(t, ptm)   new_localtime_r(*(t), ptm)


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
   int32_t bias;          /* UTC = local time + this bias */

   int32_t mode;          /* 0 = at given week of the month  1 = time frome begin of the year (leap day 02/29 ignored)  2 = time from begin of the year (leap day not ignored) */
   int32_t year_day;      /* absolute or relative day since begin of the year */
   int32_t month;         /* month of the year  0 = January */
   int32_t mweek;         /* week of the month  1 .. 5  (5 == last week the wday occurs ) */
   int32_t wday;          /* day of the week the rule applies starting with 0 = Sunday */
   int32_t time;          /* local time of the day that this time rule applies in seconds */

   int32_t start[14];     /* by init_tz_rule_offsets() precalculated time of a year that this rule applies depending on the weekday that the year begins with */
   char    zone_name[72]; /* name of the time zone as usually specified in TZ environment variable */
};

typedef struct TIME_ZONE_INFO_S TIME_ZONE_INFO;
struct TIME_ZONE_INFO_S
{
   TIME_ZONE_RULE standard; /* standard time rules */
   TIME_ZONE_RULE daylight; /* daylight saving time rules */
   int32_t        type;     /* 0 = uninitialized  1 = standard time only  2 = day light saving */
};

/* ------------------------------------------------------------------------- *\
   read_TZ parses a Unix conform TZ evironment variable conform string for
   the time zone rules and stores this rules in success case in a struct
   TIME_ZONE_INFO. The function returns nonzero in success case only.
   If the function fails because of an invalid string then the storage that
   ptzi points to is unchanged.
\* ------------------------------------------------------------------------- */
int read_TZ (TIME_ZONE_INFO * pzi, const char * pTZ);


/* ------------------------------------------------------------------------- *\
   mktime_of_zone is a thread safe mktime implementation for any timezone
   where the daylight saving rules are given in a struct TIME_ZONE_INFO
\* ------------------------------------------------------------------------- */
time64_t mktime_of_zone(const struct tm * ptm, const TIME_ZONE_INFO * ptzi);


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
struct tm * localtime_of_zone(time64_t t, struct tm * ptm, const TIME_ZONE_INFO * ptzi);


/* ------------------------------------------------------------------------- *\
   get_local_zone_info stores the local time zone information of the system
   in a user provided struct TIME_ZONE_INFO.
   It returns nonzero in success case.
\* ------------------------------------------------------------------------- */
int get_local_zone_info(TIME_ZONE_INFO * ptzi);

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
   Be aware that standard C time functions as provided by a compiler aren't
   required to be thread safe implemented during changes of the global time
   settings e.g. if there are changes of the TZ environment variable of your
   process!
\* ------------------------------------------------------------------------- */
typedef void (* TIME_API_LOCK) (void * pv_context); /* the mutex lock or unlock callback function prototype */

void init_time_api_lock(TIME_API_LOCK pfn_lock,     /* pointer to a user provided mutex lock callback function */
                        TIME_API_LOCK pfn_unlock,   /* pointer to a user provided mutex unlock callback function */
                        void *        pv_context);  /* user provided context, e.g. pointer to the mutex. */

#ifdef __cplusplus
}/* extern "C" */
#endif

#endif /* TIME_API_H */

/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
