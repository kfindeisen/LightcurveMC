# Compilation make for lightcurveMC
# by Krzysztof Findeisen
# Created March 24, 2010
# Last modified May 4, 2013

SHELL = /bin/sh

include makefile.inc

#---------------------------------------
# Select all files
PROJ     = lightcurveMC
SOURCES  = cmd.cpp cmd_classes.cpp binstats.cpp approxequal.cpp nanstats.cpp \
	lcsupport.cpp lightcurve.cpp paramlist.cpp lcregistry.cpp fluxmag.cpp \
	uncopyable.cpp \
	mcio.cpp lcsio.cpp
OBJS     = $(SOURCES:.cpp=.o)
DIRS     = samples stats waves
LIBS     = timescales gsl gslcblas
TESTLIBS = $(LIBS) boost_unit_test_framework 

#---------------------------------------
# Primary build option
$(PROJ): driver.o $(OBJS) $(DIRS)
	@echo "Linking $@ with $(LIBS:%=-l%)"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^) $(DIRS:%=-l%) $(LIBS:%=-l%) $(LIBDIRS:%=-L %) -L .

#---------------------------------------
# Subdirectories
# Can't declare the directories phony directly, or the executable will be built every time
.PHONY: cd
samples: cd
	@make -C samples --no-print-directory $(MFLAGS)

stats: cd
	@make -C stats --no-print-directory $(MFLAGS)

tests/%.o: tests
tests: cd
	@make -C tests --no-print-directory $(MFLAGS)

waves: cd
	@make -C waves --no-print-directory $(MFLAGS)

include makefile.common

#---------------------------------------
# Doxygen
.PHONY: doc
doc: doc/
doc/: driver.cpp $(SOURCES) $(DIRS) tests doxygen.cfg
	$(RM) -r doc/*
	doxygen doxygen.cfg

#---------------------------------------
# Test cases
.PHONY: unittest
unittest: tests/test
tests/test: $(OBJS) $(DIRS)
	@echo "Linking $@ with $(TESTLIBS:%=-l%)"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^) $(DIRS:%=-l%) $(TESTLIBS:%=-l%) $(LIBDIRS:%=-L %) -L .

.PHONY: autotest
autotest: $(PROJ) unittest
	@echo "Beginning regression test suite..."
	@echo "Tests started on `date`"
	@cd tests; ./autotest.sh; echo "Tests completed on `date`"

include driver.d
include test.d
test.d: | tests

#---------------------------------------
# Build program, test suite, and documentation
.PHONY: all
all: $(PROJ) unittest doc
