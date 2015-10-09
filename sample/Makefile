#
#  TOPPERS/ASP Kernel
#      Toyohashi Open Platform for Embedded Real-Time Systems/
#      Advanced Standard Profile Kernel
# 
#  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
#                              Toyohashi Univ. of Technology, JAPAN
#  Copyright (C) 2006-2011 by Embedded and Real-Time Systems Laboratory
#              Graduate School of Information Science, Nagoya Univ., JAPAN
# 
#
#	The above copyright holders grant permission gratis to use,
#	duplicate, modify, or redistribute (hereafter called use) this
#	software (including the one made by modifying this software),
#	provided that the following four conditions (1) through (4) are
#	satisfied.
#
#	(1) When this software is used in the form of source code, the above
#    	copyright notice, this use conditions, and the disclaimer shown
#    	below must be retained in the source code without modification.
#
#	(2) When this software is redistributed in the forms usable for the
#    	development of other software, such as in library form, the above
#    	copyright notice, this use conditions, and the disclaimer shown
#    	below must be shown without modification in the document provided
#    	with the redistributed software, such as the user manual.
#
#	(3) When this software is redistributed in the forms unusable for the
#    	development of other software, such as the case when the software
#    	is embedded in a piece of equipment, either of the following two
#   	 conditions must be satisfied:
#
#  	(a) The above copyright notice, this use conditions, and the
#         	disclaimer shown below must be shown without modification in
#     		the document provided with the redistributed software, such as
#      	the user manual.
#
# 		(b) How the software is to be redistributed must be reported to the
#     		TOPPERS Project according to the procedure described
#     		separately.
#
#	(4) The above copyright holders and the TOPPERS Project are exempt
#    	from responsibility for any type of damage directly or indirectly
#   	caused from the use of this software and are indemnified by any
#    	users or end users of this software from any and all causes of
#    	action whatsoever.
#
#	THIS SOFTWARE IS PROVIDED "AS IS." THE ABOVE COPYRIGHT HOLDERS AND
#	THE TOPPERS PROJECT DISCLAIM ANY EXPRESS OR IMPLIED WARRANTIES,
#	INCLUDING, BUT NOT LIMITED TO, ITS APPLICABILITY TO A PARTICULAR
#	PURPOSE. IN NO EVENT SHALL THE ABOVE COPYRIGHT HOLDERS AND THE
#	TOPPERS PROJECT BE LIABLE FOR ANY TYPE OF DAMAGE DIRECTLY OR
#	INDIRECTLY CAUSED FROM THE USE OF THIS SOFTWARE.
# 
#  @(#) $Id: Makefile 2048 2011-04-03 02:36:03Z ertl-hiro $
# 

#
#  target (suppress the setting in Makefile.target)
#
all:

#
#  target abbreviation
#
TARGET = @(TARGET)

#
#  the definition of program language
#
SRCLANG = @(SRCLANG)
ifeq ($(SRCLANG),c++)
  USE_CXX = true
  CXXLIBS = -lstdc++ -lm -lc
  CXXRTS = cxxrt.o newlibrt.o
endif

#
#  the definition of source file directory
#
SRCDIR = @(SRCDIR)

#
#  the file name extension of object file
#
OBJEXT = @(OBJEXT)

#
#  the definition of debug environment (maybe defined in
#  target-dependent part) 
#
DBGENV := @(DBGENV)

#
#  the definition of kernel library directory (if kernel library is the
#  target of make, this definition is null )
#
KERNEL_LIB = @(KERNEL_LIB)

#
#  whether the kernel is compiled in the unit of function
#
KERNEL_FUNCOBJS = @(KERNEL_FUNCOBJS)

#
#  whether trace log function is enabled
#
ENABLE_TRACE = @(ENABLE_TRACE)

#
#  the definition of utility tools' name
#
PERL = @(PERL)
CFG = @(CFG)

#
#  the definition of object file's name
#
OBJNAME = asp
ifdef OBJEXT
  OBJFILE = $(OBJNAME).$(OBJEXT)
  CFG1_OUT = cfg1_out.$(OBJEXT)
else
  OBJFILE = $(OBJNAME)
  CFG1_OUT = cfg1_out
endif

#
#  the definition of target-dependent directory
#
TARGETDIR = $(SRCDIR)/target/$(TARGET)

#
#  include the target-dependent part of makefile
#
include $(TARGETDIR)/Makefile.target

