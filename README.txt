limitless_times - time handling functions which overcome some annoying limits

It did bother me since a while that the common time functions in C are not only
quite slow usually but that it's not even possible to use them for any dates
and times before 1970. It's because the implementations of those functions are
usually unable to handle any negative time_t values.
even worse there exist that nightmare of handling the daylight saving rules and
times in the different times zones. It's not hard to do of course as long as
you know the time offset and daylight saving rules. And if it comes to myself I
did want some fast and reliable portable functions for my own applications and
especially network logging of those in all the systems all over the world.

The wrappers for gmtime_r, mkgmtime, mktime and localtime_r here can handle
Gregorian time even back to the age of dinosaur and also the same time span
ahead in the future. Additionally there there are the functions
mktime_of_zone() and localtime_of_zone() now which provide a thread save
conversion between the UTC time of a time_t and the times in given time zones
and can handle the daylight saving rules of the different zones. For speeding
the calculations up a static pre-calculated struct can be used that contains
the local time-zone information.

Thread safety may be a problem in mktime() and localtime_r() because the
functions rely of the environment parameter TZ which can be adjusted at runtime.
For speeding the things up a static pre-calculated struct can be used now that
contains the local time-zone information. The C standard says about the *_r
time functions that they "shall not be subject to data races, unless the time
or calendar state is changed in a multi-thread execution." That's why it's
dangerous to use those functions in big multi-threaded and portable software
projects if time zone adjustments at runtime may happen. 

For being thread safe you need just call update_time_zone_info() before
creating any threads. In case that you need changes of TZ or your local time
zone during at random times once your process is running you may provide an
own mutex lock and unlock function for init_time_api_lock() and rely on the
functions provided by this API.

The support of the daylight saving rules are not that funny to implement but
new_mktime and new localtime_r should handle them right as long as the
environment variable TZ is set and conforms the Unix standard.

The default value is searched in /etc/localtime if that file exists as common
in many Unix systems. The algorithm doesn't care the true binary format of
the time zone data base files but extracts the TZ value at the end of that
file only. This works in Linux and BSD but does not care about the big bunch
of the historical time offsets and daylight saving rules.
Of course I doubt that any of us will go back in time for enjoying those old
days again and for this it shouldn't be a big problem for most developers.
Be aware that the provided functions don't care about any leap seconds as well.
Those are applied at random times for adjusting the timegap between the very
local Gregorian time and the UTC time but a Gregorian year has an even bigger
deviation from an average tropical year either and leap seconds can't fix the
problems that the GPS and other systems face either.
The Unix standard says that "As represented in seconds since the Epoch, each
and every day shall be accounted for by exactly 86400 seconds."
( https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap04.html#tag_04_15 )
For this leap seconds are usually ignored and I guess it's pretty OK to do
so as well. A Gregorian year lasts currently a bit longer than a tropical year
either and in a few ten thousend years after the earth rotation has slowed down
a bit more the time will match again and for this it seems rather an academic
problem of a nitpicker if somebody has a problem with that.

For testing the functions and comparing the speed with the compiler build-in
functions you may execute the test_times.c right as a shell script in a Linux
or BSD system of your choice where a C compiler exist.
There is a little test solution for Visual Studio as well now here.

Why that 'Civil Usage Public License'? The license is kind of a little mix of
the conditions of BSD or Apache license but in opposite to the former it
prohibits any usage for weapons, spyware and systems for a secret monitoring of
people without their knowledge and prior agreement.
I dislike the idea to find anything of my software in military devices, weapons
or spyware because it's traceably in the binaries of the most recently
available compilers. The license ist not a big deal for most people except for
the ones who make money out of wars and things that are usually a nasty stuff.
I don't expect anything good in return of supporting those people with this.
But the license is much more permissive if it comes to a commercial usage than
something like the GPL. It's for sure a good idea to use the software legally
only and to care about the conditions of the license. Please be aware that this
code is despite of its very permissive license no public domain software!
( Except the list of time zones from TZ database which remain public domain as
in the IANA time-zone database from https://www.iana.org/time-zones in version
2024a. )

Life is great once there are less limits and problems and for this it's about
time for limitless times now.

Kind regards,
Klaus Lux
