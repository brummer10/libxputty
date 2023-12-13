# check if we need LD to implement images
# in order of preference: xxd -i, xxdi.pl, ld, ld.gold

# check for xxd (tool from vim)
ifeq ($(shell which xxd 2>&1 >/dev/null; echo $$?),0)
	ifneq ($(shell xxd --version 2>&1 | head -n 1 | grep xxd),)
		HAVE_XXD = 1
	endif
endif

# check for xxdi.pl, a simple alternative to vim's 'xxd -i' mode
# https://github.com/gregkh/xxdi
#
ifeq ($(shell which xxdi.pl 2>&1 >/dev/null; echo $$?),0)
	HAVE_XXDI_PL = 1
endif

ifneq ($(shell $(LD) --version 2>&1 | head -n 1 | grep ld),)
	ifneq ($(shell uname -a | grep  x86_64), )
		LDEMULATION := elf_x86_64
	else ifneq ($(shell uname -a | grep amd64), )
		LDEMULATION := elf_x86_64
	else ifneq ($(shell uname -a | grep i386), )
		LDEMULATION := elf_i386
	else ifeq ($(shell uname -m),aarch64)
		LDEMULATION := aarch64elf
	endif
	ifneq ($(LDEMULATION),)
		HAVE_KNOWN_LDEMULATION = 1
	endif
else ifneq ($(shell gold --version 2>&1 | head -n 1 | grep gold),)
	HAVE_LD_GOLD = 1
	LD = gold
endif

$(info HAVE_XXD="$(HAVE_XXD)")
$(info HAVE_XXDI_PL="$(HAVE_XXDI_PL)")
$(info HAVE_KNOWN_LDEMULATION="$(HAVE_KNOWN_LDEMULATION)")
$(info LDEMULATION="$(LDEMULATION)")
$(info HAVE_LD_GOLD="$(HAVE_LD_GOLD)")

