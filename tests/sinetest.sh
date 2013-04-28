rm -vf sinetest_snr*.log
rm -vf nonspitzernonvar.cat
ln -s test1.cat nonspitzernonvar.cat
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" --add NonSpitzerNonVar \
	flat sine \
	>> sinetest_snr1e6.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" ptfjds.txt \
	flat sine \
	>> sinetest_snr1e6.log
nice -n 15 ../lightcurveMC -a "0.1 0.1" -p "0.25 400.0" ptfjds.txt \
	flat sine \
	>> sinetest_snr1e6.log
diff -s sinetarget_snr1e6.log sinetest_snr1e6.log
