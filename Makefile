#!/usr/bin/make -f
# Makefile for riban plugins #
# ---------------- #
# Created by riban - derived from falkTX
#

include DPF/Makefile.base.mk

LV2_INSTALL_PREFIX  ?= /usr/local/lib/lv2

all: dgl plugins gen deb

# --------------------------------------------------------------

ifneq ($(CROSS_COMPILING),true)
CAN_GENERATE_TTL = true
else ifneq ($(EXE_WRAPPER),)
CAN_GENERATE_TTL = true
endif

dgl:
	$(MAKE) -C DPF/dgl

plugins: dgl
	$(MAKE) all -C MultiChord

ifeq ($(CAN_GENERATE_TTL),true)
gen: plugins utils/lv2_ttl_generator
	@$(CURDIR)/DPF/utils/generate-ttl.sh
ifeq ($(MACOS),true)
	@$(CURDIR)/DPF/utils/generate-vst-bundles.sh
endif
else
gen:
endif

utils/lv2_ttl_generator:
	$(MAKE) -C DPF/utils/lv2-ttl-generator

# --------------------------------------------------------------

clean: clean_plugin clean_dpf clean_deb

clean_plugin:
	$(MAKE) clean -C MultiChord
	rm -rf bin build

clean_dpf:
	$(MAKE) clean -C DPF/dgl
	$(MAKE) clean -C DPF/utils/lv2-ttl-generator

clean_deb:
	rm -rf riban-lv2$(LV2_INSTALL_PREFIX)

install: plugins
	mkdir -p $(LV2_INSTALL_PREFIX)
	cp -r bin/lv2/* $(LV2_INSTALL_PREFIX)/

deb: plugins
	mkdir -p riban-lv2$(LV2_INSTALL_PREFIX)
	cp -r bin/lv2/* riban-lv2$(LV2_INSTALL_PREFIX)/
	dpkg-deb --build riban-lv2 bin

# --------------------------------------------------------------

.PHONY: dgl plugins