#
#  the definition of configurator options
#
CFG_TABS := --api-table $(SRCDIR)/kernel/kernel_api.csv \
			--cfg1-def-table $(SRCDIR)/kernel/kernel_def.csv $(CFG_TABS)
CFG_OBJS := kernel_cfg.o $(CFG_OBJS)
CFG2_OUT := kernel_cfg.c kernel_cfg.h $(CFG2_OUT)

#
#  the definition of common compile options
#
COPTS := $(COPTS) -g
ifndef OMIT_WARNING_ALL
  COPTS := $(COPTS) -Wall
endif
ifndef OMIT_OPTIMIZATION
  COPTS := $(COPTS) -O2
endif
CDEFS := $(CDEFS)
INCLUDES := -I. -I$(SRCDIR)/include -I$(SRCDIR)/arch -I$(SRCDIR) $(INCLUDES)
LDFLAGS := $(LDFLAGS)
LIBS := $(LIBS) $(CXXLIBS)
CFLAGS = $(COPTS) $(CDEFS) $(INCLUDES)

#
#  the definitions about application program
#
APPLNAME = @(APPLNAME)
APPLDIR = @(APPLDIR)
APPL_CFG = $(APPLNAME).cfg

APPL_DIR = $(APPLDIR) $(SRCDIR)/library
APPL_ASMOBJS =
ifdef USE_CXX
  APPL_CXXOBJS = $(APPLNAME).o @(APPLOBJS)
  APPL_COBJS =
else
  APPL_COBJS = $(APPLNAME).o @(APPLOBJS)
endif
APPL_CFLAGS =
APPL_LIBS =
ifdef APPLDIR
  INCLUDES := $(INCLUDES) $(foreach dir,$(APPLDIR),-I$(dir))
endif

#
#  the definitions about system service
#
SYSSVC_DIR := $(SYSSVC_DIR) $(SRCDIR)/syssvc $(SRCDIR)/library
SYSSVC_ASMOBJS := $(SYSSVC_ASMOBJS)
SYSSVC_COBJS := $(SYSSVC_COBJS) banner.o syslog.o serial.o logtask.o \
				log_output.o vasyslog.o t_perror.o strerror.o \
				$(CXXRTS)
SYSSVC_CFLAGS := $(SYSSVC_CFLAGS)
SYSSVC_LIBS := $(SYSSVC_LIBS)
INCLUDES := $(INCLUDES)

#
#  the definitions about kernel
#
#  KERNEL_ASMOBJS: the assemble language kernel object files¡¥
#  KERNEL_COBJS: the C language kernel object files which have related C
#  				 language source files
#  KERNEL_LCSRCS: the C language kernel source files which can
# 				  generate multiple object files
#  KERNEL_LCOBJS: the kernel object files generated by KERNEL_LCSRCS
#  KERNEL_AUX_COBJS: the C language object files which are not included
#  					in the load module but are complied with the same
#  					options of kernel object files
#
KERNEL_DIR := $(KERNEL_DIR) $(SRCDIR)/kernel
KERNEL_ASMOBJS := $(KERNEL_ASMOBJS)
KERNEL_COBJS := $(KERNEL_COBJS)
KERNEL_CFLAGS := $(KERNEL_CFLAGS) -I$(SRCDIR)/kernel
ifdef OMIT_MAKEOFFSET
  OFFSET_H =
else
  OFFSET_H = offset.h
ifndef OFFSET_TF
  KERNEL_AUX_COBJS := $(KERNEL_AUX_COBJS) makeoffset.o
endif
endif

#
#  target file (only one can be selected)
#
all: $(OBJFILE)
#all: $(OBJNAME).bin
#all: $(OBJNAME).srec

##### don't modify the following contents #####

#
#  the compile options depending on debug environment
#
ifdef DBGENV
  CDEFS := $(CDEFS) -D$(DBGENV)
endif

#
#  the definitions about kernel library
#
ifdef KERNEL_LIB
  LIBKERNEL = $(KERNEL_LIB)/libkernel.a
else
  LIBKERNEL = libkernel.a
endif

#
#  the definition of kernel file structure
#
include $(SRCDIR)/kernel/Makefile.kernel
ifdef KERNEL_FUNCOBJS
  KERNEL_LCSRCS := $(KERNEL_FCSRCS)
  KERNEL_LCOBJS := $(foreach file,$(KERNEL_FCSRCS),$($(file:.c=)))
