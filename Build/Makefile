
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
	LDFLAGS += -fPIC `$(PKGCONFIG) $(PKGCONFIG_FLAGS) --cflags cairo $(XELF)` 
	# set link flags to static link cairo and X11
	# (you could ignore ld warnings, but the resulting binary is huge
	# (6.4Mb for x11 and cairo) NOT RECOMMENDED!!
	STATIC_LDFLAGS += -fPIC -static `pkg-config --static --cflags cairo ` \
	 -lX11 -ldl -lxcb -lpthread -lXau -lXdmcp
	# debug flags, use this to indicate a problem with libxputty
	DEBUG_LDFLAGS += -g -DDEBUG 


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
	CONFIG_H := $(shell cat config.h 2>/dev/null | grep 'USE_LD =' )
	CONFIG_X := $(shell cat config.h 2>/dev/null | grep 'undef' )

ifdef USE_LDD
	RESOURCES_O = $(RESOURCES_OBJ)
else
	RESOURCES_O = $(RESOURCES_C)
endif

.PHONY: all clean debug nls updatepot po shared

.NOTPARALLEL:

all: config $(RESOURCEHEADER) libxputty.$(STATIC_LIB_EXT)
	@$(B_ECHO) "=================== DONE =======================$(reset)"

debug: LDFLAGS += $(DEBUG_LDFLAGS)
debug: all

nls: LDFLAGS += -DENABLE_NLS
nls: all

updatepot:
	@:

po:
	@:

clean:
ifndef EXTRAQUIET
	$(QUIET)@$(ECHO) "Clean up $(reset)"
endif
	$(QUIET)rm -f libxputty.$(STATIC_LIB_EXT) libxputty.$(LIB_EXT) libxputty.$(LIB_EXT).1 \
	libxputty.$(LIB_EXT).1.0 libxputty.lib xputty.pc $(RESOURCES_C) $(SVGRESOURCES_C) \
	$(LIB_OBJ) $(WIDGET_OBJ) $(RESOURCES_OBJ) $(SVGRESOURCES_OBJ) $(DIALOG_OBJ) $(XDG_OBJ) $(DEP) $(RESOURCEHEADER)
	$(QUIET)rm -rf $(RELEASE_DIR)
	$(QUIET)rm -f config.h

config:
	@$(B_ECHO) "================================================$(reset)"
ifdef USE_LDD
ifeq (,$(findstring 1,$(CONFIG_H)))
	$(QUIET)@rm -f config.h
	$(QUIET)echo '#ifndef USE_LD' >> config.h ;
	$(QUIET)echo '#define USE_LD = 1' >> config.h ;
	$(QUIET)echo '#endif' >> config.h ;
endif
else ifdef USE_GOLD
ifeq (,$(findstring 1,$(CONFIG_H)))
	$(QUIET)@rm -f config.h
	$(QUIET)echo '#ifndef USE_LD' >> config.h ;
	$(QUIET)echo '#define USE_LD = 1' >> config.h ;
	$(QUIET)echo '#endif' >> config.h ;
endif
else
ifeq (,$(findstring undef,$(CONFIG_X)))
	$(QUIET)@rm -f config.h
	$(QUIET)echo '#ifdef USE_LD' >> config.h ;
	$(QUIET)echo '#undef USE_LD' >> config.h ;
	$(QUIET)echo '#endif' >> config.h ;
endif
endif

$(RESOURCEHEADER): $(RESOURCES_O)
	$(QUIET)rm -f $(RESOURCEHEADER)
	$(QUIET)for f in $(RESOURCE_EXTLD); do \
		echo 'EXTLD('$${f}')' >> $(RESOURCEHEADER) ; \
	done
	$(QUIET)for f in $(SVGRESOURCE_EXTLD); do \
		echo 'extern const char* '$${f}';' >> $(RESOURCEHEADER) ; \
	done

