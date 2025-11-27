#if 0
rm -f ./_test_times ; cc -Wall -O3 -o _test_times -I . -I zones test_times.c time_api.c zones/tz_value.c ; ./_test_times ; exit $?
#endif

/*****************************************************************************\
*                                                                             *
*  FILENAME:     time_test.c                                                  *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
*  DESCRIPTION:  test of time functions                                       *
*                                                                             *
* --------------------------------------------------------------------------- *
*                                                                             *
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

#include <stdlib.h> /* setenv */
#include <stdio.h>
#include <string.h> /* memset */
#include <errno.h>
#include <inttypes.h>

#ifndef _WIN32
#include <sys/time.h>
#endif

#include <time_api.h>
#include <tz_value.h>


/* ------------------------------------------------------------------------- *\
   test_time_range tests the range of new_mktime, mew_mkgmtime,
   new_localtime_r and new_gmtime_r
\* ------------------------------------------------------------------------- */

int test_time_range()
{
    int bRet = 0;
    int year = -20000;

    while (year <= 20000)
    {
       int d = 0;
       struct tm ti;
       struct tm stm;
       time64_t t;

       memset(&stm, 0, sizeof(stm));
       memset(&ti, 0, sizeof(ti));
       ti.tm_year  = year - 1900;
       ti.tm_mon   = 11; // December
       ti.tm_mday  = 31;
       ti.tm_hour  = 22;
       ti.tm_min   = 59;
       ti.tm_sec   = 59;
       ti.tm_isdst = -1;

       t = new_timegm(&ti);

       if((t - 3600) != mktime(&ti))
       {
          fprintf(stderr, "mktime error %d %lld != %lld !\n", year, (long long) (t - 3600), (long long) mktime(&ti));
          goto Exit;
       }

       d = (!(year % 4) && ((year % 100) || !(year % 400))) ? 365 : 364;

       gmtime_r (&t, &stm);
       if(   (stm.tm_year != year - 1900)
          || (stm.tm_mon  != 11) /* December */
          || (stm.tm_mday != 31)
          || (stm.tm_hour != 22) /* assumes UTC */
          || (stm.tm_min  != 59)
          || (stm.tm_sec  != 59)
          || (stm.tm_yday != d))
       {
          fprintf(stderr, "gmtime_r error!\n");
          goto Exit;
       }

       /* fprintf(stdout, "GMT %.2d.%.2d.%.4d %.2d:%.2d:%.2d!\n",  stm.tm_mday, stm.tm_mon+1, stm.tm_year+1900, stm.tm_hour, stm.tm_min, stm.tm_sec); */

       localtime_r(&t, &stm);

       if(   (stm.tm_year != year - 1900)
          || (stm.tm_mon  != 11) /* December */
          || (stm.tm_mday != 31)
          || (stm.tm_hour != 23) /* assumes CET */
          || (stm.tm_min  != 59)
          || (stm.tm_sec  != 59)
          || (stm.tm_yday != d))
       {
          fprintf(stderr, "localtime_r error!\n");
          goto Exit;
       }

       /* fprintf(stdout, "CET %.2d.%.2d.%.4d %.2d:%.2d:%.2d!\n",  stm.tm_mday, stm.tm_mon+1, stm.tm_year+1900, stm.tm_hour, stm.tm_min, stm.tm_sec); */

       t += 3600 + 1;
       d=0;

       gmtime_r (&t, &stm);
       if(   (stm.tm_year != year + 1 - 1900)
          || (stm.tm_mon  != 0) /* January */
          || (stm.tm_mday != 1)
          || (stm.tm_hour != 0) /* assumes UTC */
          || (stm.tm_min  != 0)
          || (stm.tm_sec  != 0)
          || (stm.tm_yday != 0))
       {
          fprintf(stderr,  "gmtime_r error!\n");
          goto Exit;
       }

       /* fprintf(stdout, "GMT %.2d.%.2d.%.4d %.2d:%.2d:%.2d!\n",  stm.tm_mday, stm.tm_mon+1, stm.tm_year+1900, stm.tm_hour, stm.tm_min, stm.tm_sec); */

       t -= 3600;
       localtime_r(&t, &stm);

       if(   (stm.tm_year != year + 1 - 1900)
          || (stm.tm_mon  != 0)  /* January */
          || (stm.tm_mday != 1)
          || (stm.tm_hour != 0)  /* because CET */
          || (stm.tm_min  != 0)
          || (stm.tm_sec  != 0)
          || (stm.tm_yday != 0))
       {
          fprintf(stderr, "localtime_r error!\n");
          goto Exit;
       }

       /* fprintf(stdout, "CET %.2d.%.2d.%.4d %.2d:%.2d:%.2d!\n",  stm.tm_mday, stm.tm_mon+1, stm.tm_year+1900, stm.tm_hour, stm.tm_min, stm.tm_sec); */

       ++year;
   }

   bRet = 1;
   Exit:;

   if(!bRet)
       fprintf(stderr, "Time conversion tests have failed!\n\n");
   else
       fprintf(stdout, "Time conversion tests of the years 20001 BC til 20000 AD  passed!\n\n");
   return(bRet);
} /* int test_time_range() */


