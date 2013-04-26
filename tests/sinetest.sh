rm -vf _sinetest_snr*.log
rm -vf nonspitzernonvar.cat
ln -s test1.cat nonspitzernonvar.cat
../lightcurveMC -a "1.0 1.0" -p "0.25 400.0" --add NonSpitzerNonVar ptfjds.txt flat sine > sinetest_snr1e6.log
diff -s sinetest_snr1e6.log sinetarget_snr1e6.log
