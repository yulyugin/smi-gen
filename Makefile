KVERSION	= $(shell uname -r)
KMAKE		= $(MAKE) -C /lib/modules/$(KVERSION)/build

MKDIR		= mkdir -p
LN			= ln -sf
RM			= rm -rf

DRIVER_DIR=build-smigen-$(KVERSION)

all: smigen-driver

prepare-driver-dir:
	$(MKDIR) $(DRIVER_DIR)
	$(LN) ../smigen/module.c $(DRIVER_DIR)
	$(LN) ../smigen/GNUMakefile $(DRIVER_DIR)/Makefile
	for x in smigen/*.h ; do $(LN) ../$$x $(DRIVER_DIR) ; done

smigen-driver: prepare-driver-dir
	$(KMAKE) M="$$PWD/$(DRIVER_DIR)"

clean-driver:
	$(RM) $(DRIVER_DIR)

clean: clean-driver

