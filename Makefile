SHELL := /bin/sh
TOP := $(abspath .)
OUTER := $(abspath $(TOP)/..)
DIST := $(TOP)/build/dist
RELEASE := $(TOP)/game/release
PORTABLE_DATA := $(DIST)/portable
ANDROID_HOME ?= $(abspath $(OUTER)/../ANDROID_SDK)
export ANDROID_HOME
MINGW_BIN ?= /usr/x86_64-w64-mingw32/sys-root/mingw/bin
PROGRAM := $(shell sed -n 's/^program=//p' game/game.cfg)

.PHONY: all host input recomp gen obj linux dist windows portable portable-data apk clean distclean help test build-sr regen-llasm

# Default target
all: linux windows

# Stage targets
host:
	scons stage=host

input:
	scons stage=input

recomp:
	scons stage=recomp

gen:
	scons stage=gen

obj:
	scons stage=obj

dist:
	scons stage=dist

# Native (Linux) build: the device follows the host — pc64-linux on x86_64,
# arm64-linux on aarch64 (game.py: no -m64 there, the aarch64 gcc rejects it).
HOST_ARCH := $(shell uname -m)
ifeq ($(HOST_ARCH),aarch64)
LINUX_DEVICE := arm64-linux
else
LINUX_DEVICE := pc64-linux
endif

linux:
	scons stage=bin device=$(LINUX_DEVICE) -j$$(nproc)

# Windows cross build. Separate from `linux`: two concurrent scons
# invocations race on the shared build/ tree, so run them one at a time.
# Stages the MinGW runtime DLLs beside the exe (SDL2.dll is sdl2-compat and
# loads SDL3.dll) so build/bin/pc64-windows/ is self-contained.
windows:
	scons stage=bin device=pc64-windows -j$$(nproc)
	@for dll in SDL2.dll SDL3.dll SDL2_mixer.dll; do \
		[ -f "$(MINGW_BIN)/$$dll" ] || { echo "Missing $$dll in $(MINGW_BIN)" >&2; exit 1; }; \
		cp -p "$(MINGW_BIN)/$$dll" "$(TOP)/build/bin/pc64-windows/$$dll"; \
	done

# Aliases
build-sr: host
regen-llasm: gen

# Portable game directory: a copy of the disc's files (the .G93/.Z94 scenes
# and sounds, the .DEF and .C93 files) without the installer, as
# game/portable.cfg describes it, plus the _CD_ marker (the directory that
# tells the port that the data directory is the disc's root, the drive D:).
# Needs python3; built once, delete build/dist/portable to rebuild it.
portable-data:
	@if [ -f "$(PORTABLE_DATA)/CYB.EXE" ]; then \
		echo "==> Portable data: $(PORTABLE_DATA)"; \
	else \
		command -v python3 >/dev/null || { echo "python3 not found"; exit 1; }; \
		echo "==> Portable data: making $(PORTABLE_DATA) (game/portable.cfg)"; \
		python3 $(TOP)/SR-SDK/tools/portable_cd.py "$(PORTABLE_DATA)"; \
		mkdir -p "$(PORTABLE_DATA)/_CD_"; \
	fi

# Release archives in build/dist/: the port, its launchers/readmes from
# game/release/, plus the game data; per platform, like IGNITION's dist/
portable: linux windows portable-data
	@echo "==> build/dist/cyberia-linux-x64"
	rm -rf $(DIST)/cyberia-linux-x64
	mkdir -p $(DIST)/cyberia-linux-x64
	cp $(TOP)/build/bin/pc64-linux/$(PROGRAM) $(DIST)/cyberia-linux-x64/
	cp $(RELEASE)/linux/cyberia.sh $(DIST)/cyberia-linux-x64/
	cp $(RELEASE)/linux/x64/readme-Linux.txt $(DIST)/cyberia-linux-x64/
	cp -a $(PORTABLE_DATA)/. $(DIST)/cyberia-linux-x64/
	chmod +x $(DIST)/cyberia-linux-x64/$(PROGRAM) $(DIST)/cyberia-linux-x64/cyberia.sh
	tar -C $(DIST) -czf $(DIST)/cyberia-linux-x64.tar.gz cyberia-linux-x64
	@echo "==> build/dist/cyberia-windows-x64"
	rm -rf $(DIST)/cyberia-windows-x64
	mkdir -p $(DIST)/cyberia-windows-x64
	cp $(TOP)/build/bin/pc64-windows/$(PROGRAM).exe $(DIST)/cyberia-windows-x64/
	cp $(TOP)/build/bin/pc64-windows/*.dll $(DIST)/cyberia-windows-x64/
	cp $(RELEASE)/windows/cyberia.cmd $(DIST)/cyberia-windows-x64/
	cp $(RELEASE)/windows/x64/readme-Windows.txt $(DIST)/cyberia-windows-x64/
	cp -a $(PORTABLE_DATA)/. $(DIST)/cyberia-windows-x64/
	rm -f $(DIST)/cyberia-windows-x64.zip
	cd $(DIST) && zip -qr cyberia-windows-x64.zip cyberia-windows-x64
	@echo "OK: $(DIST)/cyberia-linux-x64.tar.gz"
	@echo "OK: $(DIST)/cyberia-windows-x64.zip"

# Android APK: SDL for arm64, libmain.so via scons, gradle; the result goes to
# build/dist/.  All generated bits (SDL, .so, game data, gradle output/cache)
# land in build/ (SR-SDK/tools/android_apk.py).
apk: portable-data
	python3 $(TOP)/SR-SDK/tools/android_apk.py --data $(PORTABLE_DATA) --out $(DIST)/cyberia-android-arm64.apk

# Clean
clean:
	@echo "==> Cleaning build/ run/ and artifacts"
	rm -rf $(TOP)/build $(TOP)/run
	rm -f $(TOP)/.sconsign.dblite $(TOP)/*.EXE $(TOP)/*.exe

distclean: clean
	rm -rf $(TOP)/.sconsign.dblite* $(DIST)

# Quick test
test:
	@echo "==> Test: clean + build + verify"
	$(MAKE) clean >/dev/null
	$(MAKE) linux >/dev/null
	@test -f build/bin/$(LINUX_DEVICE)/$(PROGRAM) && echo "OK: binary exists"

help:
	@echo "Targets:"
	@echo "  all (default)  build Linux + Windows binaries"
	@echo "  host           build host tools (SR.exe + llasm)"
	@echo "  input          stage 0"
	@echo "  recomp         stage 2"
	@echo "  gen            stage 3"
	@echo "  obj            stage 4"
	@echo "  linux          build the native Linux binary (pc64-linux / arm64-linux)"
	@echo "  dist           stage 6 (if implemented)"
	@echo "  windows        build the Windows cross target (pc64-windows)"
	@echo "  portable       release archives in build/dist/ (port + game data)"
	@echo "  portable-data  make build/dist/portable (the disc's files) if missing"
	@echo "  apk            Android APK into build/dist/ (scons arm64-android + gradle)"
	@echo "  build-sr       alias for host"
	@echo "  regen-llasm    alias for gen"
	@echo "  clean          remove build/, run/, artifacts"
	@echo "  distclean      clean + caches + dist/"
	@echo "  test           quick integrity check"
