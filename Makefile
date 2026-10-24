# The targets (linux, windows, portable, apk, clean, ...) are SR-SDK/scons/game.mk's.
# the Android SDK of the workspace, for `make apk`
export ANDROID_HOME ?= $(abspath $(CURDIR)/../../ANDROID_SDK)
include SR-SDK/scons/game.mk
