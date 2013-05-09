#!/bin/bash

rm -vf hiamptest_snr*.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "1.0 1.0" -p "0.25 0.25" --ntrials 20 --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp -s C1 -s dmdtcut \
	>> hiamptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "1.0 1.0" -p "1.0 1.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp -s C1 -s dmdtcut \
	>> hiamptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "1 1" -p "4.0 4.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp -s C1 -s dmdtcut \
	>> hiamptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "1 1" -p "16.0 16.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp -s C1 -s dmdtcut \
	>> hiamptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "1 1" -p "64.0 64.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp -s C1 -s dmdtcut \
	>> hiamptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "1 1" -p "256.0 256.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp -s C1 -s dmdtcut \
	>> hiamptest_snr1e6.log
diff -s hiamptarget_snr1e6.log hiamptest_snr1e6.log
# diff returns 0 iff files are equal
exit $?
