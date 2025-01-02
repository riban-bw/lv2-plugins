#!/usr/bin/make -f
# Makefile for riban plugins #
# Created by riban - derived from falkTX

# DPF magic
include DPF/Makefile.base.mk

# Default LV2 install folder
LV2_INSTALL_PREFIX  ?= /usr/lib/lv2

# Default target builds all riban lv2 plugins
all: plugins

# Target to build DGL
dgl:
	$(MAKE) -C DPF/dgl

# Target to build TTL generator command
utils/lv2_ttl_generator:
	$(MAKE) -C DPF/utils/lv2-ttl-generator

# Target to build riban lv2 plugins
plugins: dgl utils/lv2_ttl_generator
	$(MAKE) all -C MultiChord
	$(MAKE) all -C CCSend
	$(MAKE) all -C Chordulator
	$(MAKE) all -C TonalChords

# Target to build deb package containing all riban lv2 plugins
deb: plugins
	mkdir -p riban-lv2$(LV2_INSTALL_PREFIX)
	cp -r bin/lv2/* riban-lv2$(LV2_INSTALL_PREFIX)/
	dpkg-deb --build riban-lv2 bin

# Target to clean plugins
clean_plugin:
	$(MAKE) clean -C MultiChord
	$(MAKE) clean -C CCSend
	$(MAKE) clean -C Chordulator
	$(MAKE) clean -C TonalChords
	rm -rf bin build

# Target to clean DPF
clean_dpf:
	$(MAKE) clean -C DPF/dgl
	$(MAKE) clean -C DPF/utils/lv2-ttl-generator

# Target to clean deb package
clean_deb:
	rm -rf riban-lv2$(LV2_INSTALL_PREFIX)
	rm -f bin/riban-lv2*.deb

# Target to clean everything
clean: clean_plugin clean_dpf clean_deb

# Target to install plugins
install: plugins
	mkdir -p $(LV2_INSTALL_PREFIX)
	cp -r bin/lv2/* $(LV2_INSTALL_PREFIX)/

# Target to uninstall plugins
uninstall:
	rm -rf $(LV2_INSTALL_PREFIX)/riban*.lv2

# List of targets that do not build files from their title
.PHONY: dgl plugins all deb clean clean_plugin clean_dpf clean_deb install uninstall
