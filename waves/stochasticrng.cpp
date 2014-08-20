/** Defines a random number generator wrapper
 * @file lightcurveMC/waves/stochasticrng.cpp
 * @author Krzysztof Findeisen
 * @date Created May 12, 2013
 * @date Last modified May 12, 2013
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

#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>
#include "lcstochastic.h"

namespace lcmc { namespace models {

/** Initializes a random number generator
 *
 * @param[in] seed the initial seed for the generator.
 *
 * @exception std::bad_alloc Thrown if there was not enough memory 
 *	to construct the generator.
 *
 * @exceptsafe Object construction is atomic.
 */
StochasticRng::StochasticRng(unsigned long seed) : rng(gsl_rng_alloc(gsl_rng_mt19937)) {
	gsl_rng_set(rng, seed);
}

StochasticRng::~StochasticRng() {
	gsl_rng_free(rng);
}

/** Creates a random number generator with an identical 
 *	state to another
 *
 * @param[in] other The generator to duplicate
 *
 * @exception std::bad_alloc Thrown if there was not enough memory 
 *	to construct the generator.
 *
 * @exceptsafe Object construction is atomic.
 */
StochasticRng::StochasticRng(const StochasticRng& other) : rng(gsl_rng_clone(other.rng)) {
}

/** Sets the random number generator state equal to another generator
 *
 * @param[in] other The generator to duplicate
 *
 * @return An assignable rvalue
 *
 * @exceptsafe Does not throw exceptions.
 */
const StochasticRng& StochasticRng::operator=(const StochasticRng& other) {
	gsl_rng_memcpy(rng, other.rng);

	return *this;
}

/** Draws a standard uniform random variate.
 *
 * @return A number distributed uniformly over [0, 1), drawn independently 
 *	of any other calls to rUnif() or rNorm().
 *
 * @exceptsafe Does not throw exceptions.
 */
double StochasticRng::rUnif() const {
	return gsl_rng_uniform(rng);
}

/** Draws a standard normal random variate.
 *
 * @return A number distributed normally with mean 0 and variance 1, drawn 
 *	independently of any other calls to rUnif() or rNorm().
 *
 * @exceptsafe Does not throw exceptions.
 */
double StochasticRng::rNorm() const {
	return gsl_ran_ugaussian(rng);
}

}}	// end lcmc::models
