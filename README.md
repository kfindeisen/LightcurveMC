LightcurveMC
============

LightcurveMC is a versatile and easily extended simulation suite for testing the performance of time series analysis tools under controlled conditions. LightcurveMC is designed to be highly modular, allowing new lightcurve types or new analysis tools to be introduced without excessive development overhead. The statistical tools are completely agnostic to how the lightcurve data is generated, and the lightcurve generators are completely agnostic to how the data will be analyzed. The use of fixed random seeds throughout guarantees that the program generates consistent results from run to run.

Current features include:
* Generation of periodic light curves having a variety of shapes
* Generation of stochastic light curves having a variety of correlation properties
* Two error models: Gaussian measurement error, or signal injection using a randomized sample of base light curves
* Testing of C1 shape statistic, periodograms, ΔmΔt plots, autocorrelation function plots, peak-finding plots, and Gaussian process regression.

This software has been published through the Astrophysics Source Code Library as [ascl:1408.012](http://ascl.net/1408.012). Please cite as 
> Findeisen, K. 2014, Astrophysics Source Code Library, record ascl:1408.012.
