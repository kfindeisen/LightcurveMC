/** Helper functions for generating light curves
 * @file lightcurveMC/waves/generators.h
 * @author Krzysztof Findeisen
 * @date Created April 18, 2013
 * @date Last modified May 6, 2013
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

#ifndef LCMCGENERATORSH
#define LCMCGENERATORSH

#include <vector>
#include <boost/smart_ptr.hpp>
#include <gsl/gsl_matrix.h>

namespace lcmc { namespace utils {

/** Transforms an uncorrelated sequence of Gaussian random numbers into a 
 *	correlated sequence
 */
void multiNormal(const std::vector<double>& indVec, const boost::shared_ptr<gsl_matrix>& covar, 
		std::vector<double>& corrVec);

}}		// end lcmc::utils

#endif		// LCMCGENERATORSH
