SHELL := /bin/sh
TOP := $(abspath .)
MINGW_BIN ?= /usr/x86_64-w64-mingw32/sys-root/mingw/bin
PROGRAM := $(shell sed -n 's/^program=//p' game/game.cfg)

.PHONY: all host input recomp gen obj linux windows clean help

all: linux windows

host input recomp gen obj:
	scons stage=$@

HOST_ARCH := $(shell uname -m)
ifeq ($(HOST_ARCH),aarch64)
LINUX_DEVICE := arm64-linux
else
LINUX_DEVICE := pc64-linux
endif
linux:
	scons stage=bin device=$(LINUX_DEVICE) -j$$(nproc)

# Never run it at the same time as `linux`: both write to build/.
windows:
	scons stage=bin device=pc64-windows -j$$(nproc)
	@for dll in SDL2.dll SDL3.dll SDL2_mixer.dll; do \
		[ -f "$(MINGW_BIN)/$$dll" ] || { echo "Missing $$dll in $(MINGW_BIN)" >&2; exit 1; }; \
		cp -p "$(MINGW_BIN)/$$dll" "$(TOP)/build/bin/pc64-windows/$$dll"; \
	done

clean:
	rm -rf $(TOP)/build $(TOP)/run $(TOP)/.sconsign.dblite

help:
	@echo "targets: host input recomp gen obj linux windows clean"
