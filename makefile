# Compilation make for lightcurveMC
# by Krzysztof Findeisen
# Created March 24, 2010
# Last modified April 28, 2013

SHELL = /bin/sh

include makefile.inc

#---------------------------------------
# Select all files
PROJ     = lightcurveMC
SOURCES  = driver.cpp cmd.cpp cmd_classes.cpp binstats.cpp nanstats.cpp \
	lcsupport.cpp lightcurve.cpp paramlist.cpp lcregistry.cpp fluxmag.cpp \
	uncopyable.cpp \
	mcio.cpp $(INFORMALINC)/kpffileio.cpp ../lightCurveSpecs/lcsio.cpp
OBJS     = $(SOURCES:.cpp=.o)
DIRS     = samples stats waves
LIBS     = timescales gsl gslcblas
TESTDIRS = $(DIRS) tests
TESTLIBS = $(LIBS) boost_unit_test_framework 

#---------------------------------------
# Primary build option
$(PROJ): $(OBJS) $(DIRS)
	@echo "Linking $@ with $(addprefix -l,$(LIBS))"
	@$(CXX) $(CXXFLAGS) $(LIBFLAGS) -o $@ $(OBJS) $(DIRS:%=-l%) $(LIBS:%=-l%) $(LIBDIRS:%=-L%) -L.

#---------------------------------------
# Subdirectories
# Can't declare the directories phony directly, or the program will be built every time
.PHONY: cd
samples: cd
	cd samples; make $(MFLAGS)

stats: cd
	cd stats; make $(MFLAGS)

tests: cd
	cd tests; make $(MFLAGS)

waves: cd
	cd waves; make $(MFLAGS)

include makefile.common

#---------------------------------------
# Debug mode
#debug: clean
#	CXXFLAGS = $(CXXFLAGS) -D _DEBUG
#	make debug2
#debug2: $(OBJS)
#	$(CXX) $(CXXFLAGS) $(LIBFLAGS) -o $(PROJ) $^ $(addprefix -l,$(LIBS)) $(addprefix -L ,$(LIBDIRS))

#---------------------------------------
# Doxygen
# No phony needed, since we are literally building a directory
doc: $(SOURCES) $(TESTSOURCES) doxygen.cfg
	$(RM) -r doc/*
	doxygen doxygen.cfg

#---------------------------------------
# Test cases
.PHONY: unittest
unittest: tests/test
tests/test: $(OBJS) $(TESTDIRS)
	@echo "Linking $@ with $(addprefix -l,$(TESTLIBS))"
	@$(CXX) $(CXXFLAGS) $(LIBFLAGS) -o $@ $(OBJS) $(TESTDIRS:%=-l%) $(TESTLIBS:%=-l%) $(LIBDIRS:%=-L%) -L.

.PHONY: autotest
autotest: $(PROJ) unittest
	cd tests; source autotest.sh

#---------------------------------------
# Doxygen
.PHONY: all
all: $(PROJ) unittest doc
