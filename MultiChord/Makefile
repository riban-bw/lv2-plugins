#!/usr/bin/make -f
# Makefile for riban MultiChord plufin
# ---------------------------- #
# Created by riban - derived from falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = riban_midi_multichord

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	MultiChord.cpp

# --------------------------------------------------------------
# Do some magic

include ../Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(LINUX),true)
TARGETS += jack
endif

TARGETS += lv2_dsp
TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
