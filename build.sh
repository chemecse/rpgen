#!/bin/sh

set -eu

./bin/sokol-shdc --input demo.glsl --output demo.glsl.h --slang metal_macos:glsl100

mkdir -p ./dist

gcc demo.c rp_gen.c watt_math.c \
	-DSOKOL_METAL=1 \
	-o ./dist/demo \
	-ObjC \
	-fobjc-arc \
	-framework Cocoa \
	-framework QuartzCore \
	-framework Metal \
	-framework MetalKit \
	-framework AudioToolbox

emcc demo.c rp_gen.c watt_math.c \
	-DSOKOL_GLES2=1 \
	-o ./dist/demo.js

