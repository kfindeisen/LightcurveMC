# Common makefile definitions
# by Krzysztof Findeisen
# Created March 24, 2010
# Last modified June 24, 2013

SHELL := /bin/sh

#---------------------------------------
# Directories
INCDIRS  := 
LIBDIRS  := 

#---------------------------------------
# Compiler settings
CXX       := g++

LANGTYPE  := -std=c++98 -pedantic-errors
WARNINGS  := -Wall -Wextra -Weffc++ -Wdeprecated -Wold-style-cast -Wsign-promo -fdiagnostics-show-option
OPTFLAGS  := -O3 -DNDEBUG
CXXFLAGS  := $(LANGTYPE) $(WARNINGS) $(OPTFLAGS) -Werror -D BOOST_TEST_DYN_LINK
LDFLAGS   := 

#---------------------------------------
# Intermediate builds
AR	:= ar
ARFLAGS	:= rusv

#---------------------------------------
# Other variables
RM      := rm -f

#---------------------------------------
# Headers and libraries for R 

RCPPFLAGS   := $(shell R CMD config --cppflags)
RLDFLAGS    := $(shell R CMD config --ldflags)
RBLAS       := $(shell R CMD config BLAS_LIBS)
RLAPACK     := $(shell R CMD config LAPACK_LIBS)

#---------------------------------------
# Headers and libraries for Rcpp interface classes

RCPPINCL    := $(shell echo 'Rcpp:::CxxFlags()' | R --vanilla --slave)
RCPPLIBS    := $(shell echo 'Rcpp:::LdFlags()'  | R --vanilla --slave)

#---------------------------------------
# Headers and libraries for RInside embedding classes

RINSIDEINCL := $(shell echo 'RInside:::CxxFlags()' | R --vanilla --slave)
RINSIDELIBS := $(shell echo 'RInside:::LdFlags()'  | R --vanilla --slave)

#---------------------------------------
# Append to existing code

RFLAGS      := $(RCPPFLAGS) $(RCPPINCL) $(RINSIDEINCL)
RLIBS       := $(RINSIDELIBS) $(RCPPLIBS) $(RLDFLAGS) $(RBLAS) $(RLAPACK)

CXXFLAGS    := $(RFLAGS) $(CXXFLAGS)
