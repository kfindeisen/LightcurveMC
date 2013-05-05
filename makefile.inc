# Common makefile definitions
# by Krzysztof Findeisen
# Created March 24, 2010
# Last modified May 5, 2013

SHELL = /bin/sh

#---------------------------------------
# Directories
INCDIRS  = 
LIBDIRS  = 

#---------------------------------------
# Compiler settings
CXX       = g++

# ISO C++ disallows floating-point constants?? Really???
#LANGTYPE  = -std=c++98 -pedantic-errors -D _USE_MATH_DEFINES 
LANGTYPE  = -std=c++98 -D _USE_MATH_DEFINES 
WARNINGS  = -Wall -Wextra -Weffc++ -Wdeprecated -Wold-style-cast -Wsign-promo -fdiagnostics-show-option
OPTFLAGS  = -O3 -DNDEBUG
CXXFLAGS  = $(LANGTYPE) $(WARNINGS) $(OPTFLAGS) -Werror -D BOOST_TEST_DYN_LINK
LDFLAGS   = 

#---------------------------------------
# Intermediate builds
AR	= ar
ARFLAGS	= rusv

#---------------------------------------
# Other variables
RM      = rm -f
