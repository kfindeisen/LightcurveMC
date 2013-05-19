# Compilation make for lightcurveMC test driver
# by Krzysztof Findeisen
# Created April 28, 2013
# Last modified May 18, 2013

include ../makefile.inc

#---------------------------------------
# Directory contents
PROJ     := test

SOURCES  := testdriver.cpp test_common.cpp \
	unit_stats.cpp unit_dmdt.cpp unit_gp.cpp unit_paramlist.cpp unit_waves.cpp

OBJS     := $(SOURCES:.cpp=.o)

#---------------------------------------
# Primary build option
.PHONY: depend $(PROJ)

# Unit test code cannot be properly compiled as a library, so leave the objects as standalone files
$(PROJ): $(OBJS)

depend: 
	@echo "Updating list of test code files..."
	@echo "tests/test: $(OBJS:%=tests/%)" > ../test.d

include ../makefile.common
