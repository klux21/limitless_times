#!/bin/sh
rm -f ./_test_times
cc -Wall -O3 -o _test_times -I . -I zones test_times.c time_api.c zones/tz_value.c
./_test_times
exit $?
