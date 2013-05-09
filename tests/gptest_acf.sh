#!/bin/bash

rm -vf gptest_acf_snr*.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "8.0 8.0" -p "0.25 0.25" --ntrials 20 --noise 1e-6 ptfjds_2011.txt \
	-s acfcut -s acfplot white_noise drw simple_gp \
	>> gptest_acf_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "2.0 2.0" -p "1.0 1.0" --ntrials 20 --noise 1e-6 ptfjds_2011.txt --print 2 \
	-s acfcut -s acfplot white_noise drw simple_gp \
	>> gptest_acf_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.5 0.5" -p "4.0 4.0" --ntrials 20 --noise 1e-6 ptfjds_2011.txt \
	-s acfcut -s acfplot white_noise drw simple_gp \
	>> gptest_acf_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.125 0.125" -p "16.0 16.0" --ntrials 20 --noise 1e-6 ptfjds_2011.txt \
	-s acfcut -s acfplot white_noise walk drw simple_gp \
	>> gptest_acf_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.03125 0.03125" -p "64.0 64.0" --ntrials 20 --noise 1e-6 ptfjds_2011.txt \
	-s acfcut -s acfplot white_noise walk drw simple_gp \
	>> gptest_acf_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.0078125 0.0078125" -p "256.0 256.0" --ntrials 20 --noise 1e-6 ptfjds_2011.txt \
	-s acfcut -s acfplot white_noise walk drw simple_gp \
	>> gptest_acf_snr1e6.log
#diff -s gptarget_acf_snr1e6.log gptest_acf_snr1e6.log
# diff returns 0 iff files are equal
exit $?