$(XDG_OBJ): $(XDG_SOURCES)
	@$(ECHO) "Building object file $@ $(reset)"
	$(QUIET)$(CC) $(CFLAGS) -MMD -Wall -DHAVE_MMAP -c $(addprefix $(XDG_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

ifdef XXDI
$(RESOURCES_O): $(RESOURCES)
	@$(ECHO) "Building object file for Resources $(reset)"
	$(QUIET)$(foreach f, $(RESOURCES_OBJ), \
		cd $(RESOURCES_DIR) ; \
		$(XXDI) $(patsubst %.o,%.png,$(f)) > ../../Build/$(patsubst %.o,%.c,$(f)) ; \
		cd ../../Build/ ; \
	)
	$(QUIET)$(foreach f, $(RESOURCES_C), \
		$(CC) -c $(f) -o $(patsubst %.c,%.o,$(f)); \
	)
else ifdef USE_LDD
LDFLAGS += -DUSE_LD=1
$(RESOURCES_O): $(RESOURCES)
	@$(ECHO) "Building Resource object $@$(reset)"
	$(QUIET)cd $(RESOURCES_DIR) && $(LD) -r -b binary -m $(LDEMULATION) $(patsubst %.o,%.png,$@) -o ../../Build/$@
else
LDFLAGS += -DUSE_LD=1
$(RESOURCES_O): $(RESOURCES)
	@$(ECHO) "Building Resource object $@$(reset)"
	$(QUIET)cd $(RESOURCES_DIR) && $(LD) -r -b binary -z noexecstack $(patsubst %.o,%.png,$@) -o ../../Build/$@
endif

$(SVGRESOURCES_C): $(SVGRESOURCES)
	@$(ECHO) "Building SVG Resource object $@ $(reset)"
	$(QUIET)@cd $(RESOURCES_DIR) && echo "const char* $(patsubst %.c,%_svg,$@) = \""| tr -d '\r\n' > ../../Build/$@
	$(QUIET)@cd $(RESOURCES_DIR) && base64  $(patsubst %.c,%.svg,$@) | tr -d '\r\n' >> ../../Build/$@
	$(QUIET)@cd $(RESOURCES_DIR) && echo "\";" >> ../../Build/$@
	$(QUIET)$(CC) $(CFLAGS) -c $@ -o $(patsubst %.c,%.o,$@)

$(DIALOG_OBJ): $(DIALOG_SOURCES)
	@$(ECHO) "Building object file $@ $(reset)"
	$(QUIET)$(CC) $(CFLAGS) -MMD -Wall -c $(addprefix $(DIALOG_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(WIDGET_OBJ): $(WIDGET_SOURCES)
	@$(ECHO) "Building object file $@ $(reset)"
	$(QUIET)$(CC) $(CFLAGS) -MMD -Wall -c $(addprefix $(WIDGET_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(LIB_OBJ): $(LIB_SOURCES)
	@$(ECHO) "Building object file $@ $(reset)"
	$(QUIET)$(CC) $(CFLAGS) -MMD -Wall -c $(addprefix $(LIB_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

-include $(DEP)

xputty.pc: xputty.pc.in
ifeq ($(TARGET), Linux)
	$(QUIET)sed 's;PATH;$(PREFIX);' $< >$@
endif

libxputty.$(STATIC_LIB_EXT): $(SVGRESOURCES_C) $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(XDG_OBJ)
	@$(B_ECHO) "Build static library $@ $(reset)"
	$(QUIET)$(AR) rcs libxputty.$(STATIC_LIB_EXT) $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ)  $(SVGRESOURCES_OBJ) $(XDG_OBJ)
	$(QUIET)mkdir -p $(RELEASE_DIR)include/
	$(QUIET)cp $(HEADER_DIR)*.h $(RELEASE_DIR)include/
	$(QUIET)cp $(WIDGET_HEADER_DIR)*.h $(RELEASE_DIR)include/
	$(QUIET)cp $(DIALOG_HEADER_DIR)*.h $(RELEASE_DIR)include/
	$(QUIET)cp $(RESOURCEHEADER) $(RELEASE_DIR)include/
	$(QUIET)cp config.h $(RELEASE_DIR)include/
ifeq ($(TARGET), Linux)
	$(QUIET)cp $(XDG_DIR)*.h $(RELEASE_DIR)include/
endif
	$(QUIET)cp -r $(LIB_DIR)lv2_plugin $(RELEASE_DIR)
	$(QUIET)cp libxputty.$(STATIC_LIB_EXT) $(RELEASE_DIR)

libxputty.$(LIB_EXT): xputty.pc libxputty.$(STATIC_LIB_EXT)
	@$(B_ECHO) "Compile shared library $@ $(reset)"
	$(QUIET)$(CC) -shared -Wl,-soname,libxputty.$(LIB_EXT).1 -o libxputty.$(LIB_EXT).1.0 $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ) $(XDG_OBJ)
	$(QUIET)cp libxputty.$(LIB_EXT).1.0 $(RELEASE_DIR)
	$(QUIET)ln -sf libxputty.$(LIB_EXT).1.0 libxputty.$(LIB_EXT).1
	$(QUIET)ln -sf libxputty.$(LIB_EXT).1 libxputty.$(LIB_EXT)
	$(QUIET)cp -Pf libxputty.$(LIB_EXT).1 $(RELEASE_DIR)
	$(QUIET)cp -Pf libxputty.$(LIB_EXT) $(RELEASE_DIR)
ifeq ($(TARGET), Linux)
	$(QUIET)cp xputty.pc $(RELEASE_DIR)
endif

shared: libxputty.$(LIB_EXT)
	@:

doc:
	@:

mod:
	@:

install: shared
ifneq ("$(wildcard $(RELEASE_DIR))","")
	@$(B_ECHO) "Install libxputty libraries $(reset)"
	$(QUIET)mkdir -p $(DESTDIR)$(INCLUDE_DIR)xputty/
	$(QUIET)mkdir -p $(DESTDIR)$(INSTALL_DIR)
	$(QUIET)mkdir -p $(DESTDIR)$(SHARE_DIR)pkgconfig/
	$(QUIET)cp $(RELEASE_DIR)include/*.h $(DESTDIR)$(INCLUDE_DIR)xputty/
	$(QUIET)cp $(RELEASE_DIR)*.$(LIB_EXT).1.0 $(DESTDIR)$(INSTALL_DIR)
	$(QUIET)cp -Pf $(RELEASE_DIR)*.$(LIB_EXT).1 $(DESTDIR)$(INSTALL_DIR)
	$(QUIET)cp -Pf $(RELEASE_DIR)*.$(LIB_EXT) $(DESTDIR)$(INSTALL_DIR)
	$(QUIET)cp $(RELEASE_DIR)libxputty.$(STATIC_LIB_EXT) $(DESTDIR)$(INSTALL_DIR)
	$(QUIET)cp $(RELEASE_DIR)xputty.pc $(DESTDIR)$(SHARE_DIR)pkgconfig/
	@$(B_ECHO) "=================== DONE =======================$(reset)"
else
	@$(R_ECHO) "you must run 'make' first$(reset)"
endif

uninstall:
	@rm -rf $(DESTDIR)$(INSTALL_DIR)libxputty.$(LIB_EXT) $(DESTDIR)$(INSTALL_DIR)libxputty.$(LIB_EXT).1 \
	$(DESTDIR)$(INSTALL_DIR)libxputty.$(LIB_EXT).1.0 $(DESTDIR)$(INSTALL_DIR)libxputty.$(STATIC_LIB_EXT)
	@rm -rf $(DESTDIR)$(INCLUDE_DIR)xputty/
	@rm -rf $(DESTDIR)$(SHARE_DIR)pkgconfig/xputty.pc
