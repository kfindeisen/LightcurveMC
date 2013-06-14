# Compilation make for Lightcurve MC
# by Krzysztof Findeisen
# Created March 24, 2010
# Last modified May 26, 2013

include makefile.inc

#---------------------------------------
# Select all files
PROJ     := lightcurveMC
SOURCES  := binstats.cpp sims.cpp approxequal.cpp fluxmag.cpp \
	nanstats.cpp mcio.cpp \
	lcsupport.cpp lightcurve.cpp paramlist.cpp lcregistry.cpp \
	statsupport.cpp \
	cerror_except.cpp lcsio.cpp utils_except.cpp
OBJS     := $(SOURCES:.cpp=.o)
# except must be last because other libraries depend on it
DIRS     := cmd samples stats waves except 
LIBS     := timescales gsl gslcblas
TESTLIBS := $(LIBS) boost_unit_test_framework-mt 

#---------------------------------------
# Primary build option
$(PROJ): driver.o $(OBJS) $(DIRS)
	@echo "Linking $@ with $(LIBS:%=-l%)"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^) $(DIRS:%=-l%) $(LIBS:%=-l%) $(LIBDIRS:%=-L %) -L .

#---------------------------------------
# Subdirectories
# Can't declare the directories phony directly, or the executable will be built every time
.PHONY: cd
cmd: cd
	@make -C cmd --no-print-directory $(MFLAGS)

except: cd
	@make -C except --no-print-directory $(MFLAGS)

samples: cd
	@make -C samples --no-print-directory $(MFLAGS)

stats: cd
	@make -C stats --no-print-directory $(MFLAGS)

waves: cd
	@make -C waves --no-print-directory $(MFLAGS)

tests: cd
	@make -C tests --no-print-directory $(MFLAGS)

include makefile.common

#---------------------------------------
# Doxygen
.PHONY: doc
doc: doc/
doc/: driver.cpp $(SOURCES) $(DIRS) tests doxygen.cfg statistics.bib
	doxygen doxygen.cfg
	cd doc/latex && make

#---------------------------------------
# Test cases
.PHONY: unittest
unittest: tests/test
tests/test: $(OBJS) $(DIRS) tests
	@echo "Linking $@ with $(TESTLIBS:%=-l%)"
	@$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $(filter %.o,$^) $(DIRS:%=-l%) $(TESTLIBS:%=-l%) $(LIBDIRS:%=-L %) -L .

.PHONY: autotest
autotest: $(PROJ) unittest
	@echo "Beginning regression test suite..."
	@echo "Tests started on `date`"
	@cd tests && ./autotest.sh ; echo "Tests completed on `date`"

# Unit test code cannot be properly compiled as a library, so depend on the objects directly
test.d: tests/makefile
	@make depend -C tests --no-print-directory $(MFLAGS)
# Cancel the implicit rule to keep make from being hung up over the absence of tests/makefile.o
# It's a useless rule for large projects, anyway
% : %.o

include driver.d
include test.d

#---------------------------------------
# Build program, test suite, and documentation
.PHONY: all
all: $(PROJ) unittest doc
