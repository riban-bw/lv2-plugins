#!/usr/bin/make -f
# Makefile for riban MultiChord plugin
# Created by riban - derived from falkTX

# Project name, used for binaries
NAME = ribanCCSend

# Files to build
FILES_DSP = \
	ccsend.cpp

# DPF magic
include ../Makefile.plugins.mk

# Target to build LV2 plugin and generate ttl
lv2_with_ttl: lv2_dsp
	cd ../bin/lv2/$(NAME).lv2 && ../../../DPF/utils/lv2_ttl_generator ./$(NAME)_dsp.so

# Enable required plugin types
#TARGETS += jack
TARGETS += lv2_with_ttl
#TARGETS += vst2
#TARGETS += vst3
#TARGETS += clap
#TARGETS += au

all: $(TARGETS)
