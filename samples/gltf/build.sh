#!/bin/sh

set -eu

gcc write_gltf.c ../../rp_gen.c \
	-o gltfwriter

