#!/bin/bash

# Test case for regression of bug where DRW and WN light curves 
#	had lower amplitudes than predicted
# Root cause was correlations between independent RNGs with the same seed

rm -vf lightcurve_*.dat
rm -vf rngtest_snr*.log
status=0
nice -n 15 ../lightcurveMC -a "0.25 0.25" -d "0.115 0.115" -p "0.1 0.1" --ntrials 20 --noise 0.05 ptfjds.txt \
	white_noise drw -s dmdtcut --print 1 \
	>> rngtest_snr20.log
status=$(($status || $?))

diff -s   lctarget_drw_a0.25_d0.12_p0.10_p0.00_n0.05_0.dat \
	lightcurve_drw_a0.25_d0.12_p0.10_p0.00_n0.05_0.dat
status=$(($status || $?))
diff -s   lctarget_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05_0.dat \
	lightcurve_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05_0.dat
status=$(($status || $?))
diff -s rngtarget_snr20.log rngtest_snr20.log
status=$(($status || $?))

exit $status
