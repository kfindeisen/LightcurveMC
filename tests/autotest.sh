#! /bin/bash

rm -vf *test*.log
rm -vf lightcurve*.dat
status=0

nice -n 15 ./test.exe -t test_dmdt,test_stats	; status=$(($status || $?))
source sinetest.sh				; status=$(($status || $?))
#source injecttest.sh				; status=$(($status || $?))
source periodictest.sh				; status=$(($status || $?))
source cmdtest.sh				; status=$(($status || $?))
source gptest.sh				; status=$(($status || $?))
source hiamptest.sh				; status=$(($status || $?))
source paradoxtest.sh				; status=$(($status || $?))
rm -f run_*.dat

if [[ $status == 0 ]] ; then
	echo "All tests completed successfully."
else 
	echo "Some tests failed. Please examine output for details."
fi
return $status
