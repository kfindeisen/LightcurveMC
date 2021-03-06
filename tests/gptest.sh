#!/bin/bash

rm -vf gptest_snr*.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "8.0 8.0" -p "0.25 0.25" --ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "2.0 2.0" -p "1.0 1.0" --ntrials 20 --noise 1e-6 ptfjds.txt --print 2 \
	-s C1 -s dmdtcut white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.5 0.5" -p "4.0 4.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.125 0.125" -p "16.0 16.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut white_noise walk drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.03125 0.03125" -p "64.0 64.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut white_noise walk drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.0078125 0.0078125" -p "256.0 256.0" --ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut white_noise walk drw simple_gp \
	>> gptest_snr1e6.log

nice -n 15 ../lightcurveMC -a "0.3333 0.3333" --amp2 "1.0 1.0" -p "0.025 0.025" --period2 "0.25 0.25" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.3333 0.3333" --amp2 "1.0 1.0" -p "0.0833 0.0833" --period2 "0.25 0.25" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" --amp2 "1.0 1.0" -p "0.025 0.025" --period2 "0.25 0.25" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "3.0 3.0" --amp2 "1.0 1.0" -p "0.0833 0.0833" --period2 "0.25 0.25" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.3333 0.3333" --amp2 "1.0 1.0" -p "0.4 0.4" --period2 "4.0 4.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.3333 0.3333" --amp2 "1.0 1.0" -p "1.3333 1.3333" --period2 "4.0 4.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" --amp2 "1.0 1.0" -p "0.4 0.4" --period2 "4.0 4.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "3.0 3.0" --amp2 "1.0 1.0" -p "1.3333 1.3333" --period2 "4.0 4.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.3333 0.3333" --amp2 "1.0 1.0" -p "6.4 6.4" --period2 "64.0 64.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.3333 0.3333" --amp2 "1.0 1.0" -p "21.3333 21.3333" --period2 "64.0 64.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" --amp2 "1.0 1.0" -p "6.4 6.4" --period2 "64.0 64.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "3.0 3.0" --amp2 "1.0 1.0" -p "21.3333 21.3333" --period2 "64.0 64.0" \
	--ntrials 20 --noise 1e-6 ptfjds.txt \
	-s C1 -s dmdtcut two_gp \
	>> gptest_snr1e6.log

diff -s gptarget_snr1e6.log gptest_snr1e6.log
# diff returns 0 iff files are equal
exit $?
