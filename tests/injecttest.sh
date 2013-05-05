#!/bin/bash

rm -vf injecttest_snr*.log
rm -vf nonspitzernonvar.cat
#ln will print error message on failure
ln -vs test1.cat nonspitzernonvar.cat || exit $?
# soft link creation can fail quietly
if [[ ! -r nonspitzernonvar.cat ]] ; then echo "Cannot read nonspitzernonvar.cat" ; exit 1 ; fi
status=0

nice -n 15 ../lightcurveMC -a "0.5 0.5" -p "0.25 400.0" -w "1.00 1.00" --width2 "1.000 1.000" --ntrials 10 --add NonSpitzerNonVar \
	flat sine \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.30 0.30" --width2 "0.030 0.030" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.23 0.23" --width2 "0.023 0.023" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.18 0.18" --width2 "0.018 0.018" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.13 0.13" --width2 "0.013 0.013" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.10 0.10" --width2 "0.010 0.010" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.08 0.08" --width2 "0.008 0.008" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.06 0.06" --width2 "0.006 0.006" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.04 0.04" --width2 "0.004 0.004" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.03 0.03" --width2 "0.003 0.003" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.02 0.02" --width2 "0.002 0.002" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.01 0.01" --width2 "0.001 0.001" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
diff -s injecttarget_snr1e6.log injecttest_snr1e6.log
# diff returns 0 iff files are equal
status=$(($status || $?))

rm -vf nonspitzernonvar.cat
#ln will print error message on failure
ln -vs test2.cat nonspitzernonvar.cat || exit $?
# soft link creation can fail quietly
if [[ ! -r nonspitzernonvar.cat ]] ; then echo "Cannot read nonspitzernonvar.cat" ; exit 1 ; fi

nice -n 15 ../lightcurveMC -a "0.5 0.5" -p "0.25 400.0" -w "1.00 1.00" --width2 "1.000 1.000" --ntrials 10 --add NonSpitzerNonVar \
	flat sine \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.30 0.30" --width2 "0.030 0.030" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.23 0.23" --width2 "0.023 0.023" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.18 0.18" --width2 "0.018 0.018" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.13 0.13" --width2 "0.013 0.013" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.10 0.10" --width2 "0.010 0.010" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.08 0.08" --width2 "0.008 0.008" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.06 0.06" --width2 "0.006 0.006" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.04 0.04" --width2 "0.004 0.004" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.03 0.03" --width2 "0.003 0.003" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.02 0.02" --width2 "0.002 0.002" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.01 0.01" --width2 "0.001 0.001" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr100.log
diff -s injecttarget_snr100.log injecttest_snr100.log
# diff returns 0 iff files are equal
status=$(($status || $?))

rm -vf nonspitzernonvar.cat
#ln will print error message on failure
ln -vs test3.cat nonspitzernonvar.cat || exit $?
# soft link creation can fail quietly
if [[ ! -r nonspitzernonvar.cat ]] ; then echo "Cannot read nonspitzernonvar.cat" ; exit 1 ; fi

nice -n 15 ../lightcurveMC -a "0.5 0.5" -p "0.25 400.0" -w "1.00 1.00" --width2 "1.000 1.000" --ntrials 10 --add NonSpitzerNonVar \
	flat sine \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.30 0.30" --width2 "0.030 0.030" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.23 0.23" --width2 "0.023 0.023" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.18 0.18" --width2 "0.018 0.018" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.13 0.13" --width2 "0.013 0.013" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.10 0.10" --width2 "0.010 0.010" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.08 0.08" --width2 "0.008 0.008" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.06 0.06" --width2 "0.006 0.006" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.04 0.04" --width2 "0.004 0.004" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.03 0.03" --width2 "0.003 0.003" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.02 0.02" --width2 "0.002 0.002" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.01 0.01" --width2 "0.001 0.001" --ntrials 10 --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr5.log
diff -s injecttarget_snr5.log injecttest_snr5.log
# diff returns 0 iff files are equal
status=$(($status || $?))

exit $?
