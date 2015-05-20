RESOURCES = /usr/local/share/graffiks/
INCLUDE_DIR = /usr/local/include/graffiks/
LIB_DIR = /usr/local/lib/

INCLUDE := -Iinclude
CC_WARNINGS = -Wall -Wextra -Wno-implicit-function-declaration

.PHONY: android
ANDROID_PLATFORM := android-19
ANDROID_ARM_SYSROOT := $(NDK)/platforms/$(ANDROID_PLATFORM)/arch-arm/
ANDROID_ARM_TOOLCHAIN_BINARY_PATH := /toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin/
ANDROID_ARM_CC := $(NDK)$(ANDROID_ARM_TOOLCHAIN_BINARY_PATH)arm-linux-androideabi-gcc \
				  --sysroot=$(ANDROID_ARM_SYSROOT) $(INCLUDE) -DANDROID $(CC_WARNINGS)
ANDROID_ARM_LD := $(NDK)$(ANDROID_ARM_TOOLCHAIN_BINARY_PATH)arm-linux-androideabi-ld \
				  --sysroot=$(ANDROID_ARM_SYSROOT) $(INCLUDE)

LINUX_CC := gcc $(INCLUDE) -DLINUX -DRESOURCE_PATH=\"$(RESOURCES)\" $(CC_WARNINGS) -fPIC
LINUX_LD := ld $(INCLUDE)

default: linux

linux: CC = $(LINUX_CC)
linux: LD = $(LINUX_LD)
linux: folders mesh object core driver-linux
	mkdir -p lib/$@
	$(LD) -shared \
		-lc -lm -lX11 -lGL -lGLU -lGLEW \
		o/governor.o o/driver-linux.o \
		o/gl_helper.o o/graffiks.o \
		o/mesh.o \
		o/cube_mesh.o o/plane_mesh.o o/triangle_mesh.o \
		o/obj_loader.o o/mtl_loader.o\
		o/object.o \
		o/material.o \
		-o lib/$@/libgraffiks.so

android_arm: CC = $(ANDROID_ARM_CC)
android_arm: LD = $(ANDROID_ARM_LD)
android_arm: ndk folders mesh object core driver-android
	mkdir -p lib/$@
	$(LD) -shared \
		-lc -lm -llog -lGLESv2 -landroid \
		o/governor.o o/driver-jni-android.o \
		o/gl_helper.o o/graffiks.o \
		o/mesh.o \
		o/cube_mesh.o o/plane_mesh.o o/triangle_mesh.o \
		o/material.o \
		-o lib/$@/libgraffiks.so

ndk:
	ifndef NDK
	$(error export NDK=/your/ndk/path/ before building)
	endif

folders:
	mkdir -p lib
	mkdir -p o

mesh:
	$(CC) -c src/mesh/mesh.c -o o/mesh.o
	$(CC) -c src/mesh/cube_mesh.c -o o/cube_mesh.o
	$(CC) -c src/mesh/plane_mesh.c -o o/plane_mesh.o
	$(CC) -c src/mesh/triangle_mesh.c -o o/triangle_mesh.o

object:
	$(CC) -c src/object/obj_loader.c -o o/obj_loader.o
	$(CC) -c src/object/mtl_loader.c -o o/mtl_loader.o
	$(CC) -c src/object/object.c -o o/object.o

core:
	$(CC) -c src/governor.c -o o/governor.o
	$(CC) -funroll-loops -c src/gl_helper.c -o o/gl_helper.o
	$(CC) -c src/graffiks.c -o o/graffiks.o
	$(CC) -c src/material.c -o o/material.o

driver-android:
	$(CC) -Wno-unused-parameter -c src/driver/driver-jni-android.c -o o/driver-jni-android.o

driver-linux:
	$(CC) -Wno-unused-parameter -c src/driver/driver-linux.c -o o/driver-linux.o

install: install-linux

install-linux:
	mkdir -p $(LIB_DIR)
	cp lib/linux/libgraffiks.so $(LIB_DIR)
	mkdir -p $(RESOURCES)
	cp -r share/* $(RESOURCES)
	mkdir -p $(INCLUDE_DIR)
	cp -r include/* $(INCLUDE_DIR)

clean:
	rm -rf o
	rm -rf lib
