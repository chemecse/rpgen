#define CGLTF_WRITE_IMPLEMENTATION
#include "cgltf_write.h"
#include "../../rp_gen.h"

struct write_data {
	cgltf_buffer buffers[1];
	cgltf_buffer_view buffer_views[3];
	cgltf_accessor accessors[3];
	cgltf_attribute attributes[2];
	cgltf_material material;
	cgltf_primitive primitive;
	cgltf_mesh mesh;
	cgltf_node nodes[2];
	cgltf_node *node_refs[2];
	cgltf_scene scene;
};

int main(void) {
	const int32_t facet_count = 5;
	const int32_t vertex_element_count = RP_GET_VERTEX_ELEMENT_COUNT(facet_count);
	const int32_t index_element_count = RP_GET_INDEX_ELEMENT_COUNT(facet_count);

    const size_t buffer_size = vertex_element_count * (int32_t)sizeof(float) + index_element_count * (int32_t)sizeof(uint16_t);

	printf("facet count %d\n", facet_count);
	printf("vertex element count %d\n", vertex_element_count);
	printf("index element count %d\n", index_element_count);
	printf("vertex buffer size %d\n", vertex_element_count * (int32_t)sizeof(float));
	printf("index buffer size %d\n", index_element_count * (int32_t)sizeof(uint16_t));
	printf("total buffer size %d\n", (int32_t)buffer_size);

    void *buffer = calloc(buffer_size, 1);
    uint16_t *indices = (uint16_t *)buffer;
    float *vertices = (float *)(buffer + (index_element_count * sizeof(uint16_t)));

    rp_gen(&(struct rp_data){
        .vertices = vertices,
        .indices = indices,
        .facet_count = facet_count,
        .facet_radius = 2.0f,
        .extrusion_depth = 0.3f
    });

    FILE *bin_file = fopen("./out.bin", "wb");
    fwrite(buffer, buffer_size, 1, bin_file);
    fclose(bin_file);

	struct write_data wd = {0};

	size_t index_buffer_size = index_element_count * sizeof(uint16_t);
	size_t vertex_buffer_size = vertex_element_count * sizeof(float);

	wd.buffers[0] = (cgltf_buffer){
		.size =  index_buffer_size + vertex_buffer_size,
		.uri = "out.bin"
	};

	// index buffer view
	wd.buffer_views[0] = (cgltf_buffer_view){
		.buffer = &wd.buffers[0],
		.offset = 0,
		.size = 72,
		.type = cgltf_buffer_view_type_indices
	};

	// position buffer view
	wd.buffer_views[1] = (cgltf_buffer_view){
		.buffer = &wd.buffers[0],
		.offset = index_buffer_size,
		.size = 392,
		.stride = 28,
		.type = cgltf_buffer_view_type_vertices
	};

	// color buffer view
	wd.buffer_views[2] = (cgltf_buffer_view){
		.buffer = &wd.buffers[0],
		.offset =  index_buffer_size + 12,
		.size = 392 - 12,
		.stride = 28,
		.type = cgltf_buffer_view_type_vertices
	};

	// index accessor
	wd.accessors[0] = (cgltf_accessor){
		.component_type = cgltf_component_type_r_16u,
		.type = cgltf_type_scalar,
		.offset = 0,
		.count = 36,
		.buffer_view = &wd.buffer_views[0]
	};

	// position accessor
	wd.accessors[1] = (cgltf_accessor){
		.component_type = cgltf_component_type_r_32f,
		.type = cgltf_type_vec3,
		.offset = 0,
		.count = 14,
		.buffer_view = &wd.buffer_views[1]
	};

	// color accessor
	wd.accessors[2] = (cgltf_accessor){
		.component_type = cgltf_component_type_r_32f,
		.type = cgltf_type_vec4,
		.offset = 0,
		.count = 14,
		.buffer_view = &wd.buffer_views[2]
	};

	// position attribute
	wd.attributes[0] = (cgltf_attribute){
		.name = "POSITION",
		.type = cgltf_attribute_type_position,
		.index = 1,
		.data = &wd.accessors[1]
	};

	// color attribute
	wd.attributes[1] = (cgltf_attribute){
		.name = "COLOR_0",
		.type = cgltf_attribute_type_color,
		.index = 2,
		.data = &wd.accessors[2]
	};

	wd.material = (cgltf_material){
		.has_pbr_metallic_roughness = 1,
		.pbr_metallic_roughness = {
			.base_color_factor = { 1.0f, 1.0f, 1.0f, 1.0f },
            .roughness_factor = 1.0f
		},
        .alpha_mode = cgltf_alpha_mode_opaque
	};

	wd.primitive = (cgltf_primitive){
		.type = cgltf_primitive_type_triangles,
		.indices = &wd.accessors[0],
		.material = &wd.material,
		.attributes_count = 2,
		.attributes = wd.attributes,
	};

	wd.mesh = (cgltf_mesh){
		.name = "rpgen-mesh",
		.primitives_count = 1,
		.primitives = &wd.primitive
	};

	wd.node_refs[0] = &wd.nodes[0];
	wd.node_refs[1] = &wd.nodes[1];

	// root node
	wd.nodes[0] = (cgltf_node){
        .children_count = 1,
		.children = &wd.node_refs[1]
	};

	// mesh node
	wd.nodes[1] = (cgltf_node){
		.mesh = &wd.mesh
	};

	wd.scene = (cgltf_scene){
		.nodes_count = 1,
		.nodes = &wd.node_refs[0]
	};

	cgltf_options options = {0};
	cgltf_data data = {
		.asset = {
			.generator = "rpgen",
			.version = "2.0"
		},
		.meshes_count = 1,
		.meshes = &wd.mesh,
		.materials_count = 1,
		.materials = &wd.material,
		.accessors_count = 3,
		.accessors = wd.accessors,
		.buffer_views_count = 3,
		.buffer_views = wd.buffer_views,
		.buffers_count = 1,
		.buffers = wd.buffers,
		.nodes_count = 2,
		.nodes = wd.nodes,
		.scenes_count = 1,
		.scenes = &wd.scene,
		.scene = &wd.scene
	};

	cgltf_result result = cgltf_write_file(&options, "out.gltf", &data);
	if (result != cgltf_result_success)
	{
		/* TODO handle error */
		printf("err!! %d", result);
	}
	return 0;
}

