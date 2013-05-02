#!/bin/bash

rm -vf sinetest_snr*.log
rm -vf nonspitzernonvar.cat
#ln will print error message on failure
ln -vs test1.cat nonspitzernonvar.cat || exit $?
# soft link creation can fail quietly
if [[ ! -r nonspitzernonvar.cat ]] ; then echo "Cannot read nonspitzernonvar.cat" ; exit 1 ; fi

nice -n 15 ../lightcurveMC -a "0.5 0.5" -p "0.25 400.0" --add NonSpitzerNonVar \
	flat sine \
	>> sinetest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" ptfjds.txt \
	flat sine \
	>> sinetest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.1 0.1" -p "0.25 400.0" ptfjds.txt \
	flat sine \
	>> sinetest_snr1e6.log
diff -s sinetarget_snr1e6.log sinetest_snr1e6.log
# diff returns 0 iff files are equal
exit $?
