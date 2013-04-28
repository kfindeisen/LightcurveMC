rm -vf injecttest_snr*.log
rm -vf nonspitzernonvar.cat
ln -s test1.cat nonspitzernonvar.cat
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.30 0.30" --width2 "0.030 0.030" --add NonSpitzerNonVar \
	flat sine slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.23 0.23" --width2 "0.023 0.023" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.18 0.18" --width2 "0.018 0.018" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.13 0.13" --width2 "0.013 0.013" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.10 0.10" --width2 "0.010 0.010" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.08 0.08" --width2 "0.008 0.008" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.06 0.06" --width2 "0.006 0.006" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.04 0.04" --width2 "0.004 0.004" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.03 0.03" --width2 "0.003 0.003" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.02 0.02" --width2 "0.002 0.002" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.01 0.01" --width2 "0.001 0.001" --add NonSpitzerNonVar \
	slow_peak flare_peak flat_peak \
	>> injecttest_snr1e6.log
diff -s injecttarget_snr1e6.log injecttest_snr1e6.log

#rm -vf nonspitzernonvar.cat
#ln -s test2.cat nonspitzernonvar.cat
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.30 0.30" --width2 "0.030 0.030" --add NonSpitzerNonVar \
#	flat sine slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.23 0.23" --width2 "0.023 0.023" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.18 0.18" --width2 "0.018 0.018" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.13 0.13" --width2 "0.013 0.013" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.10 0.10" --width2 "0.010 0.010" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.08 0.08" --width2 "0.008 0.008" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.06 0.06" --width2 "0.006 0.006" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.04 0.04" --width2 "0.004 0.004" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.03 0.03" --width2 "0.003 0.003" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.02 0.02" --width2 "0.002 0.002" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.01 0.01" --width2 "0.001 0.001" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr100.log
#diff -s injecttarget_snr100.log injecttest_snr100.log
#
#rm -vf nonspitzernonvar.cat
#ln -s test3.cat nonspitzernonvar.cat
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.30 0.30" --width2 "0.030 0.030" --add NonSpitzerNonVar \
#	flat sine slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.23 0.23" --width2 "0.023 0.023" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.18 0.18" --width2 "0.018 0.018" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.13 0.13" --width2 "0.013 0.013" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.10 0.10" --width2 "0.010 0.010" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.08 0.08" --width2 "0.008 0.008" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.06 0.06" --width2 "0.006 0.006" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.04 0.04" --width2 "0.004 0.004" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.03 0.03" --width2 "0.003 0.003" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.02 0.02" --width2 "0.002 0.002" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" -w "0.01 0.01" --width2 "0.001 0.001" --add NonSpitzerNonVar \
#	slow_peak flare_peak flat_peak \
#	>> injecttest_snr5.log
#diff -s injecttarget_snr5.log injecttest_snr5.log
