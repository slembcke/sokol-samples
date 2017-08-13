//------------------------------------------------------------------------------
//  triangle-metal.c
//------------------------------------------------------------------------------
#include "osxentry.h"
#include "sokol_gfx.h"

sg_pass_action pass_action = {0};
sg_draw_state draw_state = {0};

void init(const void* mtl_device) {
    /* setup sokol */
    sg_desc desc = {
        .mtl_device = mtl_device,
        .mtl_renderpass_descriptor_cb = osx_mtk_get_render_pass_descriptor,
        .mtl_drawable_cb = osx_mtk_get_drawable
    };
    sg_setup(&desc);

    /* a vertex buffer with 3 vertices */
    float vertices[] = {
        // positions        colors
         0.0f, 0.5f, 0.5f,  1.0f, 0.0f, 0.0f, 1.0f,
         0.5f, -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f
    };
    draw_state.vertex_buffers[0] = sg_make_buffer(&(sg_buffer_desc){
        .size = sizeof(vertices),
        .data_ptr = vertices,
        .data_size = sizeof(vertices)
    });

    /* a shader pair, compiled from source code */
    sg_shader shd = sg_make_shader(&(sg_shader_desc){
        .vs.func = "vs_main",
        .vs.source =
            "#include <metal_stdlib>\n"
            "#include <simd/simd.h>\n"
            "using namespace metal;\n"
            "struct vs_in {\n"
            "  float4 position [[attribute(0)]];\n"
            "  float4 color [[attribute(1)]];\n"
            "};\n"
            "struct vs_out {\n"
            "  float4 color [[user(locn0)]];\n"
            "  float4 position [[position]];\n"
            "};\n"
            "vertex vs_out vs_main(vs_in in [[stage_in]]) {\n"
            "  vs_out out;\n"
            "  out.position = in.position;\n"
            "  out.color = in.color;\n"
            "  return out;\n"
            "}\n",
        .fs.func = "fs_main",
        .fs.source =
            "#include <metal_stdlib>\n"
            "#include <simd/simd.h>\n"
            "using namespace metal;\n"
            "struct fs_in {\n"
            "  float4 color [[user(locn0)]];\n"
            "};\n"
            "struct fs_out {\n"
            "  float4 color [[color(0)]];\n"
            "};\n"
            "fragment fs_out fs_main(fs_in in [[stage_in]]) {\n"
            "  fs_out out;\n"
            "  out.color = in.color;\n"
            "  return out;\n"
            "}\n"
    });

    /* create a pipeline object */
    draw_state.pipeline = sg_make_pipeline(&(sg_pipeline_desc){
        .vertex_layouts[0] = {
            .stride = 28,
            .attrs = {
                [0] = { .offset=0, .format=SG_VERTEXFORMAT_FLOAT3 },
                [1] = { .offset=12, .format=SG_VERTEXFORMAT_FLOAT4 }
            },
        },
        .shader = shd
    });
}

void frame() {
    sg_begin_default_pass(&pass_action, osx_width(), osx_height());
    /*
    sg_apply_draw_state(&draw_state);
    sg_draw(0, 3, 1);
    */
    sg_end_pass();
    sg_commit();
}

void shutdown() {
    sg_shutdown();
}

int main() {
    osx_start(640, 480, 1, "Sokol Triangle (Metal)", init, frame, shutdown);
    return 0;
}
