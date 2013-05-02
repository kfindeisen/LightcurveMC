#!/bin/bash

rm -vf paradoxgrid_snr*.log
alias lightcurveMC="nice -n 15 /cygdrive/c/krzys/softdev/CPP/astroapps/lightcurveMC-stable/lightcurveMC"

oldoptions=$(set +o); oldoptions=${oldoptions//set /; set }; oldoptions=${oldoptions/#;/}
set +o noclobber

doGridPoint()
{
	# $1 is output file
	# $2 is 5-95% amplitude
	# $3 is timescale
	# $4 is noise

	# Convert 5-95% amplitudes to amplitude parameters
	# Half-amplitude for sine
	sinAmp=$( echo "scale=10; $2 / 1.975" | bc )
	# RMS for Gaussian processes
	gaussAmp=$( echo "scale=10; $2 / 3.291" | bc )
	# Diffusion coefficient for DRW
	diffus=$( echo "scale=10; 2 * $gaussAmp / $3" | bc )
	
        echo "Output File:" $1
        echo "Amplitude:"   $2
        echo "Timescale:"   $3
        echo "Noise:"       $4

	lightcurveMC -a "$gaussAmp $gaussAmp" -d "$diffus $diffus" -p "$3 $3" --noise $4 paradox_ptfjds.txt \
	 	drw simple_gp --print 4 \
	 	>> $1
}

unset amps
amps=(1.0)

unset times
times=(0.25)

unset noises
noises=(1e-6)
snrs=(1e6)

total=$((${#amps[@]}*${#times[@]}*${#noises[@]}))
echo "Generating light curves over" $total "parameter bins."

for a in ${amps[@]} ; do 
	for t in ${times[@]} ; do 
		for ((i=0; i<${#noises[@]}; i++)) ; do 
			doGridPoint paradoxgrid_snr${snrs[$i]}.log $a $t ${noises[$i]}
		done
	done
done

eval $oldoptions
echo "Simulation complete."
