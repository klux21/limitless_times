limitless_times - time handling functions which overcome some annoying limits

It did bother me that several time function are not only quite slow but also
that it's not even possible to use those for the calculate my own age. It's
just because of the implementations of functions like mktime or gmtime of
most systems and C compilers are unable to handle dates before 1970 despite
that the time_t value is a signed data type. Beside of that is there the
nightmare of the daylight saving in the different times zones and correct
time diffing between the times of different zones. It's kind of easy to do of
course once you aware of the daylight saving rules as provided by the tz
library. As it comes to myself I need some really fast and reliable functions
for my application and network logging at the manufacturers all over the
world. 

The wrappers for gmtime_r, mkgmtime, mktime and localtime_r here can handle
Gregorian time even back to the age of dinosaur and also the same time span
ahead in the future.

And there are the functions mktime_of_zone() and localtime_of_zone() that
provide a thread save conversions between time_t and the times of different
time zones then the local one and care about the daylight saving rules of
the several time zones.

Thread safety is is till an issue in new_mktime() and new_localtime_r()
because that caring of an environment parameter like TZ which that can be
adjusted at runtime requires updates of static pre-calculated values which
contain the time-zone information.
For being thread safe you need to call new_mktime() or update_time_zone_info()
before creating any threads. After every change of TZ afterwards
update_time_zone_info() must be called for the activation of the changes. 

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
system of your choise where you have access the compiler.
I did share my little test solution for Visual Studio as well.

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
Please be aware that this code is despite of its very permissive license not
even partially public domain software!
But now it's about time for some limitless times, don't you agree?


Kind regards,

Klaus Lux
