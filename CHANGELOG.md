# Changelog of klux21/limitless_times

## limitless_times_2.4.6 / 2026-07-10
 - CHANGELOG.md added
 - initial version of run_tests.sh
 - README.txt replaced by README.md

## limitless_times_2.4.5 / 2026-06-26
 - typos (also in the license) corrected 
  
## limitless_times_2.4.4 / 2026-06-03
 - calender_week_of_time uses local time now

## limitless_times_2.4.3 / 2026-29-05
 - minior adjustment for Cygwin
 - Canada/Pacific and America/Vancouver updated

## limitless_times_2.4.2 / 2026-04-19
 - SIZE_MAX, PTRDIFF_MAX, intptr_t, and uintprtr_t in Win32/inttypes.h defined

## limitless_times_2.4.1 / 2026-04-05
 - adjusted according tzdb 2026a
 - the tests are using gmtime_s and localtime_s as the wrappers
   of gmtime_r and localtime_r in Windows

## limitless_times_2.4 / 2026-02-15
 - unix_time_ns added to the API
 - README.txt adjusted

## limitless_times_2.3.1 / 2026-02-06
 - time.h include in time_api.h again
 - minor adjustments of localtime_r
 
## limitless_times_2.3 / 2025-12-09
 - possibly missing header sys/times.h included again

## limitless_times_2.2 / 2025-12-02
 - unused system headers removed from time_api.h

## limitless_times_2.1 / 2025-12-01
 - std_timegm and std_mktime are returning time64_t values now

## limitless_times_2.0 / 2025-11-27
 - uses time64_t now always
 - no unnecessary pointer arguments for non standard wrappers

## limitless_times_1.5 / 2025-06-02
 - error handling of std_timegm and std_mktime improved
 - some test cases for std_timegm and std_mktime added
 - new_mkgmtime renamed to new_timegm
 - new functions std_timegm and std_mktime added

## limitless_times_1.4.5 / 2025-05-30
 - array of precalculated start time of daylight saving and standard time added again

## limitless_times_1.4.4 / 2025-05-27
 -  cache of precalculated values removed

## limitless_times_1.4.3 / 2025-05-27
 - faster differentiation between standard time and daylight saving

## limitless_times_1.4.2 / 2025-05-25
 - redundant code moved to new static function get_rule_time

## limitless_times_1.4.1 / 2025-07-10
 - ignores quotes of the time zone names in TZ variable now
 - error logging corrected
 - leap year calculation simplified and less redundant statics
 - calculation of weekday of daylight saving date adjusted
 - test_new_gmtime_r added

## limitless_times_1.4 / 2025-05-10
 - initial support of tm_gmtoff and tm_zone in the struct tm of Posix 2024 systems
 - supports time offset up to 167 hours now as specified in Posix 2024

## limitless_times_1.3.6 / 2025-04-16
 - minor optimization of new_gmtime_r

## limitless_times_1.3.5 / 2025-04-08
 - America/Coyhaique added

## limitless_times_1.3.4 / 2025-02-16
 - tz_value.c adjusted according tzdb-2025a
 - TZ value of America/Asuncion adjusted

## limitless_times_1.3.3 / 2024-12-31
 - overflow in handling in  new_gmtime_r new_localtime_r improved

## limitless_times_1.3.2 / 2024-12-05
 - project description improved

## limitless_times_1.3.1 / 2024-09-20
 - minor time zone correction according to tzdb-2024b

## limitless_times_1.3 / 2024-08-20
 - API enhanced for handling thread safety problems now
 - call of optional user provided thread lock and unlock callbacks added 

## limitless_times_1.2.8 / 2024-05-08
 - unix_time() added to time API
 - uses stdint.h instead of inttypes.h now 
 - additional defines for compatibility added
 - README.txt adjusted

## limitless_times_1.2.7 / 2024-03-09
 - obsolete struct member removed

## limitless_times_1.2.6 / 2024-03-04
 - inttypes.h and stdint.h for old VC++ compilers added

## limitless_times_1.2.5 / 2024-02-18
 - Civil Usage Public License, Version 1.1, January 2024
 - b_read_TZ renamed to read_TZ
 - new function get_local_zone_info() added

## limitless_times_1.2.4 / 2024-02-16
 - pc_find_TZ() in zones/tz_value.c and related test added
 - some not Posix conform daylight saving rules are supported now

## limitless_times_1.2.3 / 2024-02-15
 - identifiers renamed for better matching C standard

## limitless_times_1.2.2 / 2024-02-12
 - UnixTime renamed to unix_time

## limitless_times_1.2.1 / 2024-02-12
 - unnecessary cast removed
 - typos in comments corrected

## limitless_times_1.2 / 2024-02-10
 - uses GetSystemTimePreciseAsFileTime instead of GetSystemTimeAsFileTime
   for the time in Windows now (if available)
 - declared tz_value.c and tz_value.h to be public domain now
 - provides TZ values (tzdb 2024a) in form of an array
 - initial version of .gitignore added
 - mktime returnes standard time in case of an ambiguous time values now
 - better handling of an empty TZ variable
 - 32bit performance of new_gmtime_r, new_localtime_r and localtime_of_zone improved
 - initial version of the WIN32 test project
 - update_time_zone_info() needs to be called for caring any changes of TZ now
 - handling of daylight saving termination rules improved
 - support of the hour 24 in TZ rules for daylight saving added because found in timezone database
 - mktime_of_zone() and localtime_of_zone() added and bugfix of update_time_zone_info if TZ is not set
 
## limitless_times_1.1 / 2024-02-01
 - vInitTimeZoneInfo  renamed to udate_time_zone_info
 - typo in update_time_zone_info corrected

## limitless_times_1.0 / 2024-01-31
 - initial version
 - Civil Usage Public License, Version 1.1, January 2024
