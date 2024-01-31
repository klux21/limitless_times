limitless_times - time handling functions which overcome some annoying limits

It did bother me that several time function are not only quite slow but also
that it's not even possible to use those for the calculate my own age just
because the implementations of functions like mktime or gmtime of most systems
and C compilers are unable to handle dates before 1970 despite that the time_t
value is a signed data type. So this bunch of functions are a thing that I did
want to implement since a long time already.

The wrappers for gmtime_r, mkgmtime, mktime and localtime_r here can handle
Gregorian time even back to the age of dinosaur and also the same time span
ahead in the future.

Thread safety is is till an issue in new_mktime() and new_localtime_r()
because that caring of an environment parameter like TZ which that can be
adjusted at runtime requires updates of some pre-calculated values which
contain the time-zone information.
For being thread safe you need to call new_mktime() or udate_time_zone_info()
before creating any threads and to prevent adjustments of the TZ environment
variable afterwards.

The support of the daylight saving rules are not that funny to implement but
new_mktime and new localtime_r should handle them right as long as the
environment variable TZ is set and conforms the Unix standard.

The values right searched in /etc/localtime if TZ referes to a file there as
common in many Unix systems. The algorythm doesn't cares the true binary
format of the time zone data base files but looks for the TZ value
at the end of that file only. This hack should work in Linux and BSD for most
countries and time zones but the big bunch of the historical daylight saving
time rules which may apply in case of historical times are still ignored.

Of course I doubt that any of us will go back in time for enjoying those old
days again and for this it shouldn't be a big problem. Be aware that functions
don't care about any leap seconds as well. Those are applied at random times
for adjusting the Gregorian time against the UTC time a bit but a Gregorian
year has an even bigger deviation from an average tropical year either and leap
seconds can't fix the deviation problems that the GPS and other navigation
systems face either. For being Unix standard conform it's required as well that
"As represented in seconds since the Epoch, each and every day shall be
accounted for by exactly 86400 seconds."
( https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_15 )
For this the implementations of the the most systems ignore leap seconds and
I guess it's pretty OK to ignore leap seconds as well.
The Gregorian year lasts currently a bit longer than a tropical year either
but in a few ten thousend years in the future after the earth rotation has
slowed down a bit more then the time will match it's rotation again and for
this it seems to be a rather academic problem of some nitpickers only who have
no patience to wait a little bit.

For testing the functions and comparing the speed with the compiler build-in
functions you may execute the test_times.c as shell script in a Linux or BSD
system where you have access the compiler.

Why that 'Civil Usage Public License'?
The license is kind a mix of the conditions of BSD or Apache license but in
opposite to the former it prohibits any usage for weapons, spyware and secret
monitoring of people without their knowledge and prior agreement.
I dislike the idea to find anything of my software in military devices, weapons
or spyware because it's traceably in the binaries of the most recently
available compilers.
That's not a big deal for most people except for the ones who make money out
of wars and things that are usually pretty nasty. I don't expect anything
good in return of supporting those people for free. The license shouldn't be a
big problem for most people but because it is much more permissive if it
comes to commercial usage than something like the GPL. But despite of that it's
for sure a good idea to use the software legally only regarding the conditions
of the license.
Please be aware that this code is despite of this very permissive license not
even partially public domain software! For this it's a very good idea to care
about the license conditions.


Kind regards,

Klaus Lux
