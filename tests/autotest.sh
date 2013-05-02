#!/bin/bash

rm -vf *test*.log
rm -vf lightcurve*.dat
status=0

#-----------------------------------------------------------
# Commented-out tests are deemed too expensive for regular regression testing
# Run them, but less frequently

nice -n 15 ./test -t test_dmdt,test_stats,test_waves
#nice -n 15 ./test -t test_gp
status=$(($status || $?))

./cmdtest.sh		; status=$(($status || $?))
./gptest.sh		; status=$(($status || $?))
./hiamptest.sh		; status=$(($status || $?))
#./injecttest.sh		; status=$(($status || $?))
./periodictest.sh	; status=$(($status || $?))
./sinetest.sh		; status=$(($status || $?))
rm -f run_*.dat

if [[ $status == 0 ]] ; then
	echo "All tests completed successfully."
else 
	echo "Some tests failed. Please examine output for details."
fi
exit $status
