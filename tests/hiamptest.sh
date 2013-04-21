rm -vf hiamptest_snr*.log
../lightcurveMC -a "1.0 1.0" -d "1.0 1.0" -p "0.25 0.25" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> hiamptest_snr1e6.log
../lightcurveMC -a "1.0 1.0" -d "1.0 1.0" -p "1.0 1.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> hiamptest_snr1e6.log
../lightcurveMC -a "1.0 1.0" -d "1 1" -p "4.0 4.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> hiamptest_snr1e6.log
../lightcurveMC -a "1.0 1.0" -d "1 1" -p "16.0 16.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> hiamptest_snr1e6.log
../lightcurveMC -a "1.0 1.0" -d "1 1" -p "64.0 64.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> hiamptest_snr1e6.log
../lightcurveMC -a "1.0 1.0" -d "1 1" -p "256.0 256.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> hiamptest_snr1e6.log
diff -s hiamptest_snr1e6.log hiamptarget_snr1e6.log
