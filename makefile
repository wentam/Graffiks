INCLUDE := -Iinclude

.PHONY: android
ANDROID_PLATFORM := android-19
ANDROID_ARM_SYSROOT := $(NDK)/platforms/$(ANDROID_PLATFORM)/arch-arm/
ANDROID_ARM_TOOLCHAIN_BINARY_PATH := /toolchains/arm-linux-androideabi-4.8/prebuilt/linux-x86_64/bin/
ANDROID_ARM_CC := $(NDK)$(ANDROID_ARM_TOOLCHAIN_BINARY_PATH)arm-linux-androideabi-gcc \
				  --sysroot=$(ANDROID_ARM_SYSROOT) $(INCLUDE) -DANDROID -Wall -Wextra \
				  -Wno-implicit-function-declaration
ANDROID_ARM_LD := $(NDK)$(ANDROID_ARM_TOOLCHAIN_BINARY_PATH)arm-linux-androideabi-ld \
				  --sysroot=$(ANDROID_ARM_SYSROOT) $(INCLUDE)

X86_CC := gcc $(INCLUDE) -fPIC
X86_LD := ld $(INCLUDE)

default: android_arm

android_arm: CC = $(ANDROID_ARM_CC)
android_arm: LD = $(ANDROID_ARM_LD)
android_arm: ndk folders mesh core driver-android
	$(LD) -shared \
		-lc -lm -llog -lGLESv2 -landroid \
		o/renderer.o o/driver-jni-android.o \
		o/gl_helper.o o/graffiks.o \
		o/mesh.o o/cube_mesh.o o/plane_mesh.o o/triangle_mesh.o \
		o/material.o \
		-o lib/$@/libgraffiks.so

ndk:
ifndef NDK
    $(error export NDK=/your/ndk/path/ before running building)
endif

folders:
	mkdir -p lib
	mkdir -p lib/$@
	mkdir -p o

mesh:
	$(CC) -c src/mesh.c -o o/mesh.o
	$(CC) -c src/cube_mesh.c -o o/cube_mesh.o
	$(CC) -c src/plane_mesh.c -o o/plane_mesh.o
	$(CC) -c src/triangle_mesh.c -o o/triangle_mesh.o

core:
	$(CC) -c src/renderer.c -o o/renderer.o
	$(CC) -funroll-loops -c src/gl_helper.c -o o/gl_helper.o
	$(CC) -c src/graffiks.c -o o/graffiks.o
	$(CC) -c src/material.c -o o/material.o

driver-android:
	$(CC) -Wno-unused-parameter -c src/driver-jni-android.c -o o/driver-jni-android.o

clean:
	rm -rf o
	rm -rf lib
