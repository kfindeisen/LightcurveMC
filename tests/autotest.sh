rm *test*.log
rm lightcurve*.dat
./test.exe -t test_dmdt,test_stats
#source sinetest.sh
#source injecttest.sh
source periodictest.sh
source gptest.sh
source hiamptest.sh
rm run_*.dat
