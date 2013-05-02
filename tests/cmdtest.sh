#!/bin/bash

# Tests the command-line interface
rm -vf cmdtest_*.log
rm -vf nonspitzernonvar.cat
#ln will print error message on failure
ln -vs test1.cat nonspitzernonvar.cat || exit $?
# soft link creation can fail quietly
if [[ ! -r nonspitzernonvar.cat ]] ; then echo "Cannot read nonspitzernonvar.cat" ; exit 1 ; fi

echo "EXPECTED RESULT: FAIL (invalid range)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0" -p "0.25 0.25" --ntrials 5 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (negative range)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "-1.0 -0.5" -p "0.25 0.25" --ntrials 5 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (inverted range)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "1.0 0.25" --ntrials 5 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (no tests)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 0 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (negative tests)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials -10 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (fractional tests)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5.5 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (negative noise)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --noise -0.1 ptfjds.txt \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: FAIL (no simulation parameters)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (jdList but no waves)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 ptfjds.txt \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (jdList and bad wave given)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 ptfjds.txt \
	fakeTestWave \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: RUN (jdList and one wave given)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 ptfjds.txt \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: RUN (jdList and multiple waves given)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 ptfjds.txt \
	magsine white_noise \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (--add but no waves)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (--add with bad argument)" &>> cmdtest_domain.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add GarbageTestSample \
	magsine \
	&>> cmdtest_domain.log

echo "EXPECTED RESULT: RUN (--add and one wave given)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: RUN (--add and multiple waves given)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar \
	magsine white_noise \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (both --add and jdList given, but no waves)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar ptfjds.txt \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (both --add and jdList given, with one wave)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar ptfjds.txt \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (both --add and jdList given, with multiple waves)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar ptfjds.txt \
	magsine white_noise \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (--noise given without jdList)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --noise 0.01 \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (--noise and jdList given, but no waves)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --noise 0.01 ptfjds.txt \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: RUN (--noise and jdList given, with one wave)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --noise 0.0 ptfjds.txt \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: RUN (--noise and jdList given, with multiple waves)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --noise 0.01 ptfjds.txt \
	magsine white_noise \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (--noise and --add given)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar --noise 1e-6 \
	magsine \
	&>> cmdtest_xor.log

echo "EXPECTED RESULT: FAIL (--noise, --add, and jdList all given)" &>> cmdtest_xor.log
nice -n 15 ../lightcurveMC -a "1.0 1.0" -p "0.25 0.25" --ntrials 5 --add NonSpitzerNonVar --noise 1e-6 ptfjds.txt \
	magsine \
	&>> cmdtest_xor.log

diff -s cmdtarget_xor.log cmdtest_xor.log
diff -s cmdtarget_domain.log cmdtest_domain.log
# diff returns 0 iff files are equal
exit $?
