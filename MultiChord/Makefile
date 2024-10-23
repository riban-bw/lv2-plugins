#!/usr/bin/make -f
# Makefile for riban MultiChord plufin
# ---------------------------- #
# Created by riban - derived from falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = ribanMultiChord

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	MultiChord.cpp

# --------------------------------------------------------------
# Do some magic

include ../Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all required types

#TARGETS += jack
TARGETS += lv2_dsp
#TARGETS += vst2
#TARGETS += vst3
#TARGETS += clap
#TARGETS += au

all: $(TARGETS)

# --------------------------------------------------------------
