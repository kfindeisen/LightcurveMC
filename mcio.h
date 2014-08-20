/** Input and output routines for Lightcurve MC
 * @file lightcurveMC/mcio.h
 * @author Krzysztof Findeisen
 * @date Created February 4, 2011
 * @date Last modified June 18, 2013
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

#ifndef LMCIOH
#define LMCIOH

#include <string>
#include <vector>
#include <cstdio>

/** Shorthand for a vector of doubles
 */
typedef std::vector<double> DoubleVec;


/** Reads a file containing timestamps into a vector of dates
 */	
void readTimeStamps(FILE* hInput, DoubleVec &dates, double &minDelT, double &maxDelT);

/** Reads a file containing timestamps into a vector of dates
 */	
void readTimeStamps(FILE* hInput, DoubleVec &dates);

/** Dumps the contents of a lightcurve to a file
 */
void printLightCurve(const std::string& fileName, 
		const DoubleVec& timeGrid, const DoubleVec& fluxGrid);

#endif		// end ifndef LMCIOH
