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
	XDG_DIR := $(LIB_DIR)xdgmime/
	OBJ_DIR := .
	RELEASE_DIR := ../libxputty/

	# include path for library headers
	INCLUDES := -I. -I$(HEADER_DIR) -I$(WIDGET_HEADER_DIR) -I$(RESOURCES_DIR) -I$(DIALOG_HEADER_DIR) -I$(XDG_DIR)
	# link flags to dynamic link cairo and X11 (default)
	LDFLAGS += -fPIC `pkg-config --cflags --libs cairo x11` -lm
	# set link flags to static link cairo and X11
	# (you could ignore ld warnings, but the resulting binary is huge
	# (6.4Mb for x11 and cairo) NOT RECOMMENDED!!
	STATIC_LDFLAGS += -fPIC -static `pkg-config --static --cflags --libs cairo ` \
	 -lX11 -ldl -lxcb -lpthread -lXau -lXdmcp
	# debug flags, use this to indicate a problem with libxputty
	DEBUG_LDFLAGS += -g -D DEBUG  -fPIC\
	 `pkg-config --static --cflags --libs cairo` \
	 -L/usr/X11/lib -lX11 -lm


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

all:  $(RESOURCEHEADER) libxputty.a

debug: LDFLAGS = $(DEBUG_LDFLAGS)
debug: all

nls: LDFLAGS += -DENABLE_NLS
nls: all

updatepot:

po:

clean:
	rm -f libxputty.a libxputty.so libxputty.so.1 libxputty.so.1.0 xputty.pc $(RESOURCES_C) $(SVGRESOURCES_C) \
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
	sed 's;PATH;$(PREFIX);' $< >$@

libxputty.a: $(RESOURCES_OBJ) $(SVGRESOURCES_OBJ) $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(XDG_OBJ)
	$(AR) rcs libxputty.a $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ)  $(SVGRESOURCES_OBJ) $(XDG_OBJ)
	mkdir -p $(RELEASE_DIR)include/
	cp $(HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(WIDGET_HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(DIALOG_HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(RESOURCEHEADER) $(RELEASE_DIR)include/
	cp $(XDG_DIR)*.h $(RELEASE_DIR)include/
	cp -r $(LIB_DIR)lv2_plugin $(RELEASE_DIR)
	cp libxputty.a $(RELEASE_DIR)

libxputty.so: xputty.pc libxputty.a
	$(CC) -shared -Wl,-soname,libxputty.so.1 -o libxputty.so.1.0 $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ) $(XDG_OBJ)
	cp libxputty.so.1.0 $(RELEASE_DIR)
	ln -sf libxputty.so.1.0 libxputty.so.1
	ln -sf libxputty.so.1 libxputty.so
	cp -Pf libxputty.so.1 $(RELEASE_DIR)
	cp -Pf libxputty.so $(RELEASE_DIR)
	cp xputty.pc $(RELEASE_DIR)

shared: libxputty.so

doc:

mod:

install: shared
ifneq ("$(wildcard $(RELEASE_DIR))","")
	mkdir -p $(DESTDIR)$(INCLUDE_DIR)xputty/
	mkdir -p $(DESTDIR)$(INSTALL_DIR)
	mkdir -p $(DESTDIR)$(SHARE_DIR)pkgconfig/
	cp $(RELEASE_DIR)include/*.h $(DESTDIR)$(INCLUDE_DIR)xputty/
	cp $(RELEASE_DIR)*.so.1.0 $(DESTDIR)$(INSTALL_DIR)
	cp -Pf $(RELEASE_DIR)*.so.1 $(DESTDIR)$(INSTALL_DIR)
	cp -Pf $(RELEASE_DIR)*.so $(DESTDIR)$(INSTALL_DIR)
	cp $(RELEASE_DIR)libxputty.a $(DESTDIR)$(INSTALL_DIR)
	cp $(RELEASE_DIR)xputty.pc $(DESTDIR)$(SHARE_DIR)pkgconfig/
else
	@echo "you must run 'make' first"
endif

uninstall:
	@rm -rf $(DESTDIR)$(INSTALL_DIR)libxputty.so $(DESTDIR)$(INSTALL_DIR)libxputty.so.1 \
	$(DESTDIR)$(INSTALL_DIR)libxputty.so.1.0 $(DESTDIR)$(INSTALL_DIR)libxputty.a
	@rm -rf $(DESTDIR)$(INCLUDE_DIR)xputty/
	@rm -rf $(DESTDIR)$(SHARE_DIR)pkgconfig/xputty.pc
