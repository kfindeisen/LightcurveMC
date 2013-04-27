# Compilation make for lightcurveMC
# by Krzysztof Findeisen
# Created March 24, 2010
# Last modified April 27, 2013

#---------------------------------------
# Platform-specific stuff
include make.platform

#---------------------------------------
# Select all files
PROJ     = lightcurveMC
COMMONSOURCES  = cmd.cpp cmd_classes.cpp binstats.cpp nanstats.cpp \
	lcsupport.cpp lightcurve.cpp paramlist.cpp lcregistry.cpp fluxmag.cpp waves/multinormal.cpp \
	waves/lcdeterministic.cpp waves/lcstochastic.cpp \
	waves/lcflat.cpp waves/lcperiodic.cpp waves/lcsine.cpp waves/lcellipse.cpp \
	waves/lctriangle.cpp waves/lcmagsine.cpp waves/lceclipse.cpp waves/lcbroad.cpp \
	waves/lcsharp.cpp waves/lcflarepeak.cpp waves/lcslowpeak.cpp waves/lcsquarepeak.cpp \
	waves/lcflaredip.cpp waves/lcslowdip.cpp waves/lcsquaredip.cpp waves/lcwhite.cpp \
	waves/lcdrw.cpp waves/lcgp1.cpp \
	samples/observations.cpp samples/datafactory.cpp samples/obs_nonspitzernonvar.cpp \
	samples/obs_spitzernonvar.cpp samples/obs_nonspitzervar.cpp samples/obs_spitzervar.cpp \
	stats/dmdt.cpp stats/magdist.cpp stats/experimental.cpp \
	uncopyable.cpp \
	mcio.cpp $(INFORMALINC)/kpffileio.cpp ../lightCurveSpecs/lcsio.cpp
SOURCES = driver.cpp $(COMMONSOURCES)
TESTSOURCES = tests/testdriver.cpp tests/unit_stats.cpp tests/unit_dmdt.cpp tests/unit_gp.cpp $(COMMONSOURCES)
OBJS     = $(SOURCES:.cpp=.o)
#OBJS     = $(OBJS,F)
TESTOBJS = $(TESTSOURCES:.cpp=.o)
LIBS     = timescales gsl gslcblas

#---------------------------------------
# Compiler settings
CC       = g++
LANGTYPE  = -std=c++98 -D _USE_MATH_DEFINES 
WARNINGS  = -Wall -Wextra -Weffc++ -Wdeprecated -Wold-style-cast -Wsign-promo -fdiagnostics-show-option
OPTFLAGS = -O3
CFLAGS     = $(LANGTYPE) $(WARNINGS)  $(OPTFLAGS) -Werror
#CFLAGS    = $(LANGTYPE) -D _DEBUG -D USELFP
#TCLAPFLAGS = $(LANGTYPE) -D _DEBUG -D USELFP

#---------------------------------------
# Primary build option
$(PROJ): $(OBJS)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(addprefix -l,$(LIBS)) $(addprefix -L ,$(LIBDIRS))

#---------------------------------------
# Debug mode
debug: clean
	CFLAGS = $(CFLAGS) -D _DEBUG
	make debug2
debug2: $(OBJS)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $(PROJ) $^ $(addprefix -l,$(LIBS)) $(addprefix -L ,$(LIBDIRS))

#---------------------------------------
# Actual compilation options

# make sure the flags are used when compiling
%.o: %.cpp
	$(CC) -c $(CFLAGS) -o $@ $(patsubst %.o, %.cpp, $@) $(addprefix -I ,$(INCDIRS))

#---------------------------------------
# Set up dependencies
# Bug: currently can't properly handle source files in subdirectories
%.d: %.cpp
	$(CC) $(CFLAGS) -MM $^ -MF $@.$$$$; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm $@.$$$$

include $(SOURCES:.cpp=.d)
include $(TESTSOURCES:.cpp=.d)

#---------------------------------------
# Cleanup code
.PHONY: clean cleanall cleandepend
clean:
	-rm -vf *.o samples/*.o stats/*.o tests/*.o waves/*.o core *.stackdump
cleandepend: 
	-rm -vf *.d samples/*.d stats/*.d tests/*.d waves/*.d
cleanall: clean cleandepend

.PHONY: rebuild
rebuild: clean $(PROJ)

#---------------------------------------
# Test cases
.PHONY: unittest
unittest: tests/test
tests/test: $(TESTOBJS)
	$(CC) $(CFLAGS) $(LIBFLAGS) -o $@ $^ $(addprefix -l,$(LIBS)) -lboost_unit_test_framework $(addprefix -L ,$(LIBDIRS))

.PHONY: inttest
inttest:
	pushd tests
	source autotest.sh
	popd
