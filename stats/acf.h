/** Temporary replacements for a lag-n autocorrelation function
 * @file lightcurveMC/stats/acf.h
 * @author Krzysztof Findeisen
 * @date Created May 7, 2013
 * @date Last modified May 7, 2013
 */

/* Copyright 2014, California Institute of Technology.
 *
 * This file is part of LightcurveMC.
 * 
 * LightcurveMC is free software: you can redistribute it and/or 
 * modify it under the terms of the GNU General Public License as published 
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, subject to the following 
 * exception added under Section 7 of the License:
 *	* Neither the name of the copyright holder nor the names of its contributors 
 *	  may be used to endorse or promote products derived from this software 
 *	  without specific prior written permission.
 * 
 * LightcurveMC is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with LightcurveMC. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LCMCACFH
#define LCMCACFH

namespace lcmc { namespace stats { 

/** This function computes the lag-@p n autocorrelation of the dataset @p data, 
 *	using the signal-processing convention.
 */
void autoCorrelation_sp(const double data[], double acfs[], size_t n);

/** This function computes the lag-@p n autocorrelation of the dataset @p data, 
 * 	using the statistical analysis convention.
 */
void autoCorrelation_stat(const double data[], double acfs[], size_t n);

}}		// end lcmc::stats

#endif		// LCMCACFH
