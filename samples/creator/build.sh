#!/bin/sh

set -eu

./sokol-shdc --input demo.glsl --output demo.glsl.h --slang glsl100

mkdir -p ./dist

emcc creator.c watt_math.c ../../rp_gen.c \
	-DSOKOL_GLES2=1 \
	-o ./dist/creator.js

