#include "rp_gen.h"
#include <math.h>
#include <assert.h>

#define RP_PI32 3.14159265359f

void rp_gen(struct rp_data *data)
{
	float *vertices = data->vertices;
	uint16_t *indices = data->indices;
	const int32_t facet_count = data->facet_count;
	const float facet_radius = data->facet_radius;
	const float extrusion_depth = data->extrusion_depth;

	assert(vertices && indices);
	assert(facet_count >= 3);
	assert(facet_radius > 0.0f);
	assert(extrusion_depth > 0.0f);

	const float facet_rad = RP_PI32 * 2.0f / (float)facet_count;

	const float front_face_color_r = 1.0f;
	const float front_face_color_g = 0.0f;
	const float front_face_color_b = 0.0f;

	const float back_face_color_r = 0.0f;
	const float back_face_color_g = 0.0f;
	const float back_face_color_b = 1.0f;

	const float edge_color_r = 0.0f;
	const float edge_color_g = 1.0f;
	const float edge_color_b = 0.0f;

	int32_t vertex_offset = 0;

	// front facet vertices
	int32_t front_facet_start_vertex = vertex_offset / RP_VERTEX_STRIDE;
	for (int32_t facet_idx = 0; facet_idx < facet_count; ++facet_idx) {
		int32_t idx = facet_idx * RP_VERTEX_STRIDE + vertex_offset;
		float rad = facet_rad * facet_idx;
		vertices[idx + 0] = sinf(rad) * facet_radius;
		vertices[idx + 1] = cosf(rad) * facet_radius;
		vertices[idx + 2] = 0.0f;
		vertices[idx + 3] = front_face_color_r;
		vertices[idx + 4] = front_face_color_g;
		vertices[idx + 5] = front_face_color_b;
		vertices[idx + 6] = 1.0f;
	}
	vertex_offset += (facet_count * RP_VERTEX_STRIDE);

	// front edge vertices
	int32_t front_edge_start_vertex = vertex_offset / RP_VERTEX_STRIDE;
	for (int32_t facet_idx = 0; facet_idx < facet_count; ++facet_idx) {
		int32_t idx = facet_idx * RP_VERTEX_STRIDE + vertex_offset;
		float rad = facet_rad * facet_idx;
		vertices[idx + 0] = sinf(rad) * facet_radius;
		vertices[idx + 1] = cosf(rad) * facet_radius;
		vertices[idx + 2] = 0.0f;
		vertices[idx + 3] = edge_color_r;
		vertices[idx + 4] = edge_color_g;
		vertices[idx + 5] = edge_color_b;
		vertices[idx + 6] = 1.0f;
	}
	vertex_offset += (facet_count * RP_VERTEX_STRIDE);

	// back facet vertices
	int32_t back_facet_start_vertex = vertex_offset / RP_VERTEX_STRIDE;
	for (int32_t facet_idx = 0; facet_idx < facet_count; ++facet_idx) {
		int32_t idx = facet_idx * RP_VERTEX_STRIDE + vertex_offset;
		float rad = facet_rad * facet_idx;
		vertices[idx + 0] = sinf(rad) * facet_radius;
		vertices[idx + 1] = cosf(rad) * facet_radius;
		vertices[idx + 2] = -extrusion_depth;
		vertices[idx + 3] = back_face_color_r;
		vertices[idx + 4] = back_face_color_g;
		vertices[idx + 5] = back_face_color_b;
		vertices[idx + 6] = 1.0f;
	}
	vertex_offset += (facet_count * RP_VERTEX_STRIDE);

	// back edge vertices
	int32_t back_edge_start_vertex = vertex_offset / RP_VERTEX_STRIDE;
	for (int32_t facet_idx = 0; facet_idx < facet_count; ++facet_idx) {
		int32_t idx = facet_idx * RP_VERTEX_STRIDE + vertex_offset;
		float rad = facet_rad * facet_idx;
		vertices[idx + 0] = sinf(rad) * facet_radius;
		vertices[idx + 1] = cosf(rad) * facet_radius;
		vertices[idx + 2] = -extrusion_depth;
		vertices[idx + 3] = edge_color_r;
		vertices[idx + 4] = edge_color_g;
		vertices[idx + 5] = edge_color_b;
		vertices[idx + 6] = 1.0f;
	}
	vertex_offset += (facet_count * RP_VERTEX_STRIDE);

	// front center vertex
	int32_t front_center_vertex = vertex_offset / RP_VERTEX_STRIDE;
	vertices[vertex_offset + 0] = 0.0f;
	vertices[vertex_offset + 1] = 0.0f;
	vertices[vertex_offset + 2] = 0.0f;
	vertices[vertex_offset + 3] = front_face_color_r;
	vertices[vertex_offset + 4] = front_face_color_g;
	vertices[vertex_offset + 5] = front_face_color_b;
	vertices[vertex_offset + 6] = 1.0f;
	vertex_offset += RP_VERTEX_STRIDE;

	// back center vertex
	int32_t back_center_vertex = vertex_offset / RP_VERTEX_STRIDE;
	vertices[vertex_offset + 0] = 0.0f;
	vertices[vertex_offset + 1] = 0.0f;
	vertices[vertex_offset + 2] = -extrusion_depth;
	vertices[vertex_offset + 3] = back_face_color_r;
	vertices[vertex_offset + 4] = back_face_color_g;
	vertices[vertex_offset + 5] = back_face_color_b;
	vertices[vertex_offset + 6] = 1.0f;
	vertex_offset += RP_VERTEX_STRIDE;

	int32_t index_offset = 0;

	// front face indices
	for (uint16_t i = 0; i < facet_count; i += 1) {
		int32_t idx = index_offset + i * RP_INDEX_STRIDE;
		indices[idx + 0] = front_center_vertex;
		indices[idx + 1] = front_facet_start_vertex + i;
		indices[idx + 2] = front_facet_start_vertex + (i + 1) % facet_count;
	}
	index_offset += facet_count * RP_INDEX_STRIDE;

	// back face indices
	for (uint16_t i = 0; i < facet_count; i += 1) {
		int32_t idx = index_offset + i * RP_INDEX_STRIDE;
		indices[idx + 0] = back_center_vertex;
		indices[idx + 1] = back_facet_start_vertex + (i + 1) % facet_count;
		indices[idx + 2] = back_facet_start_vertex + i;
	}
	index_offset += facet_count * RP_INDEX_STRIDE;

	// edge indices
	for (uint16_t i = 0; i < facet_count; i += 1) {
		uint16_t start_vertex = (uint16_t)front_edge_start_vertex + i;
		uint16_t end_vertex = (uint16_t)back_edge_start_vertex + (i + 1) % facet_count;
		int32_t idx = index_offset + i * (RP_INDEX_STRIDE * 2);
		indices[idx + 0] = start_vertex;
		indices[idx + 1] = (uint16_t)back_edge_start_vertex + i;
		indices[idx + 2] = end_vertex;
		indices[idx + 3] = end_vertex;
		indices[idx + 4] = (uint16_t)front_edge_start_vertex + (i + 1) % facet_count;
		indices[idx + 5] = start_vertex;
	}
	index_offset += facet_count * (RP_INDEX_STRIDE * 2);

	return;
}

