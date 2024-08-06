limitless_times - time handling functions which overcome some annoying limits

It did bother me since a while that the common time functions in C are not only
quite slow usually but that it's not even possible to use them for any dates
and times before 1970. It's because the implementations of those functions are
usually unable to handle any negative time_t values.
Beside of that is there that nightmare of the daylight saving rules in the
different times zones. It's not hard to handle of course as long as you know
the daylight saving rules. And if it comes to myself I did want some fast and
reliable functions for my own applications and the network logging in the
different systems all over the world.

The wrappers for gmtime_r, mkgmtime, mktime and localtime_r here can handle
Gregorian time even back to the age of dinosaur and also the same time span
ahead in the future.
And there are the functions mktime_of_zone() and localtime_of_zone() as well
which enable a thread save conversion between time_t and the times in given
time zones and care about the daylight saving rules of those zones.

Thread safety may be a problem in the wrappers new_mktime() and
new_localtime_r() because the function are relying of the environment parameter
TZ which can be adjusted at runtime. For speeding up the things a static
pre-calculated struct is used that contains the local time-zone information.

For being thread safe you need to call update_time_zone_info() before creating
any threads. In case that you need changes of TZ or your local time zone during
at random times once your process is running you need to provide an own thread
lock and a unlock callback function to a call of init_time_api_lock().
the functions should call a recursive mutex or in case of Windows a critical
section internally.
The C standard says about the *_r functions that they "shall not be subject
to data races, unless the time or calendar state is changed in a multi-thread
execution." You should use the provided functions of this or a similar API
only if you need something like that in multi-threaded processes.

The support of the daylight saving rules are not that funny to implement but
new_mktime and new localtime_r should handle them right as long as the
environment variable TZ is set or can be found and conforms the Unix standard.

The default value is searched in /etc/localtime if that file exists as common
in many Unix systems. The algorithm doesn't care the true binary format of
the time zone data base files but extracts the TZ value at the end of that
file only. This hack should work in Linux and BSD for most countries and time
zones but the big bunch of the historical daylight saving time rules which may
apply in case of historical times are currently ignored.
Of course I doubt that any of us will go back in time for enjoying those old
days again and for this it shouldn't be a big problem. Be aware that functions
don't care about any leap seconds as well. Those are applied at random times
for adjusting the Gregorian time against the UTC time a bit but a Gregorian
year has an even bigger deviation from an average tropical year either and leap
seconds can't fix the deviation problems that the GPS and other navigation
systems face either. The Unix standard says "As represented in seconds since
the Epoch, each and every day shall be accounted for by exactly 86400 seconds."
( https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_15 )
For this the implementations of the the most systems ignore the leap seconds
and I guess it's pretty OK to ignore them as well.
The Gregorian year lasts currently a bit longer than a tropical year either
but in a few ten thousend years after the earth rotation has slowed down a
bit more the time will match again and for this it seems rather an academic
problem of nitpickers only who have no patience to wait.

For testing the functions and comparing the speed with the compiler build-in
functions you may execute the test_times.c as shell script in a Linux or BSD
system of your choise where you have access the compiler.
I did share my little test solution for Visual Studio as well now.

Why that 'Civil Usage Public License'?
The license is kind a mix of the conditions of BSD or Apache license but in
opposite to the former it prohibits any usage for weapons, spyware and secret
monitoring of people without their knowledge and prior agreement.
I dislike the idea to find anything of my software in military devices, weapons
or spyware because it's traceably in the binaries of the most recently
available compilers. The license ist not a big deal for most people except for
the ones who make money out of wars and things that are usually a pretty nasty
stuff. I don't expect anything good in return of supporting those for free.
The license is much more permissive if it comes to commercial usage than
something like the GPL. But despite of that it's for sure a good idea to use
the software legally only and to care about the conditions of the license.
Please be aware that this code is despite of its very permissive license not at
all public domain software! Except the time zones from TZ database which remain
public domain as in the sources from https://www.iana.org/time-zones .
But life is great and now it's time for some limitless times, don't you agree?


Kind regards,

Klaus Lux
