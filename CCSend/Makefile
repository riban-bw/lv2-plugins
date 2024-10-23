#!/usr/bin/make -f
# Makefile for riban ccSend plugin
# ---------------------------- #
# Created by riban - derived from falkTX
#

# --------------------------------------------------------------
# Project name, used for binaries

NAME = ribanCCSend

# --------------------------------------------------------------
# Files to build

FILES_DSP = \
	ccsend.cpp

# --------------------------------------------------------------
# Do some magic

include ../Makefile.plugins.mk

# --------------------------------------------------------------
# Enable all possible plugin types

ifeq ($(LINUX),true)
#TARGETS += jack
endif

TARGETS += lv2_dsp
#TARGETS += vst

all: $(TARGETS)

# --------------------------------------------------------------
