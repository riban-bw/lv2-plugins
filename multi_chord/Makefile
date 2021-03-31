#!/usr/bin/make -f
# Makefile for multi_chord.lv2 #
# ----------------------- #
#

AR  ?= ar
CC  ?= gcc
CXX ?= g++
BASE_FLAGS = -Wall -Wextra -pipe -Wno-unused-parameter
BASE_OPTS  = -O3 -ffast-math
LINK_OPTS  = -Wl,-O1 -Wl,--as-needed -Wl,--strip-all
BASE_FLAGS += -fPIC -DPIC
BASE_FLAGS += -DNDEBUG $(BASE_OPTS) -fvisibility=hidden
CXXFLAGS   += -fvisibility-inlines-hidden
BUILD_C_FLAGS   = $(BASE_FLAGS) -std=c99 -std=gnu99 $(CFLAGS)
BUILD_CXX_FLAGS = $(BASE_FLAGS) -std=c++11 $(CXXFLAGS) $(CPPFLAGS)
LINK_FLAGS      = $(LINK_OPTS) $(LDFLAGS)
LIB_EXT = .so
SHARED = -shared

# --------------------------------------------------------------

PREFIX  ?= /usr/local
DESTDIR ?=

# --------------------------------------------------------------
# Default target is to build all plugins

all: build

# --------------------------------------------------------------
# multi_chord build rules

build: multi_chord.lv2/multi_chord$(LIB_EXT)

multi_chord.lv2/multi_chord$(LIB_EXT): multi_chord.c
	$(CXX) $^ $(BUILD_CXX_FLAGS) $(LINK_FLAGS) -lm $(SHARED) -o $@


# --------------------------------------------------------------

clean:
	rm -f multi_chord.lv2/multi_chord$(LIB_EXT)

# --------------------------------------------------------------

install: build
	install -d $(DESTDIR)$(PREFIX)/lib/lv2/riban.lv2

	install -m 644 multi_chord.lv2/*.so  $(DESTDIR)$(PREFIX)/lib/lv2/riban/multi_chord.lv2/
	install -m 644 multi_chord.lv2/*.ttl $(DESTDIR)$(PREFIX)/lib/lv2/riban.multi_chord.lv2/

# --------------------------------------------------------------
