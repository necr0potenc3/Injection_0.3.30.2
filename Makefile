# Makefile for Mingw32 v1.0

MINGW_INC=/cygdrive/g/mingw/include
MINGW_LIB=/cygdrive/g/mingw/lib
MINGW_BIN=/cygdrive/g/mingw/bin

RC_COMMAND=rc
RC_INC="g:\\mingw\\include"

TEST_DIR=/cygdrive/g/injection

ifndef SRCDIR
SRCDIR=.
endif

EXPAT_DIR=/cygdrive/g/expat
EXPAT_VC_DIR=/cygdrive/g/expat-1.95.2-win32

VERSION=0.3.30.2
CXXFLAGS=-Wall -W -Werror -DVERSION=$(VERSION) -I$(EXPAT_DIR)/include
LDFLAGS=-mwindows -Wl,--image-base,0x6aaa0000 --shared -L$(EXPAT_DIR)/lib
# Testing:
LDFLAGS+= -Wl,--script,$(SRCDIR)/ldscript.x
ifdef RELEASE
CXXFLAGS+=-O2
# strip debugging info
LDFLAGS+=-s
else
CXXFLAGS+=-g
endif


CXXCOMPILE=g++ $(CXXFLAGS)
RCFILE=$(SRCDIR)/injection.rc
OBJS=common.o injection.o igui.o gui.o \
	iconfig.o world.o runebook.o hotkeys.o hotkeyhook.o\
	equipment.o vendor.o menus.o target.o spells.o skills.o hooks.o \
	ignition.o patch.o uo_huffman.o crypt.o resource.o extdll.o \
	generic_gump.o
DEP_FILES=.deps/common.P .deps/injection.P .deps/igui.P .deps/gui.P \
	.deps/iconfig.P .deps/world.P .deps/target.P .deps/spells.P \
	.deps/equipment.P .deps/vendor.P .deps/menus.P .deps/hooks.P \
	.deps/ignition.P .deps/patch.P .deps/uo_huffman.P .deps/crypt.P \
	.deps/runebook.P .deps/skills.P .deps/hotkeys.P .deps/hotkeyhook.P \
	.deps/generic_gump.P
EXEC=injection.dll
LIBS=-lcomctl32 -lwsock32 -lexpat

SRCDIST_FILES=*.h *.H *.C *.cpp *.rc *.cfg Makefile \
        ldscript.x \
	ilaunch/*.h ilaunch/*.cpp ilaunch/*.rc ilaunch/Makefile \
	ilaunch/*.ico \
	changelog.txt README.txt COMPILE.txt LICENSE.txt \
	release/Makefile release/ignition.cfg \
	ilaunch/release/Makefile ilaunch/release/ilpatch.cfg \
	injection.def Injection_vc.dsp \
	ilaunch/ilaunch.dsp \
	script/*.h script/*.dfm script/*.cpp script/script.bpr script/script.bpf \
	script/doc/*.doc script/doc/*.txt script/Y/bison.* script/Y/script.y
	
SRCDIST_ARC=injection-src-$(VERSION).zip

BINDIST_FILES=$(SRCDIR)/release/$(EXEC) $(EXPAT_DIR)/bin/libexpat.dll \
	$(MINGW_BIN)/mingwm10.dll \
	$(SRCDIR)/ilaunch/release/ilaunch.exe $(SRCDIR)/ilaunch/release/ilpatch.cfg \
	$(SRCDIR)/release/Ignition.cfg $(SRCDIR)/changelog.txt \
	$(SRCDIR)/README.txt $(SRCDIR)/LICENSE.txt
	
VC_BINDIST_FILES=$(SRCDIR)/release/$(EXEC) $(EXPAT_VC_DIR)/libs/expat.dll \
	$(SRCDIR)/ilaunch/release/ilaunch.exe $(SRCDIR)/ilaunch/release/ilpatch.cfg \
	$(SRCDIR)/release/Ignition.cfg $(SRCDIR)/changelog.txt \
	$(SRCDIR)/README.txt $(SRCDIR)/LICENSE.txt \
	$(SRCDIR)/script/script.dll \
	$(SRCDIR)/script/doc/scripting.txt 
	
BINDIST_ARC=injection-bin-$(VERSION).zip

$(EXEC) : $(OBJS)
	g++ $(LDFLAGS) -o $@ $(OBJS) $(LIBS)
#	dllwrap --driver-name=g++ -o $@ $(OBJS) $(LIBS) reloc.s

test : 
	cp $(SRCDIR)/script/script.dll $(TEST_DIR)
	cp $(SRCDIR)/ilaunch/Debug/ilaunch.exe $(TEST_DIR)
	cp $(SRCDIR)/Debug/injection.dll $(TEST_DIR)
	
srcdist:
	rm -f $(SRCDIST_ARC)
	zip $(SRCDIST_ARC) $(SRCDIST_FILES)

bindist: $(BINDIST_FILES)
	rm -f $(BINDIST_ARC)
	zip -j $(BINDIST_ARC) $(BINDIST_FILES)

vcbindist:
	rm -f $(BINDIST_ARC)
	zip -j $(BINDIST_ARC) $(VC_BINDIST_FILES)

updatecl:
	scp changelog.txt chiphead-net@injection.sourceforge.net:/home/groups/i/in/injection/htdocs

clean:
	rm -f $(EXEC) $(OBJS) $(DEP_FILES) *.res RC* *~ *.~* tmp* errs*

resource.o : $(RCFILE) $(SRCDIR)/resource.h
	export INCLUDE=$(RC_INC) ; $(RC_COMMAND) /foinjection.res $(RCFILE)
	windres injection.res $@
#	windres $(RCFILE) $@

%.o: $(SRCDIR)/%.cpp
	@echo '$(CXXCOMPILE) -c $<'; \
	$(CXXCOMPILE) -MD -c $<
	@-cp $(*F).d .deps/$(*F).P; \
	tr ' ' '\012' < $(*F).d \
	  | sed -e 's/^\\$$//' -e '/^$$/ d' -e '/:$$/ d' -e 's/$$/ :/' \
	    >> .deps/$(*F).P; \
	rm $(*F).d

-include $(DEP_FILES)

