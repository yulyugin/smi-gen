KVERSION    = $(shell uname -r)
KMAKE       = $(MAKE) -C /lib/modules/$(KVERSION)/build

CC          = gcc
CFLAGS      = -Wall -Werror

MKDIR       = mkdir -p
LN          = ln -sf
RM          = rm -rf

DRIVER_DIR=build-smigen-$(KVERSION)

all: smigen-driver smigen-exe

prepare-driver-dir:
	$(MKDIR) $(DRIVER_DIR)
	$(LN) ../smigen/module.c $(DRIVER_DIR)
	$(LN) ../smigen/GNUMakefile $(DRIVER_DIR)/Makefile
	for x in smigen/*.h ; do $(LN) ../$$x $(DRIVER_DIR) ; done

smigen-driver: prepare-driver-dir
	$(KMAKE) M="$$PWD/$(DRIVER_DIR)"

smigen-exe: smigen.c
	$(CC) $(CFLAGS) smigen.c -Ismigen -o smigen.exe

clean-driver:
	$(RM) $(DRIVER_DIR)

clean-exe:
	$(RM) smigen.exe

clean: clean-driver clean-exe

