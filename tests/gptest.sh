rm -vf gptest_snr*.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "8.0 8.0" -p "0.25 0.25" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "2.0 2.0" -p "1.0 1.0" --noise 1e-6 ptfjds.txt --print 2 \
	white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.5 0.5" -p "4.0 4.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.125 0.125" -p "16.0 16.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.03125 0.03125" -p "64.0 64.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> gptest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -d "0.0078125 0.0078125" -p "256.0 256.0" --noise 1e-6 ptfjds.txt \
	white_noise drw simple_gp \
	>> gptest_snr1e6.log
diff -s gptarget_snr1e6.log gptest_snr1e6.log
