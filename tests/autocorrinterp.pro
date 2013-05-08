;; 
; @file autocorrinterp.pro
; @author Ann Marie Cody

pro autocorrinterp, time, mag, actime, ac, itime=inttime, imag=maginterp

; Your light curve inputs are time and mag

; Interpolate to a smaller grid with 7040 datapoints. This is somewhat arbitrary- chosen for CoRoT data.
    ndatapoints=7040.
    ndays=time[n_elements(time)-1]-time[0]; Number of days of observation that we have.
    u=ndatapoints/ndays
    inttime=(lindgen(ndatapoints)/u)+time[0]; Grid to interpolate onto

; I have found that this IDL interpolation function works quite well. linterp, on the other hand, produces not so great results.
    maginterp=interpol(mag,time,inttime)

; Compute autocorrelation function:

    lag=lindgen(n_elements(maginterp))
    ac=a_correlate(maginterp-mean(maginterp),lag,double=double)

; And now plot the result:
  plot,lag/u,ac,yrange=[-1,1],thick=2,xtitle='Lag [d]',ytitle='Autocorrelation',charthick=2,charsize=1.5

	actime = lag/u

end
