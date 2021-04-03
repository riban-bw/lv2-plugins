#!/usr/bin/make -f
# Makefile for riban plugins #
# ---------------- #
# Created by riban - derived from falkTX
#

include DPF/Makefile.base.mk

LV2_INSTALL_PREFIX  ?= /usr/local/lib/lv2

all: dgl plugins gen

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

clean:
	$(MAKE) clean -C MultiChord
	rm -rf bin build

clean_dpf:
	$(MAKE) clean -C DPF/dgl
	$(MAKE) clean -C DPF/utils/lv2-ttl-generator

install: plugins
	cp -r bin/lv2/* $(LV2_INSTALL_PREFIX)/

# --------------------------------------------------------------

.PHONY: dgl plugins