/* ------------------------------------------------------------------------- *\
   test_speed measures the speed of new_mktime, mew_mkgmtime, new_localtime_r
   and new_gmtime_r and the system implementations.
\* ------------------------------------------------------------------------- */

#undef timegm
#undef mktime
#undef mkgmtime
#undef gmtime_r
#undef localtime_r


#ifdef _WIN32
#define mkgmtime _mkgmtime

/* gmtime and localtime are threadsafe implemented in Windows */
struct tm * gmtime_r(const time_t * pt, struct tm * ptm)
{
   struct tm * pgt = gmtime(pt);
   if (pgt)
       *ptm = *pgt;
   return (ptm);
} /* struct tm * gmtime_r(const time_t * pt, struct tm * ptm */

struct tm * localtime_r(const time_t * pt, struct tm * ptm)
{
   struct tm * pgt = localtime(pt);
   if (pgt)
       *ptm = *pgt;
   return (ptm);
} /* struct tm * localtime_r(const time_t * pt, struct tm * ptm */
#endif


int test_speed()
{
   int bRet = 0;
   struct tm stm;
   struct tm otm;
   time_t ot = unix_time();
   time_t tt = unix_time();
   time_t t0;
   time_t t1;
   int64_t i;

   char * pz = getenv("TZ");
   fprintf(stdout, "TZ=%s\n", pz ? pz : "<empty>");

   memset(&stm, 0, sizeof(stm));

   stm.tm_year  = 2024 - 1900;
   stm.tm_mon   =  0;  /* January */
   stm.tm_mday  = 31;
   stm.tm_hour  = 22;
   stm.tm_min   = 59;
   stm.tm_sec   = 59;
   stm.tm_isdst = -1;

#ifdef _WIN32
   i  = 1000000;
   t0 = unix_time();
   while (i--)
      ot = mkgmtime(&stm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average __ common mkgmtime() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));
#else
   i  = 1000000;
   t0 = unix_time();
   while (i--)
      ot = timegm(&stm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average ____ common timegm() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));
