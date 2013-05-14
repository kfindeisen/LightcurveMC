# Compilation make for lightcurveMC test driver
# by Krzysztof Findeisen
# Created April 28, 2013
# Last modified May 3, 2013

SHELL = /bin/sh

include ../makefile.inc

#---------------------------------------
# Directory contents
PROJ     = test

SOURCES  = testdriver.cpp test_common.cpp \
	unit_stats.cpp unit_dmdt.cpp unit_gp.cpp unit_paramlist.cpp unit_waves.cpp

OBJS     = $(SOURCES:.cpp=.o)
LIBS     = timescales gsl gslcblas boost_unit_test_framework 

#---------------------------------------
# Primary build option
.PHONY: $(PROJ)
$(PROJ): $(OBJS)
	@echo "Updating list of test code files..."
	@echo "tests/test: $(OBJS:%=tests/%)" > ../test.d

include ../makefile.common
