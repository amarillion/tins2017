#
# invoke "make BUILD=DEBUG" or "make BUILD=RELEASE"
# 
BUILD=DEBUG
#BUILD=RELEASE

TWIST_HOME=../twist5
CXX = g++
LD = g++
CFLAGS = -std=c++11 -Iinclude -I$(TWIST_HOME)/include -W -Wall -Wno-unused -DUSE_MOUSE -DUSE_CURL
LFLAGS =
LIBS= -lcurl
NAME=peppy

# Adjust build directory name based on platform
ifdef WINDOWS
	PLATFORMSUF = _win
else
	PLATFORMSUF = 	
endif

ifeq ($(BUILD),RELEASE)
	CFLAGS += -O3
	LFLAGS += -s
	BUILDDIR = build/release$(PLATFORMSUF)
endif
ifeq ($(BUILD),DEBUG)
	CFLAGS += -g -DDEBUG
	BUILDDIR = build/debug$(PLATFORMSUF)
endif
ifeq ($(BUILD),STATIC)
	CFLAGS += -O3
	LFLAGS += -s
	BUILDDIR = build/static$(PLATFORMSUF)
endif

OBJDIR=$(BUILDDIR)/obj

ifdef WINDOWS
	CFLAGS += -D__GTHREAD_HIDE_WIN32API
	LFLAGS += -Wl,--subsystem,windows
	ifeq ($(BUILD),RELEASE)
		LIBS += -lallegro_monolith
	endif
	ifeq ($(BUILD),DEBUG)
		LIBS += -lallegro_monolith-debug
	endif
	BINSUF = .exe
	ICONOBJ = $(OBJDIR)/icon.o	
else
	ALLEGRO_MODULES=allegro allegro_primitives allegro_font allegro_main allegro_dialog allegro_image allegro_audio allegro_acodec allegro_ttf allegro_color
	ifeq ($(BUILD),RELEASE)
		ALLEGRO_LIBS = $(addsuffix -5, $(ALLEGRO_MODULES))
		LIBS += `pkg-config --libs $(ALLEGRO_LIBS)`
	endif
	ifeq ($(BUILD),DEBUG)
		ALLEGRO_LIBS = $(addsuffix -debug-5, $(ALLEGRO_MODULES))
		LIBS += `pkg-config --libs $(ALLEGRO_LIBS)`
	endif
	ifeq ($(BUILD),STATIC)
		LFLAGS += '-Wl,-rpath,$$ORIGIN/../lib',--enable-new-dtags
		# This will only statically link allegro but not its dependencies.
		# See https://www.allegro.cc/forums/thread/616656
		ALLEGRO_LIBS = $(addsuffix -static-5, $(ALLEGRO_MODULES))
		LIBS += `pkg-config --libs --static $(ALLEGRO_LIBS)`
	endif
	BINSUF =
endif

BIN = $(BUILDDIR)/$(NAME)$(BINSUF)

$(shell mkdir -p $(OBJDIR) >/dev/null)

vpath %.cpp $(TWIST_HOME)/src:src:test

SRC = $(wildcard src/*.cpp) $(wildcard $(TWIST_HOME)/src/*.cpp)
OBJ = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRC)))

.PHONY: all
all: $(BIN)

$(BIN) : $(OBJ) $(ICONOBJ)
	$(LD) -o $(BIN) $^ $(LIBS) $(LFLAGS)
	@echo
	@echo "Build complete. Run $(BIN)"

# automatic dependency generation,
# as recommended by http://make.mad-scientist.net/papers/advanced-auto-dependency-generation/
DEPFLAGS = -MT $@ -MMD -MP -MF $(OBJDIR)/$*.Td

# Use temporary dep file & rename to avoid messing up deps when compilation fails.
$(OBJDIR)/%.o : %.cpp $(OBJDIR)/%.d
	$(CXX) $(CFLAGS) $(DEPFLAGS) -o $@ -c $<
	@mv -f $(OBJDIR)/$*.Td $(OBJDIR)/$*.d

$(ICONOBJ) : icon.rc icon.ico
	windres -I rc -O coff -i icon.rc -o $(ICONOBJ)

$(OBJDIR)/%.d: ;
.PRECIOUS: $(OBJDIR)/%.d

-include $(OBJDIR)/*.d

CFLAGS_TEST = $(CFLAGS) `cppunit-config --cflags`
LDFLAGS_TEST = `cppunit-config --libs`
TESTBIN = $(BUILDDIR)/test_runner

SRC_TEST = $(wildcard test/*.cpp)
OBJ_TEST = $(patsubst %.cpp, $(OBJDIR)/%.o, $(notdir $(SRC_TEST)))

$(TESTBIN): $(OBJ_TEST) $(OBJDIR)/molbi.o $(OBJDIR)/data.o
	$(CXX) -o $(TESTBIN) $^ $(LIBS) $(LDFLAGS_TEST) 

.PHONY: test
test: $(TESTBIN)
	echo Running test suite ...
	./$(TESTBIN)

.PHONY: clean
clean:
	-$(RM) $(OBJDIR)/*.o $(OBJDIR)/*.d $(TESTBIN)

.PHONY:distclean
distclean: clean
	-$(RM) $(BIN)

