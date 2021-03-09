
SUBDIR := $(sort $(wildcard */))
SUBDIR := $(filter-out  xputty/, $(SUBDIR))
SUBDIR := $(filter-out  docs/, $(SUBDIR))
SUBDIR := $(filter-out  libxputty/, $(SUBDIR))

GOALS := install uninstall

ifneq ($(filter $(GOALS),$(MAKECMDGOALS)),)
	SUBDIR := $(filter-out  Build/, $(SUBDIR))
endif

.PHONY: $(SUBDIR) recurse

$(MAKECMDGOALS) recurse: $(SUBDIR)

$(SUBDIR):
	@exec $(MAKE) -C $@ $(MAKECMDGOALS)

doc:
	#@rm -rf ./docs
	#doxygen Doxyfile
	#cp ./examples/index.html ./docs/