ifeq ($(HAVE_XXD),1)
$(info Using xxd -i)
XXDI = xxd -i
else ifeq ($(HAVE_XXDI_PL),1)
$(info Using xxdi.pl)
XXDI = xxdi.pl
else ifeq ($(HAVE_KNOWN_LDEMULATION),1)
$(info Using $(LD) with LDEMULATION=$(LDEMULATION))
USE_LDD = 1
else ifeq ($(HAVE_LD_GOLD),1)
$(info Using $(LD) (gold))
else
$(error Neither xxd, nor xxdi.pl, nor suitable ld found)
endif

	# check OS specific stuff
	OS := $(shell echo $$OS)
	UNAME_S := $(shell uname -s)
	# default library extension
	# cross compilation (e.g.: PKG_CONFIG_PATH=/usr/local/pkgconfig make CROSS=x86_64-w64-mingw32- mod)
	ifneq (,$(findstring mingw,$(CROSS)))
		# Found
		TARGET = Windows
		STRIP = $(CROSS)strip
		PKGCONFIG = $(CROSS)pkg-config
		CC = $(CROSS)cc
		CXX = $(CROSS)g++
		LD = $(CROSS)ld
	else ifneq (,$(findstring mingw,$(CXX)))
		# Found
		TARGET = Windows
	else
		# Not found
		ifeq ($(UNAME_S), Linux) #LINUX
			TARGET = Linux
		endif
		ifeq ($(OS), Windows_NT) #WINDOWS
			TARGET = Windows
		endif
		ifeq ($(UNAME_S), Darwin) #APPLE
			TARGET = Apple
		endif
	endif
	ifeq ($(TARGET), Linux)
		ABI_CFLAGS = -Wl,-z,nodelete
		ABI_CXXFLAGS = -Wl,-z,relro,-z,now
		ABI_LDFLAGS = -Wl,-z,noexecstack
		STRIP = strip
		PKGCONFIG = pkg-config
		XELF = x11
		LIB_EXT = so
		STATIC_LIB_EXT = a
		XDG_DIR := ../xputty/xdgmime/
	endif
	ifeq ($(TARGET), Windows)
		ECHO += -e
		ABI_LDFLAGS = -static -lpthread
		GUI_LIBS = -liconv -lstdc++
		STRIP = x86_64-w64-mingw32-strip
		PKGCONFIG = x86_64-w64-mingw32-pkg-config
		PKGCONFIG_FLAGS = --static
		LIB_EXT = dll
		STATIC_LIB_EXT = lib
	endif

	# set  name
	NAME = Xputty
	VER = 1.0

	PREFIX ?= /usr
	INSTALL_DIR ?= $(PREFIX)/lib/
	INCLUDE_DIR ?= $(PREFIX)/include/
	SHARE_DIR ?= $(PREFIX)/share/

	LIB_DIR := ../xputty/
	HEADER_DIR := $(LIB_DIR)header/
	WIDGET_DIR := $(LIB_DIR)widgets/
	WIDGET_HEADER_DIR := $(HEADER_DIR)widgets/
	DIALOG_DIR := $(LIB_DIR)dialogs/
	DIALOG_HEADER_DIR := $(HEADER_DIR)dialogs/
	RESOURCES_DIR :=$(LIB_DIR)resources/
	OBJ_DIR := .
	RELEASE_DIR := ../libxputty/

	# include path for library headers
	INCLUDES := -I. -I$(HEADER_DIR) -I$(WIDGET_HEADER_DIR) -I$(RESOURCES_DIR) -I$(DIALOG_HEADER_DIR) -I$(XDG_DIR)
	# link flags to dynamic link cairo and X11 (default)
	LDFLAGS += -fPIC `$(PKGCONFIG) $(PKGCONFIG_FLAGS) --cflags --libs cairo $(XELF)` -lm
	# set link flags to static link cairo and X11
	# (you could ignore ld warnings, but the resulting binary is huge
	# (6.4Mb for x11 and cairo) NOT RECOMMENDED!!
	STATIC_LDFLAGS += -fPIC -static `pkg-config --static --cflags --libs cairo ` \
	 -lX11 -ldl -lxcb -lpthread -lXau -lXdmcp
	# debug flags, use this to indicate a problem with libxputty
	DEBUG_LDFLAGS += -g -DDEBUG -DLIBDEBUG


	LIB_SOURCES := $(wildcard $(LIB_DIR)*.c)
	LIB_OBJ := $(notdir $(patsubst %.c,%.o,$(LIB_SOURCES)))

	WIDGET_SOURCES := $(wildcard $(WIDGET_DIR)*.c)
	WIDGET_OBJ := $(notdir $(patsubst %.c,%.o,$(WIDGET_SOURCES)))

	DIALOG_SOURCES := $(wildcard $(DIALOG_DIR)*.c)
	DIALOG_OBJ := $(notdir $(patsubst %.c,%.o,$(DIALOG_SOURCES)))

	RESOURCES := $(wildcard $(RESOURCES_DIR)*.png)
	RESOURCES_OBJ := $(notdir $(patsubst %.png,%.o,$(RESOURCES)))
	RESOURCES_C := $(notdir $(patsubst %.png,%.c,$(RESOURCES)))
	RESOURCE_EXTLD := $(notdir $(patsubst %.png,%_png,$(RESOURCES)))
	RESOURCEHEADER := $(RESOURCES_DIR)xresources.h

	SVGRESOURCES := $(wildcard $(RESOURCES_DIR)*.svg)
	SVGRESOURCES_OBJ := $(notdir $(patsubst %.svg,%.o,$(SVGRESOURCES)))
	SVGRESOURCES_C := $(notdir $(patsubst %.svg,%.c,$(SVGRESOURCES)))
	SVGRESOURCE_EXTLD := $(notdir $(patsubst %.svg,%_svg,$(SVGRESOURCES)))

	XDG_SOURCES :=  $(wildcard $(XDG_DIR)*.c)
	XDG_OBJ := $(notdir $(patsubst %.c,%.o,$(XDG_SOURCES)))

	DEP = $(LIB_OBJ:%.o=%.d) $(WIDGET_OBJ:%.o=%.d) $(DIALOG_OBJ:%.o=%.d) $(XDG_OBJ:%.o=%.d)

