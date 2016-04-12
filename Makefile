KVERSION    = $(shell uname -r)
KMAKE       = $(MAKE) -C /lib/modules/$(KVERSION)/build

CC          = gcc
CFLAGS      = -Wall -Werror

MKDIR       = mkdir -p
LN          = ln -sf
RM          = rm -rf

ifeq ($(VERBOSE), yes)
QUIET=
else
QUIET=@
endif

DRIVER_DIR=build-smigen-$(KVERSION)

all: smigen-driver smigen-exe

prepare-driver-dir:
	$(QUIET)$(MKDIR) $(DRIVER_DIR)
	$(QUIET)$(LN) ../smigen/module.c $(DRIVER_DIR)
	$(QUIET)$(LN) ../smigen/GNUMakefile $(DRIVER_DIR)/Makefile
	$(QUIET)for x in smigen/*.h ; do $(LN) ../$$x $(DRIVER_DIR) ; done

smigen-driver: prepare-driver-dir
	$(QUIET)$(KMAKE) M="$$PWD/$(DRIVER_DIR)"

smigen-exe: smigen.c
	$(QUIET)$(CC) $(CFLAGS) smigen.c -Ismigen -o smigen.exe

clean-driver:
	$(QUIET)$(RM) "$(DRIVER_DIR)"

clean-exe:
	$(QUIET)$(RM) smigen.exe

clean-windows:
	$(QUIET)$(RM) build-win7-fre smigen/buildfre_win7_* smigen/objfre_win7_*

clean: clean-driver clean-exe clean-windows

