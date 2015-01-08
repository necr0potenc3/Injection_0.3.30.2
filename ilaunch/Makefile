
# Makefile for Mingw32 v1.0

ifndef SRCDIR
SRCDIR=.
endif

INJECTION_DIR=$(SRCDIR)/..

EXPAT_DIR=/cygdrive/g/expat
RC_COMMAND=rc
RC_INC="g:\\mingw\\include"

TEST_DIR=/cygdrive/g/injection

VERSION=0.1.2a
CXXFLAGS=-mwindows -mthreads -Wall -W -Werror \
	-DVERSION=$(VERSION) -I$(INJECTION_DIR) -DCINTERFACE \
	-I$(EXPAT_DIR)/include
LDFLAGS=-mwindows -mthreads -L$(EXPAT_DIR)/lib
ifdef RELEASE
CXXFLAGS+=-O2
# strip debugging info
LDFLAGS+=-s
else
CXXFLAGS+=-g
endif

CXXCOMPILE=g++ $(CXXFLAGS)
RCFILE=$(SRCDIR)/ilaunch.rc
OBJS=ilaunch.o patch.o crc.o gui.o \
	ilconfig.o resource.o
DEP_FILES=.deps/ilaunch.P .deps/patch.P .deps/crc.P .deps/gui.P \
	.deps/ilconfig.P
EXEC=ilaunch.exe
LIBS=-lexpat -lole32 -lshell32

$(EXEC) : $(OBJS)
	g++ $(LDFLAGS) -o $@ $(OBJS) $(LIBS)

test : $(EXEC)
	cp $(EXEC) $(TEST_DIR)

clean:
	rm -f $(EXEC) $(OBJS) $(DEP_FILES) *.res RC* *~ *.~* tmp* errs*

resource.o : $(RCFILE) $(SRCDIR)/resource.h
	export INCLUDE=$(RC_INC) ; $(RC_COMMAND) /foilaunch.res $(RCFILE)
	windres ilaunch.res $@
#	windres $(RCFILE) $@

%.o: $(SRCDIR)/%.cpp
	@echo '$(CXXCOMPILE) -c $<'; \
	$(CXXCOMPILE) -MD -c $<
	@-cp $(*F).d .deps/$(*F).P; \
	tr ' ' '\012' < $(*F).d \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm $(*F).d

%.o: $(INJECTION_DIR)/%.cpp
	@echo '$(CXXCOMPILE) -c $<'; \
	$(CXXCOMPILE) -MD -c $<
	@-cp $(*F).d .deps/$(*F).P; \
	tr ' ' '\012' < $(*F).d \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm $(*F).d

-include $(DEP_FILES)

