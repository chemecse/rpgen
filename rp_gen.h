#ifndef RP_GEN_H
#define RP_GEN_H

#include <stdint.h>

#define RP_VERTEX_STRIDE 7
// 4 vertices per facet (1 front face, 1 back face, 2 edge) + 2 center vertices
#define RP_GET_VERTEX_ELEMENT_COUNT(facet_count) ((facet_count * 4 + 2) * RP_VERTEX_STRIDE)

#define RP_INDEX_STRIDE 3
// 4 triangles per facet (each triangle represented by 3 indicies)
#define RP_GET_INDEX_ELEMENT_COUNT(facet_count) ((facet_count * 4) * RP_INDEX_STRIDE)

struct rp_data {
	float *vertices;
	int16_t *indices;
	int32_t facet_count;
	float facet_radius;
	float extrusion_depth;
};

void rp_gen(struct rp_data *data);

#endif

