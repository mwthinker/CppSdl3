#ifndef SDL_SHADER_H
#define SDL_SHADER_H

#include "shader.ps.h"
#include "shader.vs.h"

#include <sdl/gpu/sdlgpu.h>
#include <glm/glm.hpp>

#include <array>

namespace sdl {

	VERTEX(Vertex) {
		glm::vec3 position;
		glm::vec2 tex;
		glm::vec4 color;
	};
	VERTEX_VALIDATE(Vertex);

	struct Shader {
		void load(SDL_GPUDevice* gpuDevice);

		static void uploadProjectionMatrix(SDL_GPUCommandBuffer* commandBuffer, const glm::mat4& projection);

		static constexpr std::array<SDL_GPUVertexAttribute, 3> attributes = {
			// position maps to TEXCOORD0
			SDL_GPUVertexAttribute{
				.location = 0,
				.buffer_slot = 0,
				.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT3,
				.offset = offsetof(Vertex, position)
			},
			// tex maps to TEXCOORD1
			SDL_GPUVertexAttribute{
					.location = 1,
					.buffer_slot = 0,
					.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT2,
					.offset = offsetof(Vertex, tex)
			},
			// color maps to TEXCOORD2
			SDL_GPUVertexAttribute{
					.location = 2,
					.buffer_slot = 0,
					.format = SDL_GPU_VERTEXELEMENTFORMAT_FLOAT4,
					.offset = offsetof(Vertex, color)
			}
		};

		sdl::gpu::GpuShader vertexShader;
		sdl::gpu::GpuShader fragmentShader;
	};

}

#endif