#endif

   i = 1000000;
   t0 = unix_time();
   while (i--)
      tt = std_timegm(&stm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average _______ std_timegm() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   if (ot != tt)
   {
      fprintf (stderr, "Return values of timegm() and new_timegm() differ! (%ld != %ld)", (long) ot, (long) tt);
      goto Exit;
   }

   i  = 1000000;
   t0 = unix_time();
   while (i--)
      tt = new_timegm(&stm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average _______ new_timegm() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   if (ot != tt)
   {
      fprintf (stderr, "Return values of timegm() and std_timegm() differ! (%ld != %ld)", (long) ot, (long) tt);
      goto Exit;
   }

   i  = 1000000;
   t0 = unix_time();
   while (i--)
   {
      stm.tm_isdst = -1;
      ot = mktime(&stm);
   }
   t1 = unix_time() - t0;
   fprintf(stdout, "An average ____ common mktime() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = unix_time();
   while (i--)
   {
      stm.tm_isdst = -1;
      tt = std_mktime(&stm);
   }
   t1 = unix_time() - t0;
   fprintf(stdout, "An average _______ std_mktime() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   if (ot != tt)
   {
      fprintf (stderr, "Return values of mktime() and std_mktime() differ! (%ld != %ld)", (long) ot, (long) tt);
      goto Exit;
   }

   i  = 1000000;
   t0 = unix_time();
   while (i--)
   {
      stm.tm_isdst = -1;
      tt = new_mktime(&stm);
   }
   t1 = unix_time() - t0;
   fprintf(stdout, "An average _______ new_mktime() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   if (ot != tt)
   {
      fprintf (stderr, "Return values of mktime() and new_mktime() differ! (%ld != %ld)", (long) ot, (long) tt);
      goto Exit;
   }

   i  = 1000000;
   t0 = unix_time();
   while (i--)
      gmtime_r(&tt, &otm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average __ common gmtime_r() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = unix_time();
   while (i--)
      new_gmtime_r(tt, &stm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average _____ new_gmtime_r() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

#if defined __TM_ZONE || (defined (_POSIX_VERSION) && (_POSIX_VERSION  >= 202405))
   if ((otm.tm_gmtoff != stm.tm_gmtoff) ||
        (strcmp(otm.tm_zone, stm.tm_zone) && strcmp(otm.tm_zone, "GMT")))
   {
      fprintf (stderr, "Return values of gmtime_r() and new_gmtime_r() differ! \n"
                       "gmtoff=%ld zone='%s' != gmtoff=%ld zone='%s'\n",
                        (long) otm.tm_gmtoff, otm.tm_zone, (long) stm.tm_gmtoff, stm.tm_zone);
   }
#endif

   if(   (otm.tm_year  != stm.tm_year)
      || (otm.tm_mon   != stm.tm_mon)
      || (otm.tm_mday  != stm.tm_mday)
      || (otm.tm_hour  != stm.tm_hour)
      || (otm.tm_min   != stm.tm_min)
      || (otm.tm_sec   != stm.tm_sec)
      || (otm.tm_wday  != stm.tm_wday)
      || (otm.tm_isdst != stm.tm_isdst)
      || (otm.tm_yday  != stm.tm_yday))
   {
      fprintf (stderr, "Return values of gmtime_r() and new_gmtime_r() differ! \n"
                       "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) !=  %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                        otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                        stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday);
      goto Exit;
   }

   i  = 1000000;
   t0 = unix_time();
   while (i--)
      localtime_r(&tt, &otm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average common localtime_r() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

   i  = 1000000;
   t0 = unix_time();
   while (i--)
      new_localtime_r(tt, &stm);
   t1 = unix_time() - t0;
   fprintf(stdout, "An average __ new_localtime_r() call took %ld.%.6ld us\n", (long)(t1 / 1000000), (long)(t1 % 1000000));

#if defined __TM_ZONE || (defined (_POSIX_VERSION) && (_POSIX_VERSION  >= 202405))
   if ((otm.tm_gmtoff != stm.tm_gmtoff) || strcmp(otm.tm_zone, stm.tm_zone))
   {
      fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ! \n"
                       "gmtoff=%ld zone='%s' != gmtoff=%ld zone='%s'\n",
                        (long) otm.tm_gmtoff, otm.tm_zone, (long) stm.tm_gmtoff, stm.tm_zone);
   }
#endif

   if(   (otm.tm_year  != stm.tm_year)
      || (otm.tm_mon   != stm.tm_mon)
      || (otm.tm_mday  != stm.tm_mday)
      || (otm.tm_hour  != stm.tm_hour)
      || (otm.tm_min   != stm.tm_min)
      || (otm.tm_sec   != stm.tm_sec)
      || (otm.tm_wday  != stm.tm_wday)
      || (otm.tm_isdst != stm.tm_isdst)
      || (otm.tm_yday  != stm.tm_yday))
   {
      fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ! \n"
                       "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                        otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                        stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday );
      goto Exit;
   }

   bRet = 1;
   Exit:;

   pz = getenv("TZ");

   if(!bRet)
       fprintf(stderr, "\nPerformance tests have failed! (TZ=%s) \n", pz ? pz : "<empty>");
   else
       fprintf(stdout, "\nPerformance tests passed! (TZ=%s)\n", pz ? pz : "<empty>");

   return(bRet);
} /* int test_speed() */


/* ------------------------------------------------------------------------- *\
   test_conversions compares the return values of new_mktime, mew_mkgmtime,
   new_localtime_r and new_gmtime_r around critical dates like begin and end
   of the daylight saving to the one of the system functions.
\* ------------------------------------------------------------------------- */

int test_conversions()
{
   int bRet = 0;
   struct tm stm;
   struct tm otm;
   time_t ot = unix_time();
   time_t tt = unix_time();
   time_t t;
   int64_t i;
   char * pz = getenv("TZ");

   stm.tm_year  = 2024 - 1900;
   stm.tm_mon   =  2;  /* March */
   stm.tm_mday  = 30;
   stm.tm_hour  =  0;
   stm.tm_min   =  0;
   stm.tm_sec   =  0;
   stm.tm_isdst = -1;
   t = mktime(&stm);

   i = 200000;
   while(i--)
   {
      new_localtime_r(t, &stm);
      localtime_r(&t, &otm);

      if(   (otm.tm_year  != stm.tm_year)
         || (otm.tm_mon   != stm.tm_mon)
         || (otm.tm_mday  != stm.tm_mday)
         || (otm.tm_hour  != stm.tm_hour)
         || (otm.tm_min   != stm.tm_min)
         || (otm.tm_sec   != stm.tm_sec)
         || (otm.tm_wday  != stm.tm_wday)
         || (otm.tm_isdst != stm.tm_isdst)
         || (otm.tm_yday  != stm.tm_yday))
      {
         fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ for time_t %ld (0x%lx)!\n"
                  "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                  (long) tt, (long) tt,
                  otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                  stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday );
         goto Exit;
      }

      tt = new_mktime(&stm);
      ot = mktime(&stm);

      if((tt != ot) || (tt != t))
      {
          fprintf (stderr, "Return values of new_mktime() and mktime() are differently for the time %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d)\n"
                   "( %ld (0x%lx) <>  %ld (0x%lx))\n",
                   stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday,
                   (long) tt, (long) tt, (long) ot, (long) ot );
          goto Exit;
      }

      ++t;
   }

   stm.tm_year  = 2024 - 1900;
   stm.tm_mon   =  9; /* October */
   stm.tm_mday  = 26;
   stm.tm_hour  =  0;
   stm.tm_min   =  0;
   stm.tm_sec   =  0;
   stm.tm_isdst = -1;
   t = mktime(&stm);

   i = 200000;
   while(i--)
   {
      new_localtime_r(t, &stm);
      localtime_r(&t, &otm);

      if(   (otm.tm_year  != stm.tm_year)
         || (otm.tm_mon   != stm.tm_mon)
         || (otm.tm_mday  != stm.tm_mday)
         || (otm.tm_hour  != stm.tm_hour)
         || (otm.tm_min   != stm.tm_min)
         || (otm.tm_sec   != stm.tm_sec)
         || (otm.tm_wday  != stm.tm_wday)
         || (otm.tm_isdst != stm.tm_isdst)
         || (otm.tm_yday  != stm.tm_yday))
      {
         fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ for time_t %ld (0x%lx)!\n"
                  "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                  (long) tt, (long) tt,
                  otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                  stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday );
         goto Exit;
      }

      tt = new_mktime(&stm);
      ot = mktime(&stm);

      if((tt != ot) || (tt != t))
      {
          fprintf (stderr, "Return values of new_mktime() and mktime() are differently for the time %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d)\n"
                   "( %ld (0x%lx) <>  %ld (0x%lx))\n",
                   stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday,
                   (long) tt, (long) tt, (long) ot, (long) ot );
          goto Exit;
      }

      ++t;
   }

   stm.tm_year  = 2024 - 1900;
   stm.tm_mon   =  1; /* February */
   stm.tm_mday  = 28;
   stm.tm_hour  =  0;
   stm.tm_min   =  0;
   stm.tm_sec   =  0;
   stm.tm_isdst = -1;
   t = mktime(&stm);

   i = 200000;
   while(i--)
   {
      new_localtime_r(t, &stm);
      localtime_r(&t, &otm);

      if(   (otm.tm_year  != stm.tm_year)
         || (otm.tm_mon   != stm.tm_mon)
         || (otm.tm_mday  != stm.tm_mday)
         || (otm.tm_hour  != stm.tm_hour)
         || (otm.tm_min   != stm.tm_min)
         || (otm.tm_sec   != stm.tm_sec)
         || (otm.tm_wday  != stm.tm_wday)
         || (otm.tm_isdst != stm.tm_isdst)
         || (otm.tm_yday  != stm.tm_yday))
      {
         fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ for time_t %ld (0x%lx)!\n"
                  "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                  (long) tt, (long) tt,
                  otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                  stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday );
         goto Exit;
      }

      tt = new_mktime(&stm);
      ot = mktime(&stm);

      if((tt != ot) || (tt != t))
      {
          fprintf (stderr, "Return values of new_mktime() and mktime() are differently for the time %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d)\n"
                   "( %ld (0x%lx) <>  %ld (0x%lx))\n",
                   stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday,
                   (long) tt, (long) tt, (long) ot, (long) ot );
          goto Exit;
      }

      ++t;
   }

   stm.tm_year  = 2024 - 1900;
   stm.tm_mon   = 11; /* December */
   stm.tm_mday  = 31;
   stm.tm_hour  =  0;
   stm.tm_min   =  0;
   stm.tm_sec   =  0;
   stm.tm_isdst = -1;
   t = mktime(&stm);

   i = 200000;
   while(i--)
   {
      new_localtime_r(t, &stm);
      localtime_r(&t, &otm);

      if(   (otm.tm_year  != stm.tm_year)
         || (otm.tm_mon   != stm.tm_mon)
         || (otm.tm_mday  != stm.tm_mday)
         || (otm.tm_hour  != stm.tm_hour)
         || (otm.tm_min   != stm.tm_min)
         || (otm.tm_sec   != stm.tm_sec)
         || (otm.tm_wday  != stm.tm_wday)
         || (otm.tm_isdst != stm.tm_isdst)
         || (otm.tm_yday  != stm.tm_yday))
      {
         fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ for time_t %ld (0x%lx)!\n"
                  "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                  (long) tt, (long) tt,
                  otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                  stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday );
         goto Exit;
      }

      tt = new_mktime(&stm);
      ot = mktime(&stm);

      if((tt != ot) || (tt != t))
      {
          fprintf (stderr, "Return values of new_mktime() and mktime() are differently for the time %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d)\n"
                   "( %ld (0x%lx) <>  %ld (0x%lx))\n",
                   stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday,
                   (long) tt, (long) tt, (long) ot, (long) ot );
          goto Exit;
      }

      ++t;
   }

   stm.tm_year  = 2024 - 1900;
   stm.tm_mon   =  2;  /* March */
   stm.tm_mday  = 30;
   stm.tm_hour  =  0;
   stm.tm_min   =  0;
   stm.tm_sec   =  0;
   stm.tm_isdst = -1;
   t = mktime(&stm);

   i = 200000;
   while(i--)
   {
      new_localtime_r(t, &stm);
      localtime_r(&t, &otm);

      if(   (otm.tm_year  != stm.tm_year)
         || (otm.tm_mon   != stm.tm_mon)
         || (otm.tm_mday  != stm.tm_mday)
         || (otm.tm_hour  != stm.tm_hour)
         || (otm.tm_min   != stm.tm_min)
         || (otm.tm_sec   != stm.tm_sec)
         || (otm.tm_wday  != stm.tm_wday)
         || (otm.tm_isdst != stm.tm_isdst)
         || (otm.tm_yday  != stm.tm_yday))
      {
         fprintf (stderr, "Return values of localtime_r() and new_localtime_r() differ for time_t %ld (0x%lx)!\n"
                  "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                  (long) tt, (long) tt,
                  otm.tm_year + 1900, otm.tm_mon+1, otm.tm_mday, otm.tm_hour, otm.tm_min, otm.tm_sec, otm.tm_yday, otm.tm_isdst, otm.tm_wday,
                  stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday );
         goto Exit;
      }

      stm.tm_isdst = -1;
      tt = new_mktime(&stm);
      stm.tm_isdst = -1;
      ot = mktime(&stm);

      if(tt != ot)
      {
          fprintf (stderr, "Return values of new_mktime() and mktime() are differently for the time %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d)\n"
                   "( %ld (0x%lx) <>  %ld (0x%lx))\n",
                   stm.tm_year + 1900, stm.tm_mon+1, stm.tm_mday, stm.tm_hour, stm.tm_min, stm.tm_sec, stm.tm_yday, stm.tm_isdst, stm.tm_wday,
                   (long) tt, (long) tt, (long) ot, (long) ot );
          goto Exit;
      }

      ++t;
   }

   bRet = 1;
   Exit:;

   pz = getenv("TZ");

   if(!bRet)
       fprintf(stderr, "Time conversion comparison tests have failed! (TZ=%s)\n\n", pz ? pz : "<empty>");
   else
       fprintf(stdout, "Time conversion comparison tests passed! (TZ=%s)\n\n", pz ? pz : "<empty>");

   return(bRet);
} /* int test_conversions() */



/* ------------------------------------------------------------------------- *\
   test_new_gmtime_r checks days and months as returned by new_gmtime_r in a
   loop for all days from 400BC until 4370 AD. 
\* ------------------------------------------------------------------------- */

int test_new_gmtime_r()
{
   int bRet = 0;

   static uint8_t mday[366] = /* day of a month of in a leap year */
   { 1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31,
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
    1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,26,27,28,29,30,31 };

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
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11 };

   time_t t = (-((int64_t)719528 + 146097 + 146097) * 86400); /* year 800 bc */
   struct tm stm;
   struct tm * ptm;
   int32_t   wday = 6;
   int32_t   year = -800;
   int32_t   day = 0;
   int       leap_year = 1;

   int       sec  = 0;
   int       min  = 0;
   int       hour = 0;

   errno = EAGAIN;

   while (year < 3201)
   {
      ptm = new_gmtime_r(t, &stm);
      if (!ptm)
         goto Exit;

      /* check wday */
      if (ptm->tm_wday != wday)
         goto Exit;

      wday++;
      if (wday == 7)
         wday = 0;

      if (ptm->tm_yday != day)
         goto Exit;

      if (ptm->tm_year != (year - 1900))
         goto Exit;

      if ((day < 59) || leap_year)
      {
         if (ptm->tm_mday != mday[day])
            goto Exit;

         if (ptm->tm_mon != mon[day])
            goto Exit;
      }
      else
      {
         if (ptm->tm_mday != mday[day + 1])
            goto Exit;

         if (ptm->tm_mon != mon[day + 1])
            goto Exit;
      }

      if (++day >= 365)
      {
         if (((day == 365) && !leap_year) || ((day == 366) && leap_year))
         {
            day = 0;
            ++year;
            leap_year = !(year % 4) && ((year % 100) || !(year % 400));
         }
      }

      if (ptm->tm_hour != hour)
         goto Exit;

      if (ptm->tm_min != min)
         goto Exit;

      if (ptm->tm_sec != sec)
         goto Exit;

      if (t != new_timegm(ptm))
         goto Exit;

      {
         struct tm tm1;
         struct tm tm2;

         memset(&tm1, 0, sizeof(tm1));
         tm1.tm_year = ptm->tm_year;
         tm1.tm_mon  = ptm->tm_mon;
         tm1.tm_mday = ptm->tm_mday;
         tm1.tm_hour = ptm->tm_hour;
         tm1.tm_min  = ptm->tm_min;
         tm1.tm_sec  = ptm->tm_sec;

         if ((t != std_timegm(&tm1)) || memcmp(ptm, &tm1, sizeof(*ptm)))
         {
            fprintf (stderr, "std_timegm adjusted the struct memmbers invalid for time_t %ld (0x%lx)!\n"
                     "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                     (long) t, (long) t,
                     tm1.tm_year + 1900,  tm1.tm_mon+1,  tm1.tm_mday,  tm1.tm_hour,  tm1.tm_min,  tm1.tm_sec,  tm1.tm_yday,  tm1.tm_isdst,  tm1.tm_wday,
                     ptm->tm_year + 1900, ptm->tm_mon+1, ptm->tm_mday, ptm->tm_hour, ptm->tm_min, ptm->tm_sec, ptm->tm_yday, ptm->tm_isdst, ptm->tm_wday );

            goto Exit;
         }

         memset(&tm1, 0, sizeof(tm1));
         memset(&tm2, 0, sizeof(tm2));

         new_localtime_r(t, &tm2);

         tm1.tm_year  = tm2.tm_year;
         tm1.tm_mon   = tm2.tm_mon;
         tm1.tm_mday  = tm2.tm_mday;
         tm1.tm_hour  = tm2.tm_hour;
         tm1.tm_min   = tm2.tm_min;
         tm1.tm_sec   = tm2.tm_sec;
         tm1.tm_isdst = tm2.tm_isdst; /* without this mktime may guess a wrong hour when switching to daylight saving or back */

         if ((t != std_mktime(&tm1)) || memcmp(&tm2, &tm1, sizeof(tm2)))
         {
            fprintf (stderr, "std_mktime adjusted the struct memmbers invalid for time_t %ld (0x%lx)!\n"
                     "(%.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d) != %.4d/%.2d/%.2d %.2d:%.2d:%.2d (yd=%d dst=%d wd=%d))\n",
                     (long) t, (long) t,
                     tm1.tm_year + 1900, tm1.tm_mon+1, tm1.tm_mday, tm1.tm_hour, tm1.tm_min, tm1.tm_sec, tm1.tm_yday, tm1.tm_isdst, tm1.tm_wday,
                     tm2.tm_year + 1900, tm2.tm_mon+1, tm2.tm_mday, tm2.tm_hour, tm2.tm_min, tm2.tm_sec, tm2.tm_yday, tm2.tm_isdst, tm2.tm_wday);

            goto Exit;
         }
      }

      ++t;
      if (++sec == 60)
      {
         sec = 0;

         if (++min == 60)
         {
            min = 0;

            if (++hour == 24)
            {
               hour = 0;
               t -= 86400;
            }
         }
      }

      t += 86400; /* next day */
   }

   if(errno != EAGAIN)
   { /* errno should not be set to any kind of a new value within our test loop */
      fprintf(stderr, "errno did change unexpectedly from %d (EAGAIN) to %d (%s)\n", (int) EAGAIN, (int) errno, strerror (errno));
      goto Exit;
   }

   errno = 0;

   bRet = 1;
Exit:;

   if (!bRet)
      fprintf(stderr, "Test loop of new_gmtime_r and new_mkgmtime has failed!\n\n");
   else
      fprintf(stdout, "Test loop of new_gmtime_r and new_mkgmtime for all days from 800 BC til %d AD passed!\n\n", year-1);
   return(bRet);
} /* int test_new_gmtime_r() */



/* ------------------------------------------------------------------------- *\
   main function
\* ------------------------------------------------------------------------- */
int main(int argc, char * argv[])
{
   int iRet = 1;
   TIME_ZONE_INFO tzi;
   TIME_ZONE_INFO tzi_local;

   const char * pTZ = pc_find_TZ("Paris");

#ifdef _WIN32
   TIME_ZONE_INFORMATION wtz_orig;
   /* remember the Windows time zone for restoring it afte the test */
   memset(&wtz_orig, 0, sizeof(wtz_orig));
   GetTimeZoneInformation(&wtz_orig);
#endif

   if(!pTZ || strcmp(pTZ, "CET-1CEST,M3.5.0,M10.5.0/3"))
   {
      printf("pc_find_TZ failed! Return value was '%s'!\n", pTZ ? pTZ : "<NULL>" );
      goto Exit;
   }

#ifndef _WIN32
   setenv("TZ", pTZ, 1);
#else
   if(pTZ)
   { /* tzset() in VC++ is unable to handle the daylight saving rules of the TZ variables right :o(
         For this we use b_read_TZ for parsing the data and SetTimeZoneInformation for setting this. */
      TIME_ZONE_INFORMATION wtz;
      WCHAR *               pwn = wtz.StandardName;
      CHAR *                pn  = tzi.standard.zone_name;

      if(!read_TZ (&tzi, pTZ))
      {
         printf("read_TZ (\"%s\") has failed!\n", pTZ);
         goto Exit;
      }

      memset(&wtz, 0, sizeof(wtz));

      /* copy the time zone names */
      while(*pwn++ = (unsigned char) *pn++) {};

      wtz.StandardDate.wMonth     = tzi.standard.month + 1;
      wtz.StandardDate.wDayOfWeek = tzi.standard.wday;
      wtz.StandardDate.wDay       = tzi.standard.mweek;
      wtz.StandardDate.wHour      = tzi.standard.time / 3600;
      wtz.StandardDate.wMinute    = (tzi.standard.time - wtz.StandardDate.wHour * 3600) / 60;
      wtz.StandardDate.wSecond    = tzi.standard.time % 60;
      wtz.StandardBias            = tzi.standard.bias / 60;

      pwn = wtz.DaylightName;
      pn  = tzi.daylight.zone_name;
      while(*pwn++ = (unsigned char) *pn++) {};

      wtz.DaylightDate.wMonth     = tzi.daylight.month + 1;
      wtz.DaylightDate.wDayOfWeek = tzi.daylight.wday;
      wtz.DaylightDate.wDay       = tzi.daylight.mweek;
      wtz.DaylightDate.wHour      = tzi.daylight.time / 3600;
      wtz.DaylightDate.wMinute    = (tzi.daylight.time - wtz.DaylightDate.wHour * 3600) / 60;
      wtz.DaylightDate.wSecond    = tzi.daylight.time % 60;
      wtz.DaylightBias            = tzi.daylight.bias / 60;

      if(!SetTimeZoneInformation(&wtz))
      {
         printf("SetTimeZoneInformation failed (error=%d)\n", GetLastError());
         goto Exit;
      }
   }
#endif

   if(!read_TZ (&tzi, pTZ))
   {
      printf("read_TZ (\"%s\") has failed!\n", pTZ);
      goto Exit;
   }

   if(!get_local_zone_info(&tzi_local))
   {
      printf("get_local_zone_info() has failed!\n");
      goto Exit;
   }

   if(memcmp(&tzi, &tzi_local, sizeof(tzi)))
   {
      printf("get_local_zone_info returned different settings than specified by '%s'!\n", pTZ);
      goto Exit;
   }

   if(!test_time_range())
      goto Exit;

   if(!test_speed())
      goto Exit;

   if(!test_conversions())
      goto Exit;

#if 0 /* ndef _WIN32 */
   pTZ = "XET-2XEST,M3.4.4/122,M10.4.4/122";
   setenv("TZ", pTZ, 1);

   tzset();
   update_time_zone_info();

   if (!test_speed())
      goto Exit;

   if (!test_conversions())
      goto Exit;

   pTZ = "<-04>4<-03>,M9.1.6/24,M4.1.6/24";

   setenv("TZ", pTZ, 1);

   tzset();
   update_time_zone_info();

   if(!test_speed())
      goto Exit;

   if(!test_conversions())
      goto Exit;
#endif


#ifndef _WIN32
   setenv("TZ", "UTC0", 1);
#else
   putenv("TZ=UTC0");
#endif
   tzset();
   update_time_zone_info();

   if(!test_speed())
      goto Exit;

   if(!test_conversions())
      goto Exit;

#ifndef _WIN32
   setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
#else
   putenv("TZ=CET-1CEST,M3.5.0,M10.5.0/3");
#endif

   tzset();
   update_time_zone_info();

   if (!test_new_gmtime_r())
      goto Exit;

   iRet = 0;
   Exit:;

#ifdef _WIN32
   if(!SetTimeZoneInformation(&wtz_orig))
   {
      printf("SetTimeZoneInformation for restoring original time zone has failed! (error=%d)\n"
             "The Windows time zone settings are possibly invalid now!\n", GetLastError());
   }
#endif

   return(iRet);
}/* main() */

/* ========================================================================= *\
   E N D   O F   F I L E
\* ========================================================================= */
