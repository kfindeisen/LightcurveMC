#!/bin/bash

# Test case for regression of bug where DRW and WN light curves 
#	had lower amplitudes than predicted
# Root cause was correlations between independent RNGs with the same seed

rm -vf lightcurve_*.dat
rm -vf rngtest_snr*.log
status=0
nice -n 15 ../lightcurveMC -a "0.25 0.25" -d "0.115 0.115" -p "0.1 0.1" --ntrials 20 --noise 0.05 ptfjds.txt \
	white_noise drw --print 1 \
	--stat C1 --stat periplot --stat dmdtcut --stat dmdtplot \
	--stat iacfcut --stat iacfplot --stat sacfcut --stat sacfplot \
	--stat peakcut --stat peakplot \
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


diff -s    run_c1_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_c1_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_c1_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_c1_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

# Output too unstable -- depends on roundoff errors???
#diff -s    run_pgram_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
#	target_pgram_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
#status=$(($status || $?))
#diff -s    run_pgram_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
#	target_pgram_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
#status=$(($status || $?))

diff -s    run_cut50_3_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut50_3_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut50_2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut50_2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut90_3_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut90_3_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut90_2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut90_2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut50_3_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut50_3_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut50_2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut50_2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut90_3_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut90_3_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cut90_2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cut90_2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

diff -s    run_dmdtmed_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_dmdtmed_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_dmdtmed_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_dmdtmed_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

diff -s    run_acf9_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf9_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_acf4_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf4_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_acf2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_acf9_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf9_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_acf4_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf4_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_acf2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

diff -s    run_acf_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_acf_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_acf_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

diff -s    run_sacf9_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_sacf9_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_sacf4_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_sacf4_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_sacf2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_sacf2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_sacf9_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_sacf9_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_sacf4_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_sacf4_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_sacf2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_sacf2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

# Output too unstable -- depends on roundoff errors???
#diff -s    run_sacf_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
#	target_sacf_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
#status=$(($status || $?))
#diff -s    run_sacf_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
#	target_sacf_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
#status=$(($status || $?))

diff -s    run_cutpeak3_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cutpeak3_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cutpeak2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cutpeak2_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cutpeak45_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cutpeak45_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cutpeak3_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cutpeak3_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cutpeak2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cutpeak2_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_cutpeak45_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_cutpeak45_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

diff -s    run_peaks_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_peaks_white_noise_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))
diff -s    run_peaks_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat \
	target_peaks_drw_a0.25_d0.12_p0.10_p0.00_n0.05.dat
status=$(($status || $?))

exit $status