.PHONY : all clean debug nls updatepot po shared

.NOTPARALLEL:

all:  $(RESOURCEHEADER) libxputty.$(STATIC_LIB_EXT)

debug: LDFLAGS += $(DEBUG_LDFLAGS)
debug: all

nls: LDFLAGS += -DENABLE_NLS
nls: all

updatepot:

po:

clean:
	rm -f libxputty.$(STATIC_LIB_EXT) libxputty.$(LIB_EXT) libxputty.$(LIB_EXT).1 libxputty.$(LIB_EXT).1.0 xputty.pc $(RESOURCES_C) $(SVGRESOURCES_C) \
	$(LIB_OBJ) $(WIDGET_OBJ) $(RESOURCES_OBJ) $(SVGRESOURCES_OBJ) $(DIALOG_OBJ) $(XDG_OBJ) $(DEP) $(RESOURCEHEADER)
	rm -rf $(RELEASE_DIR)

$(RESOURCEHEADER): $(RESOURCES_OBJ) 
	rm -f $(RESOURCEHEADER)
	for f in $(RESOURCE_EXTLD); do \
		echo 'EXTLD('$${f}')' >> $(RESOURCEHEADER) ; \
	done
	for f in $(SVGRESOURCE_EXTLD); do \
		echo 'extern const char* '$${f}';' >> $(RESOURCEHEADER) ; \
	done