else
  KERNEL_CFLAGS := -DALLFUNC $(KERNEL_CFLAGS)
  KERNEL_COBJS := $(KERNEL_COBJS) \
					$(foreach file,$(KERNEL_FCSRCS),$(file:.c=.o))
endif

#
#  the definition of source file directory
#
vpath %.c $(KERNEL_DIR) $(SYSSVC_DIR) $(APPL_DIR)
vpath %.S $(KERNEL_DIR) $(SYSSVC_DIR) $(APPL_DIR)
vpath %.cfg $(APPL_DIR)

#
#  the definitions of compile variables
#
KERNEL_LIB_OBJS = $(KERNEL_ASMOBJS) $(KERNEL_COBJS) $(KERNEL_LCOBJS)
SYSSVC_OBJS = $(SYSSVC_ASMOBJS) $(SYSSVC_COBJS)
APPL_OBJS = $(APPL_ASMOBJS) $(APPL_COBJS) $(APPL_CXXOBJS)
ALL_OBJS = $(START_OBJS) $(APPL_OBJS) $(SYSSVC_OBJS) $(CFG_OBJS) \
											$(END_OBJS) $(HIDDEN_OBJS)
ALL_LIBS = $(APPL_LIBS) $(SYSSVC_LIBS) $(LIBKERNEL) $(LIBS)

ifdef TEXT_START_ADDRESS
  LDFLAGS := $(LDFLAGS) -Wl,-Ttext,$(TEXT_START_ADDRESS)
endif
ifdef DATA_START_ADDRESS
  LDFLAGS := $(LDFLAGS) -Wl,-Tdata,$(DATA_START_ADDRESS)
endif
ifdef LDSCRIPT
  LDFLAGS := $(LDFLAGS) -T $(LDSCRIPT)
endif

#
#  rule to generate offset.h
#
ifdef OFFSET_TF
offset.h: kernel_cfg.timestamp
	$(CFG) --pass 3 --kernel asp $(INCLUDES) \
				--rom-image cfg1_out.srec --symbol-table cfg1_out.syms \
				-T $(OFFSET_TF) $(CFG_TABS) $(APPL_CFG)
else
offset.h: makeoffset.s $(SRCDIR)/utils/genoffset
	$(PERL) $(SRCDIR)/utils/genoffset makeoffset.s > offset.h
endif

#
#  rule to generate kernel configuration file
#
cfg1_out.c: $(APPL_CFG)
	$(CFG) --pass 1 --kernel asp $(INCLUDES) $(CFG_TABS) $<

kernel_cfg.timestamp: $(START_OBJS) cfg1_out.o $(END_OBJS) $(HIDDEN_OBJS)
kernel_cfg.timestamp $(CFG2_OUT): $(APPL_CFG)
	$(LINK) $(CFLAGS) $(LDFLAGS) $(CFG1_OUT_LDFLAGS) -o $(CFG1_OUT) \
						$(START_OBJS) cfg1_out.o $(END_OBJS)
	$(NM) -C $(CFG1_OUT) > cfg1_out.syms
	$(OBJCOPY) -O srec -S $(CFG1_OUT) cfg1_out.srec
	$(CFG) --pass 2 --kernel asp $(INCLUDES) \
				-T $(TARGETDIR)/target.tf $(CFG_TABS) $<
	touch kernel_cfg.timestamp

#
#  rule to generate kernel library
#
libkernel.a: $(OFFSET_H) $(KERNEL_LIB_OBJS)
	rm -f libkernel.a
	$(AR) -rcs libkernel.a $(KERNEL_LIB_OBJS)
	$(RANLIB) libkernel.a

#
#  the definition of dependency
#
banner.o: kernel_cfg.timestamp $(filter-out banner.o,$(ALL_OBJS)) \
											$(filter %.a,$(ALL_LIBS))

#
#  rule to link
#
$(OBJFILE): $(APPL_CFG) kernel_cfg.timestamp $(ALL_OBJS) \
											$(filter %.a,$(ALL_LIBS))
	$(LINK) $(CFLAGS) $(LDFLAGS) -o $(OBJFILE) $(START_OBJS) \
			$(APPL_OBJS) $(SYSSVC_OBJS) $(CFG_OBJS) $(ALL_LIBS) $(END_OBJS)
	$(NM) -C $(OBJFILE) > $(OBJNAME).syms
	$(OBJCOPY) -O srec -S $(OBJFILE) $(OBJNAME).srec
	$(CFG) --pass 3 --kernel asp $(INCLUDES) \
				--rom-image $(OBJNAME).srec --symbol-table $(OBJNAME).syms \
				-T $(TARGETDIR)/target_check.tf $(CFG_TABS) $<

#
#  rule to generate binary file
#
$(OBJNAME).bin: $(OBJFILE)
	$(OBJCOPY) -O binary -S $(OBJFILE) $(OBJNAME).bin

#
#  rule to generate S-record file
#
$(OBJNAME).srec: $(OBJFILE)
	$(OBJCOPY) -O srec -S $(OBJFILE) $(OBJNAME).srec

#
#  rule to clean
#
.PHONY: clean
clean:
	rm -f \#* *~ *.o $(CLEAN_FILES)
	rm -f $(OBJFILE) $(OBJNAME).syms $(OBJNAME).srec $(OBJNAME).bin
	rm -f kernel_cfg.timestamp $(CFG2_OUT)
	rm -f cfg1_out.c $(CFG1_OUT) cfg1_out.syms cfg1_out.srec
ifndef KERNEL_LIB
	rm -f $(LIBKERNEL)
endif
	rm -f makeoffset.s offset.h

.PHONY: cleankernel
cleankernel:
	rm -rf $(KERNEL_LIB_OBJS)
	rm -f makeoffset.s offset.h

.PHONY: cleandep
cleandep:
	if ! [ -f Makefile.depend ]; then \
		rm -f kernel_cfg.timestamp $(CFG2_OUT); \
		rm -f cfg1_out.c cfg1_out.o $(CFG1_OUT) cfg1_out.syms cfg1_out.srec; \
		rm -f makeoffset.s offset.h; \
	fi
	rm -f Makefile.depend

.PHONY: realclean
realclean: cleandep clean
	rm -f $(REALCLEAN_FILES)

#
#  the definitions of rules of compiling and generating dependency for
#  kernel_cfg.c and cfg1_out.c
#
#  kernel_cfg.c and cfg1_out.c are complied with the options for
#  application program, system service and kernel
#
KERNEL_CFG_CFLAGS = $(APPL_CFLAGS) $(SYSSVC_CFLAGS) $(KERNEL_CFLAGS)

kernel_cfg.o cfg1_out.o: %.o: %.c
	$(CC) -c $(CFLAGS) $(KERNEL_CFG_CFLAGS) $<

kernel_cfg.s cfg1_out.s: %.s: %.c
	$(CC) -S $(CFLAGS) $(KERNEL_CFG_CFLAGS) $<

kernel_cfg.d cfg1_out.d: %.d: %.c
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(KERNEL_CFG_CFLAGS)" $< >> Makefile.depend

#
#  rule to generate special dependency
#
cfg1_out.depend: $(APPL_CFG)
	@$(CFG) -M cfg1_out.c $(INCLUDES) $< >> Makefile.depend

makeoffset.d: makeoffset.c
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-T "makeoffset.s" -O "$(CFLAGS) $(KERNEL_CFLAGS)" $< >> Makefile.depend

#
#  rule to generate dependency
#
.PHONY: gendepend
gendepend:
	@echo "Generating Makefile.depend."

.PHONY: depend
ifdef KERNEL_LIB
depend: cleandep kernel_cfg.timestamp gendepend \
		cfg1_out.depend cfg1_out.d \
		$(ALL_OBJS:.o=.d)
else
depend: cleandep $(OFFSET_H) kernel_cfg.timestamp gendepend \
		cfg1_out.depend cfg1_out.d \
		$(KERNEL_AUX_COBJS:.o=.d) $(KERNEL_ASMOBJS:.o=.d) \
		$(KERNEL_COBJS:.o=.d) $(KERNEL_LCSRCS:.c=.d) $(ALL_OBJS:.o=.d)
endif

#
#  include dependency file
#
-include Makefile.depend

#
#  the definition of tool-chain command's name
#
ifeq ($(TOOL),gcc)
  #
  #  GNU environment
  #
  ifdef GCC_TARGET
    GCC_TARGET_PREFIX = $(GCC_TARGET)-
  else
    GCC_TARGET_PREFIX =
  endif
  CC = $(GCC_TARGET_PREFIX)gcc
  CXX = $(GCC_TARGET_PREFIX)g++
  AS = $(GCC_TARGET_PREFIX)as
  LD = $(GCC_TARGET_PREFIX)ld
  AR = $(GCC_TARGET_PREFIX)ar
  NM = $(GCC_TARGET_PREFIX)nm
  RANLIB = $(GCC_TARGET_PREFIX)ranlib
  OBJCOPY = $(GCC_TARGET_PREFIX)objcopy
  OBJDUMP = $(GCC_TARGET_PREFIX)objdump
endif

ifdef USE_CXX
  LINK = $(CXX)
else
  LINK = $(CC)
endif

#
#  the definitions of compile rules
#
KERNEL_ALL_COBJS = $(KERNEL_COBJS) $(KERNEL_AUX_COBJS)

$(KERNEL_ALL_COBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_ALL_COBJS:.o=.s): %.s: %.c
	$(CC) -S $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_LCOBJS): %.o:
	$(CC) -DTOPPERS_$(*F) -o $@ -c $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_LCOBJS:.o=.s): %.s:
	$(CC) -DTOPPERS_$(*F) -o $@ -S $(CFLAGS) $(KERNEL_CFLAGS) $<

$(KERNEL_ASMOBJS): %.o: %.S
	$(CC) -c $(CFLAGS) $(KERNEL_CFLAGS) $<

$(SYSSVC_COBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(SYSSVC_CFLAGS) $<

$(SYSSVC_COBJS:.o=.s): %.s: %.c
	$(CC) -S $(CFLAGS) $(SYSSVC_CFLAGS) $<

$(SYSSVC_ASMOBJS): %.o: %.S
	$(CC) -c $(CFLAGS) $(SYSSVC_CFLAGS) $<

$(APPL_COBJS): %.o: %.c
	$(CC) -c $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_COBJS:.o=.s): %.s: %.c
	$(CC) -S $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_CXXOBJS): %.o: %.cpp
	$(CXX) -c $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_CXXOBJS:.o=.s): %.s: %.cpp
	$(CXX) -S $(CFLAGS) $(APPL_CFLAGS) $<

$(APPL_ASMOBJS): %.o: %.S
	$(CC) -c $(CFLAGS) $(APPL_CFLAGS) $<

#
#  the definitions of rules to generate dependency
#
$(KERNEL_COBJS:.o=.d): %.d: %.c
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(KERNEL_CFLAGS)" $< >> Makefile.depend

$(KERNEL_LCSRCS:.c=.d): %.d: %.c
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) -T "$($*)" \
		-O "-DALLFUNC $(CFLAGS) $(KERNEL_CFLAGS)" $< >> Makefile.depend

$(KERNEL_ASMOBJS:.o=.d): %.d: %.S
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(KERNEL_CFLAGS)" $< >> Makefile.depend

$(SYSSVC_COBJS:.o=.d): %.d: %.c
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(SYSSVC_CFLAGS)" $< >> Makefile.depend

$(SYSSVC_ASMOBJS:.o=.d): %.d: %.S
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(SYSSVC_CFLAGS)" $< >> Makefile.depend

$(APPL_COBJS:.o=.d): %.d: %.c
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(APPL_CFLAGS)" $< >> Makefile.depend

$(APPL_CXXOBJS:.o=.d): %.d: %.cpp
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CXX) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(APPL_CFLAGS)" $< >> Makefile.depend

$(APPL_ASMOBJS:.o=.d): %.d: %.S
	@$(PERL) $(SRCDIR)/utils/makedep -C $(CC) $(MAKEDEP_OPTS) \
		-O "$(CFLAGS) $(APPL_CFLAGS)" $< >> Makefile.depend

#
#  default compile rules
#
%.o: %.c
	@echo "*** Default compile rules should not be used."
	$(CC) -c $(CFLAGS) $<

%.s: %.c
	@echo "*** Default compile rules should not be used."
	$(CC) -S $(CFLAGS) $<

%.o: %.cpp
	@echo "*** Default compile rules should not be used."
	$(CXX) -c $(CFLAGS) $<

%.s: %.cpp
	@echo "*** Default compile rules should not be used."
	$(CXX) -S $(CFLAGS) $<

%.o: %.S
	@echo "*** Default compile rules should not be used."
	$(CC) -c $(CFLAGS) $<
