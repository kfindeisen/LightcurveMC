#!/bin/bash

echo `date`
rm -vf speedtest_snr*.log
../lightcurveMC -a "1.0 1.0" -d "0.5 0.5" -p "4.0 4.0" --ntrials 5 --noise 0.05 ptfjds.txt \
	magsine drw simple_gp \
	>> speedtest_snr1e6.log
status=$?
echo `date`
exit $status
