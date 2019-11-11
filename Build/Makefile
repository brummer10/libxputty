
	# set  name
	NAME = Xputty
	VER = 0.1

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
	LDFLAGS += -fPIC `pkg-config --static --cflags --libs cairo x11` -lm
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
	RESOURCE_EXTLD := $(notdir $(patsubst %.png,%_png,$(RESOURCES)))
	RESOURCEHEADER := $(RESOURCES_DIR)xresources.h

	XDG_SOURCES :=  $(wildcard $(XDG_DIR)*.c)
	XDG_OBJ := $(notdir $(patsubst %.c,%.o,$(XDG_SOURCES)))

	DEP = $(LIB_OBJ:%.o=%.d) $(WIDGET_OBJ:%.o=%.d) $(DIALOG_OBJ:%.o=%.d) $(XDG_OBJ:%.o=%.d)

.PHONY : all clean

.NOTPARALLEL:

all:  $(RESOURCEHEADER) libxputty.a

clean:
	rm -f libxputty.a $(LIB_OBJ) $(WIDGET_OBJ) $(RESOURCES_OBJ) $(DIALOG_OBJ) $(XDG_OBJ) $(DEP) $(RESOURCEHEADER)
	rm -rf $(RELEASE_DIR)

$(RESOURCEHEADER): $(RESOURCES_OBJ) 
	rm -f $(RESOURCEHEADER)
	for f in $(RESOURCE_EXTLD); do \
		echo 'EXTLD('$${f}')' >> $(RESOURCEHEADER) ; \
	done

$(XDG_OBJ): $(XDG_SOURCES)
	$(CC) -MMD -Wall -DHAVE_MMAP -c $(addprefix $(XDG_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(RESOURCES_OBJ): $(RESOURCES)
	cd $(RESOURCES_DIR) && ld -r -b binary $(patsubst %.o,%.png,$@) -o ../../Build/$@

$(DIALOG_OBJ): $(DIALOG_SOURCES)
	$(CC) -MMD -Wall -c $(addprefix $(DIALOG_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(WIDGET_OBJ): $(WIDGET_SOURCES)
	$(CC) -MMD -Wall -c $(addprefix $(WIDGET_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

$(LIB_OBJ): $(LIB_SOURCES)
	$(CC) -MMD -Wall -c $(addprefix $(LIB_DIR),$(patsubst %.o,%.c,$@)) -o $@ $(INCLUDES) $(LDFLAGS)

-include $(DEP)

libxputty.a:  $(RESOURCES_OBJ) $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(XDG_OBJ)
	ar rcs libxputty.a $(LIB_OBJ) $(WIDGET_OBJ) $(DIALOG_OBJ) $(RESOURCES_OBJ) $(XDG_OBJ)
	mkdir -p $(RELEASE_DIR)include/
	cp $(HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(WIDGET_HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(DIALOG_HEADER_DIR)*.h $(RELEASE_DIR)include/
	cp $(RESOURCEHEADER) $(RELEASE_DIR)include/
	cp $(XDG_DIR)*.h $(RELEASE_DIR)include/
	cp libxputty.a $(RELEASE_DIR)

doc:

install:

uninstall:

