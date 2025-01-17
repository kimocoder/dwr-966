#
# Prepare a Mercurial version controlled kernel tree repo
# Copyright 2013 LANTIQ DEUTSCHLAND GMBH
#

TOPDIR:=${CURDIR}
include rules.mk
include $(INCLUDE_DIR)/image.mk

PATCH_DIR ?= target/linux/$(BOARD)/patches$(if $(wildcard target/linux/$(BOARD)/patches-$(KERNEL_PATCHVER)),-$(KERNEL_PATCHVER))$(if $(PATCH_SUFFIX),-$(PATCH_SUFFIX))
FILES_DIR ?= $(foreach dir,$(wildcard target/linux/$(BOARD)/files target/linux/$(BOARD)/files-$(KERNEL_PATCHVER)),"$(dir)")
PATCH_DIR_p := $(if $(wildcard $(PATCH_DIR)),$(shell cd $(PATCH_DIR)/ && ( pwd -P; cd - >/dev/null ) ))

KERNEL_TREE:=$(TOPDIR)/kernel_tree
KERNEL_TREE_b:=$(shell basename $(KERNEL_TREE))
LINUX_NAME:=linux-$(LINUX_VERSION)
LINUX_TAR:=$(LINUX_NAME).tar.bz2

HG_IGNORE_FILES:=.hgignore target_patches
HG_IGNORE_FILES+=*.o *.d *.a *.s *.ko *.cmd *.builtin *.order *.mod.c .quilt* .config* .tmp_* *.cpio *.bin.gen.S
HG_IGNORE_FILES+=.tmp_versions .modules .prepared .vermagic .image .version .built
HG_IGNORE_FILES+=Module.symvers user_headers fixdep kallsyms conf mconf mk_elfconfig modpost sortextable unifdef System.map vmlinux.lds vmlinux tags
HG_IGNORE_FILES+=include/config include/generated
HG_IGNORE_FILES+=scripts/kconfig/zconf.hash.c scripts/kconfig/zconf.lex.c scripts/kconfig/zconf.tab.c scripts/mod/elfconfig.h scripts/mod/devicetable-offsets.h scripts/dtc
HG_IGNORE_FILES+=kernel/timeconst.h kernel/hz.bc lib/crc32table.h lib/gen_crc32table usr/gen_init_cpio

C_RS:=\033[00;00m
C_RE:=\033[31;01m
C_GR:=\033[32;01m
C_YE:=\033[33;01m
C_BL:=\033[34;01m
C_BR:=\033[31;02m

define succ
	echo -en "$(C_GR) Done.\n$(C_RS)";
endef

define warn
	echo -en "$(C_BL)$(1)$(C_RS)\n"
endef

define err
	echo -en "$(C_RE) Error!\n$(C_RS)"; \
	$(if $(1),$(call warn,$(1));) \
	exit 1;
endef

define check_uncheckin
	$(if $(wildcard $(KERNEL_TREE)),cd $(KERNEL_TREE) && ( hg status | grep -q $$ && \
		$(call warn,\nUncommitted files found in '$(KERNEL_TREE_b)'!! Provide 'hg status' under kernel tree for more details.); cd - >/dev/null ))
endef

define check_patch_uncheckin
	$(if $(wildcard $(KERNEL_TREE)),\
		cd $(KERNEL_TREE)/target_patches && ( hg status | grep -q $$ && ( \
		$(call warn,\nFollowing uncommitted patches found in '$(KERNEL_TREE_b)/target_patches/'!! Please verify.);\
		echo "--------";hg status;echo "--------"; )||echo -n; ) \
	)
endef

define setup_target_files_links
	echo -en "Creating target files links.."; \
	$(foreach fdir,$(FILES_DIR),\
		for fls in `cd $(TOPDIR)/$(fdir)/ && find . -follow -type f`; do \
			flname=`basename $$fls`; drname=`dirname $$fls`; \
			lnpath=""; lnpath=`cd $(TOPDIR)/$(fdir)/$$drname/ && pwd -P`; \
			mkdir -p $(KERNEL_TREE)/$$drname; \
			if [ -n "$$lnpath" ]; then \
				mkdir -p $(KERNEL_TREE)/$$drname; \
				ln -s $$lnpath/$$flname $(KERNEL_TREE)/$$drname/$$flname; \
			fi; \
		done; \
	)$(call succ)
endef

ifeq ($(OPTS),-h)
all:
	@echo -en "Usage:-\n\tno option, prepares a new kernel tree and assign it in .config\n"
	@echo -en "\t-o  disables kernel tree in .config\n\t-e  enables kernel tree in .config\n"
	@echo -en "\t-u  update target files links\n"
	@echo -en "\t-d  deletes kernel tree from TOPDIR and disable in .config\n\t-v  verify uncommitted files and patches\n"

 else
  ifeq ($(OPTS),-v)
all:
	@$(call check_uncheckin)
	@$(call check_patch_uncheckin)

  else
   ifeq ($(OPTS),-o)
all:
	@$(call check_uncheckin)
	@$(call check_patch_uncheckin)
	@echo -en "Disabling external kernel tree in .config .."
	@sed -i '/.*CONFIG_EXTERNAL_KERNEL_TREE.*/d' $(TOPDIR)/.config || ( $(call err) )
	@if [ -L $(LINUX_DIR) ]; then rm -f $(LINUX_DIR)/.prepared; fi
	@$(call succ)

   else
    ifeq ($(OPTS),-e)
all:
	@echo -en "Enabling external kernel tree in .config .."
	@$(if $(wildcard $(KERNEL_TREE)),\
		$(call check_uncheckin); \
		$(call check_patch_uncheckin); \
		sed -i '/.*CONFIG_EXTERNAL_KERNEL_TREE.*/d' $(TOPDIR)/.config || ( $(call err) ); \
		echo "CONFIG_EXTERNAL_KERNEL_TREE=\"$(KERNEL_TREE)\"" >> $(TOPDIR)/.config; \
		if [ -L $(LINUX_DIR) ]; then \
			opath=`cd $(LINUX_DIR)/ && pwd -P`; \
			if ! [ "$$opath" = "$(KERNEL_TREE)" ]; then \
				rm -f $(LINUX_DIR)/.prepared; \
			fi; \
		elif [ -d $(LINUX_DIR) ]; then rm -f $(LINUX_DIR)/.prepared; fi; $(call succ) \
	,$(call warn,\nNo kernel tree named '$(KERNEL_TREE_b)' found in TOPDIR. Run this script without any args to create one.))

    else
     ifeq ($(OPTS),-d)
all:
	@$(if $(wildcard $(KERNEL_TREE)),\
		$(call check_uncheckin); \
		$(call check_patch_uncheckin); \
		echo -en "Do you want to delete the tree '$(KERNEL_TREE_b)'? (y/N) "; \
		read cnfrm; \
		if [ -n "$$cnfrm" -a "$$cnfrm" = "y" ]; then \
			rm -f $(KERNEL_TREE)/target_patches/status $(KERNEL_TREE)/target_patches/guards; \
			rm -rf $(KERNEL_TREE) && echo "Kernel tree deleted."; \
			sed -i '/.*CONFIG_EXTERNAL_KERNEL_TREE.*/d' $(TOPDIR)/.config && echo "Disabled kernel tree in .config."; \
			if [ -L $(LINUX_DIR) ]; then rm -f $(LINUX_DIR)/.prepared; fi; \
		else $(call warn,Deletion aborted!); fi; \
	,$(call warn,No kernel tree named '$(KERNEL_TREE_b)' found in TOPDIR to delete.);)

     else
      ifeq ($(OPTS),-u)
all:
	@$(if $(wildcard $(KERNEL_TREE)),\
		@echo -en "Recreating target symlinks..\n"; \
		cd $(KERNEL_TREE)/ && { \
			hg qpop -a && { \
				echo -en "Removing existing links..\n"; \
				for ifls in `find . -type l`; do readlink $$ifls|grep "^/"|grep -vq kernel_patches && rm -f $$ifls; done; \
				$(call setup_target_files_links) \
				fchf=""; fchf=`hg status`; \
				if [ -n "$$fchf" ]; then \
					echo -en "Updated files:-\n$$fchf\n"; \
					hg addremove > /dev/null 2>/dev/null || ( $(call err) ); \
					unset hg; hg ci -m "temporary checkin" || ( $(call err) ); \
				else echo -en "No change in links.\n"; fi; \
			} || echo -en "Error: unable to qpop all patches. Please check..\n"; \
			cd - >/dev/null; \
		} \
	,$(call warn,No kernel tree named '$(KERNEL_TREE_b)' found in TOPDIR);)

      else
all:
	@$(if $(OPTS),$(call err,Unknown option. Use -h for help))
	@echo -en "Verifying path and .config.."
	@if [ -z "$(BOARD)" ] || [ "$(BOARD)" != "ltqcpe" -a "$(BOARD)" != "lantiq" ]; then $(call err,Please select a valid model to proceed.) fi
	@$(if $(wildcard $(TOPDIR)/.config),,$(call err,Please select a model before executing this script.))$(call succ)
	@echo -en "Creating directory.."
	@$(if $(wildcard $(KERNEL_TREE)),$(call err,A kernel tree named '$(KERNEL_TREE_b)' already found in TOPDIR. Please use it.))
	@mkdir $(KERNEL_TREE) || ( $(call err,Unable to create directory '$(KERNEL_TREE_b)') );$(call succ)
	@echo -en "Verifying linux tarball.."
	@$(if $(wildcard $(DL_DIR)/$(LINUX_TAR)),,\
		echo -en "$(C_YE) Downloading..$(C_RS)\n"; \
		sed -i '/.*CONFIG_EXTERNAL_KERNEL_TREE.*/d' $(TOPDIR)/.config; \
		make target/linux/download >/dev/null 2>/dev/null || ( $(call err) ) && echo "Downloading complete."; \
	)$(call succ)
	@echo -en "Initializing external kernel tree.."
	@cd $(KERNEL_TREE)/ && ( hg init || ( $(call err) ) );$(call succ)
	@echo -en "Extracting kernel.."
	@cd $(KERNEL_TREE)/ && ( tar -xjf $(DL_DIR)/$(LINUX_TAR) || ( $(call err) ) )
	@cd $(KERNEL_TREE)/ && ( mv $(LINUX_NAME)/* .; rm -rf $(LINUX_NAME); );$(call succ)
	@echo -en "Copying generic files.."
	@$(CP) -L $(GENERIC_FILES_DIR)/* $(KERNEL_TREE)/ && ( $(call succ) ) || ( $(call err) )
	@$(call setup_target_files_links)
	@echo -en "Applying generic patches.."
	@$(foreach pfls,$(wildcard $(GENERIC_PATCH_DIR)/*), \
		patch -p1 -i $(pfls) -d $(KERNEL_TREE)/ >/dev/null || ( $(call err) ); \
	)$(call succ)
	@echo -en "Linking target patches.."
	@ln -s $(PATCH_DIR_p) $(KERNEL_TREE)/target_patches
	@mkdir -p $(KERNEL_TREE)/.hg; ln -sf ../target_patches $(KERNEL_TREE)/.hg/patches;rm -f $(KERNEL_TREE)/target_patches/{series,status,guards}
	@$(if ,cd $(KERNEL_TREE)/target_patches && ( hg update 2>/dev/null || true ))
	@cd $(KERNEL_TREE)/target_patches && ( ls * > series; sed -i '/.*interaptiv.*/I s/$$/ #+interaptiv/' series; );$(call succ)
	@echo -en "Adding files to version control.."
	@cd $(KERNEL_TREE)/ && ( hg add > /dev/null 2>/dev/null || ( $(call err) ) )
	@cd $(KERNEL_TREE)/ && ( unset hg; hg ci -m "temporary checkin" || ( $(call err) ) );$(call succ)
	@echo -en "Applying patches.."
	@cd $(KERNEL_TREE)/ && ( $(if $(wildcard $(TOPDIR)/NoInterAptiv),hg qselect -n;,hg qselect interaptiv;) hg qpush -a > /dev/null || ( $(call err) ) );$(call succ)
	@echo -en "Adding ignore tracking list.."
	@echo "syntax:glob" > $(KERNEL_TREE)/.hgignore;$(foreach igfls,$(HG_IGNORE_FILES),echo "$(igfls)" >> $(KERNEL_TREE)/.hgignore;)$(call succ)
	@echo -en "Assigning kernel tree to .config.."
	@sed -i '/.*CONFIG_EXTERNAL_KERNEL_TREE.*/d' $(TOPDIR)/.config || ( $(call err) )
	@echo "CONFIG_EXTERNAL_KERNEL_TREE=\"$(KERNEL_TREE)\"" >> $(TOPDIR)/.config && rm -f $(LINUX_DIR)/.prepared || ( $(call err) );$(call succ)
	@echo "Source code ready!!"

      endif
     endif
    endif
   endif
  endif
endif