$(XDG_OBJ): $(XDG_SOURCES)
	$(CC) $(CFLAGS) -MMD -Wall -DHAVE_MMAP -c $(addprefix $(XDG_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

ifdef XXDI
$(RESOURCES_OBJ): $(RESOURCES)
	cd $(RESOURCES_DIR) && $(XXDI) $(patsubst %.o,%.png,$@) > ../../Build/$(patsubst %.o,%.c,$@)
	$(CC) $(CFLAGS) -c $(patsubst %.o,%.c,$@) -o $@
else ifdef USE_LDD
LDFLAGS += -DUSE_LD=1
$(RESOURCES_OBJ): $(RESOURCES)
	cd $(RESOURCES_DIR) && $(LD) -r -b binary -m $(LDEMULATION) -z noexecstack $(patsubst %.o,%.png,$@) -o ../../Build/$@
else
LDFLAGS += -DUSE_LD=1
$(RESOURCES_OBJ): $(RESOURCES)
	cd $(RESOURCES_DIR) && $(LD) -r -b binary -z noexecstack $(patsubst %.o,%.png,$@) -o ../../Build/$@
endif

$(SVGRESOURCES_OBJ): $(SVGRESOURCES)
	@cd $(RESOURCES_DIR) && echo "const char* $(patsubst %.o,%_svg,$@) = \""| tr -d '\r\n' > ../../Build/$(patsubst %.o,%.c,$@)
	@cd $(RESOURCES_DIR) && base64  $(patsubst %.o,%.svg,$@) | tr -d '\r\n' >> ../../Build/$(patsubst %.o,%.c,$@)
	@cd $(RESOURCES_DIR) && echo "\";" >> ../../Build/$(patsubst %.o,%.c,$@)
	$(CC) $(CFLAGS) -c $(patsubst %.o,%.c,$@) -o $@

$(DIALOG_OBJ): $(DIALOG_SOURCES)
	$(CC) $(CFLAGS) -MMD -Wall -c $(addprefix $(DIALOG_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(WIDGET_OBJ): $(WIDGET_SOURCES)
	$(CC) $(CFLAGS) -MMD -Wall -c $(addprefix $(WIDGET_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(LIB_OBJ): $(LIB_SOURCES)
	$(CC) $(CFLAGS) -MMD -Wall -c $(addprefix $(LIB_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

-include $(DEP)

xputty.pc: xputty.pc.in

libxputty.$(STATIC_LIB_EXT): $(RESOURCES_OBJ) $(SVGRESOURCES_OBJ) $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(XDG_OBJ)
	$(AR) rcs libxputty.$(STATIC_LIB_EXT) $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ)  $(SVGRESOURCES_OBJ) $(XDG_OBJ)
	mkdir -p $(RELEASE_DIR)include/
	cp $(HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(WIDGET_HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(DIALOG_HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(RESOURCEHEADER) $(RELEASE_DIR)include/
ifeq ($(TARGET), Linux)
	cp $(XDG_DIR)*.h $(RELEASE_DIR)include/
endif
	cp -r $(LIB_DIR)lv2_plugin $(RELEASE_DIR)
	cp libxputty.$(STATIC_LIB_EXT) $(RELEASE_DIR)

libxputty.$(LIB_EXT): xputty.pc libxputty.$(STATIC_LIB_EXT)
	$(CC) -shared -Wl,-soname,libxputty.$(LIB_EXT).1 -o libxputty.$(LIB_EXT).1.0 $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ) $(XDG_OBJ)
	cp libxputty.$(LIB_EXT).1.0 $(RELEASE_DIR)
	ln -sf libxputty.$(LIB_EXT).1.0 libxputty.$(LIB_EXT).1
	ln -sf libxputty.$(LIB_EXT).1 libxputty.$(LIB_EXT)
	cp -Pf libxputty.$(LIB_EXT).1 $(RELEASE_DIR)
	cp -Pf libxputty.$(LIB_EXT) $(RELEASE_DIR)
	cp xputty.pc $(RELEASE_DIR)

shared: libxputty.$(LIB_EXT)
	sed 's;PATH;$(PREFIX);' $< >$@

doc:

mod:

install: shared
ifneq ("$(wildcard $(RELEASE_DIR))","")
	mkdir -p $(DESTDIR)$(INCLUDE_DIR)xputty/
	mkdir -p $(DESTDIR)$(INSTALL_DIR)
	mkdir -p $(DESTDIR)$(SHARE_DIR)pkgconfig/
	cp $(RELEASE_DIR)include/*.h $(DESTDIR)$(INCLUDE_DIR)xputty/
	cp $(RELEASE_DIR)*.$(LIB_EXT).1.0 $(DESTDIR)$(INSTALL_DIR)
	cp -Pf $(RELEASE_DIR)*.$(LIB_EXT).1 $(DESTDIR)$(INSTALL_DIR)
	cp -Pf $(RELEASE_DIR)*.$(LIB_EXT) $(DESTDIR)$(INSTALL_DIR)
	cp $(RELEASE_DIR)libxputty.$(STATIC_LIB_EXT) $(DESTDIR)$(INSTALL_DIR)
	cp $(RELEASE_DIR)xputty.pc $(DESTDIR)$(SHARE_DIR)pkgconfig/
else
	@echo "you must run 'make' first"
endif

uninstall:
	@rm -rf $(DESTDIR)$(INSTALL_DIR)libxputty.$(LIB_EXT) $(DESTDIR)$(INSTALL_DIR)libxputty.$(LIB_EXT).1 \
	$(DESTDIR)$(INSTALL_DIR)libxputty.$(LIB_EXT).1.0 $(DESTDIR)$(INSTALL_DIR)libxputty.$(STATIC_LIB_EXT)
	@rm -rf $(DESTDIR)$(INCLUDE_DIR)xputty/
	@rm -rf $(DESTDIR)$(SHARE_DIR)pkgconfig/xputty.pc
