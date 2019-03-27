//------------------------------------------------------------------------------
//  quad-glfw.c
//  Indexed drawing, explicit vertex attr locations.
//------------------------------------------------------------------------------
#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "flextgl/flextGL.h"
#define SOKOL_IMPL
#define SOKOL_GLCORE33
#include "sokol_gfx.h"

#define GLSL33(x) "#version 330\n" #x

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWwindow* w = glfwCreateWindow(640, 480, "Sokol Quad GLFW", 0, 0);
	glfwMakeContextCurrent(w);
	glfwSwapInterval(1);
	flextInit();

	sg_desc desc = {};
	sg_setup(&desc);
	assert(sg_isvalid());

	float vertices[] = {
		-0.5f,  0.5f,	-1.0f,  1.0f,
		 0.5f,  0.5f,	 1.0f,  1.0f,
		 0.5f, -0.5f,	 1.0f, -1.0f,
		-0.5f, -0.5f,	-1.0f, -1.0f,
	};
	
	sg_buffer vbuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(vertices),
		.content = vertices,
	});

	uint16_t indices[] = {0, 1, 2, 0, 2, 3};
	
	sg_buffer ibuf = sg_make_buffer(&(sg_buffer_desc){
		.size = sizeof(indices),
		.type = SG_BUFFERTYPE_INDEXBUFFER,
		.content = indices,
	});

	sg_bindings bind = {
		.vertex_buffers[0] = vbuf,
		.index_buffer = ibuf
	};

	sg_shader shd = sg_make_shader(&(sg_shader_desc){
		.vs.source = GLSL33(
			layout(location = 0) in vec4 IN_position;
			layout(location = 1) in vec2 IN_uv;
			out vec2 uv;
			void main(){
				gl_Position = IN_position;
				uv = IN_uv;
			}
		),
		.fs.source = GLSL33(
			in vec2 uv;
			out vec4 OUT_color;
			void main(){
				float len = length(uv);
				float mask = smoothstep(1 - fwidth(len), 1, len);
				OUT_color = vec4(mask, mask, mask, 1);
			}
		),
	});
	
	sg_pipeline pip = sg_make_pipeline(&(sg_pipeline_desc){
		.shader = shd,
		.index_type = SG_INDEXTYPE_UINT16,
		.layout = {
			.attrs = {
				[0] = {.offset = 0, .format = SG_VERTEXFORMAT_FLOAT2},
				[1] = {.offset = 8, .format = SG_VERTEXFORMAT_FLOAT2},
			}
		}
	});

	sg_pass_action pass_action = {};

	while (!glfwWindowShouldClose(w)) {
		int cur_width, cur_height;
		glfwGetFramebufferSize(w, &cur_width, &cur_height);
		sg_begin_default_pass(&pass_action, cur_width, cur_height);
		sg_apply_pipeline(pip);
		sg_apply_bindings(&bind);
		sg_draw(0, 6, 1);
		sg_end_pass();
		sg_commit();
		glfwSwapBuffers(w);
		glfwPollEvents();
	}

	sg_shutdown();
	glfwTerminate();

	return 0;
}
