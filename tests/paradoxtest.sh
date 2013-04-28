#! /bin/bash
rm -vf paradoxtest_snr*.log

nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "8.0 8.0" -p "0.25 0.25" --noise 1e-6 ptfjds.txt \
	magsine white_noise drw simple_gp \
	> paradoxtest_snr1e6.log
diff -s paradoxtarget_snr1e6.log paradoxtest_snr1e6.log
