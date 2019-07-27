#ifndef RP_GEN_H
#define RP_GEN_H

#include <stdint.h>

#define RP_VERTEX_STRIDE 7
// TODO(lars): document calculations
#define RP_GET_VERTEX_ELEMENT_COUNT(facet_count) ((facet_count * 4 + 2) * RP_VERTEX_STRIDE)

#define RP_INDEX_STRIDE 3
// TODO(lars): document calculations
#define RP_GET_INDEX_ELEMENT_COUNT(facet_count) (facet_count * (3 + 3 + 6))


struct rp_data {
	float *vertices;
	int16_t *indices;
	int32_t facet_count;
	float facet_radius;
	float extrusion_depth;
};

void rp_gen(struct rp_data *data);

#endif

