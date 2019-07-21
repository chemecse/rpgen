#ifndef RP_GEN_H
#define RP_GEN_H

#include <stdint.h>

#define RP_VERTEX_STRIDE 7
#define RP_GET_VERTEX_ELEMENT_COUNT(facet_count) ((facet_count * 4 + 2) * RP_VERTEX_STRIDE)

#define RP_INDEX_STRIDE 3
#define RP_GET_INDEX_ELEMENT_COUNT(facet_count) (facet_count * (3 + 3 + 6))

#define RP_FACET_RADIUS 2.0f
#define RP_DEPTH 0.3f

void rp_gen(int32_t facet_count, float *vertices, int16_t *indices);

#endif

