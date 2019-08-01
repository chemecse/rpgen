#define SOKOL_IMPL
#include "sokol_app.h"
#include "sokol_gfx.h"

#include "../../rp_gen.h"
#include "watt_math.h"
#include "demo.glsl.h"

#define MIN_FACET_COUNT 3
#define MAX_FACET_COUNT 23
#define MESH_COUNT (MAX_FACET_COUNT - MIN_FACET_COUNT)

#define SAMPLE_COUNT 4

static sg_pipeline pip;
static sg_bindings bindings;
static sg_buffer vbufs[MESH_COUNT];
static sg_buffer ibufs[MESH_COUNT];

static float rx, ry;
static int32_t g_facet_count = MIN_FACET_COUNT;

static void bind_buffers_to_pipeline(void) {
	bindings = (sg_bindings) {
		.vertex_buffers[0] = vbufs[g_facet_count - MIN_FACET_COUNT],
		.index_buffer = ibufs[g_facet_count - MIN_FACET_COUNT]
	};
}

#ifdef EMSCRIPTEN
EMSCRIPTEN_KEEPALIVE
#endif
int32_t increase_facets(void) {
	++g_facet_count;
	if (g_facet_count >= MAX_FACET_COUNT) g_facet_count = MAX_FACET_COUNT - 1;
	bind_buffers_to_pipeline();
	return g_facet_count;
}

#ifdef EMSCRIPTEN
EMSCRIPTEN_KEEPALIVE
#endif
int32_t decrease_facets(void) {
	--g_facet_count;
	if (g_facet_count < MIN_FACET_COUNT) g_facet_count = MIN_FACET_COUNT;
	bind_buffers_to_pipeline();
	return g_facet_count;
}

static void gen_polygon_buffers(void) {
	for (int32_t i = 0; i < MESH_COUNT; ++i) {
		int32_t facet_count = i + MIN_FACET_COUNT;

		size_t vertex_element_count = RP_GET_VERTEX_ELEMENT_COUNT(facet_count);
		size_t index_element_count = RP_GET_INDEX_ELEMENT_COUNT(facet_count);
		float *vertices = calloc(vertex_element_count, sizeof(float));
		uint16_t *indices = calloc(index_element_count, sizeof(uint16_t));

		rp_gen(&(struct rp_data){
			.vertices = vertices,
			.indices = indices,
			.facet_count = facet_count,
			.facet_radius = 2.0f,
			.extrusion_depth = 0.3f
		});

		vbufs[i] = sg_make_buffer(&(sg_buffer_desc){
			.size = vertex_element_count * sizeof(float),
			.content = vertices,
			.label = "rp-vertices"
		});

		ibufs[i] = sg_make_buffer(&(sg_buffer_desc){
			.type = SG_BUFFERTYPE_INDEXBUFFER,
			.size = index_element_count * sizeof(uint16_t),
			.content = indices,
			.label = "rp-indices"
		});
	}
	return;
}

static void init(void) {
	sg_setup(&(sg_desc){
		.gl_force_gles2 = sapp_gles2(),
		.mtl_device = sapp_metal_get_device(),
		.mtl_renderpass_descriptor_cb = sapp_metal_get_renderpass_descriptor,
		.mtl_drawable_cb = sapp_metal_get_drawable
	});

	/* generate polygon buffers */
	gen_polygon_buffers();

	/* create shader */
	sg_shader shd = sg_make_shader(demo_shader_desc());

	/* create pipeline object */
	pip = sg_make_pipeline(&(sg_pipeline_desc){
		.layout = {
			.buffers[0].stride = 28,
			.attrs = {
				[ATTR_vs_position].format = SG_VERTEXFORMAT_FLOAT3,
				[ATTR_vs_color0].format   = SG_VERTEXFORMAT_FLOAT4
			}
		},
		.shader = shd,
		.index_type = SG_INDEXTYPE_UINT16,
		.depth_stencil = {
			.depth_compare_func = SG_COMPAREFUNC_LESS_EQUAL,
			.depth_write_enabled = true,
		},
		.rasterizer.cull_mode = SG_CULLMODE_BACK,
		.rasterizer.sample_count = SAMPLE_COUNT,
		.label = "rp-pipeline"
	});

	bind_buffers_to_pipeline();
}

static void event(const sapp_event* e) {
	assert((e->type >= 0) && (e->type < _SAPP_EVENTTYPE_NUM));
	if (e->type == SAPP_EVENTTYPE_KEY_UP) {
		if (e->key_code == SAPP_KEYCODE_RIGHT) {
			increase_facets();
		} else if (e->key_code == SAPP_KEYCODE_LEFT) {
			decrease_facets();
		}
	}
}

static void frame(void) {
	/* NOTE: the vs_params_t struct has been code-generated by the shader-code-gen */
	vs_params_t vs_params;
	const float w = (float) sapp_width();
	const float h = (float) sapp_height();
	struct mat4 proj = mat4_perspective(60.0f / 180.0f * WATT_PI32, w/h, 0.01f, 100.0f);
	struct mat4 view = mat4_look_at(v3(0.0f, 0.0f, 10.0f), v3(0.0f, 0.0f, -1.0f), v3(0.0f, 1.0f, 0.0f));
	struct mat4 view_proj = mat4_multiply(proj, view);

	rx += 1.0f;
	ry += 2.0f;

	struct mat4 scaled = mat4_scale(mat4_identity(), v3(1.0f, 1.0f, 1.0f));
	struct mat4 rotated_and_scaled = mat4_rotate_z(
		mat4_rotate_y(
			mat4_rotate_x(
				scaled,
				WATT_RAD_FROM_DEG(rx)
			),
			WATT_RAD_FROM_DEG(ry)
		),
		0.0f
	);
	struct mat4 model = mat4_translate(rotated_and_scaled, v3(0.0f, 0.0f, 0.0f));
	vs_params.mvp = mat4_multiply(view_proj, model);

	sg_pass_action pass_action = {
		.colors[0] = {
			.action = SG_ACTION_CLEAR,
			.val = { 0.25f, 0.5f, 0.75f, 1.0f }
		}
	};
	sg_begin_default_pass(&pass_action, (int)w, (int)h);
	sg_apply_pipeline(pip);
	sg_apply_bindings(&bindings);
	sg_apply_uniforms(SG_SHADERSTAGE_VS, SLOT_vs_params, &vs_params, sizeof(vs_params));
	sg_draw(0, RP_GET_INDEX_ELEMENT_COUNT(g_facet_count), 1);
	sg_end_pass();
	sg_commit();
}

void cleanup(void) {
	sg_shutdown();
}

sapp_desc sokol_main(int argc, char* argv[]) {
	return (sapp_desc){
		.init_cb = init,
		.event_cb = event,
		.frame_cb = frame,
		.cleanup_cb = cleanup,
		.width = 800,
		.height = 600,
		.sample_count = SAMPLE_COUNT,
		.gl_force_gles2 = true,
		.window_title = "rp demo",
	};
}

