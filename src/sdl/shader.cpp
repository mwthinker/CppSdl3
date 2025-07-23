#include "shader.h"
#include "shader.ps.h"
#include "shader.vs.h"
#include "sdlexception.h"

namespace sdl {

	void Shader::load(sdl::gpu::GpuContext& context) {
		SDL_GPUShaderCreateInfo vxCreateInfo{
			.entrypoint = "main",
			.stage = SDL_GPU_SHADERSTAGE_VERTEX,
			.num_samplers = 0,
			.num_storage_textures = 0,
			.num_storage_buffers = 0,
			.num_uniform_buffers = 1
		};
		SDL_GPUShaderCreateInfo pxCreateInfo{
			.entrypoint = "main",
			.stage = SDL_GPU_SHADERSTAGE_FRAGMENT,
			.num_samplers = 1,
			.num_storage_textures = 0,
			.num_storage_buffers = 0,
			.num_uniform_buffers = 0
		};

		auto driver = SDL_GetGPUDeviceDriver(context.getGpuDevice());
		if (strcmp(driver, "vulkan") == 0) {
			vxCreateInfo.code_size = ShaderVsSpirvBytes.size();
			vxCreateInfo.code = ShaderVsSpirvBytes.data();
			vxCreateInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;

			pxCreateInfo.code_size = ShaderPsSpirvBytes.size();
			pxCreateInfo.code = ShaderPsSpirvBytes.data();
			pxCreateInfo.format = SDL_GPU_SHADERFORMAT_SPIRV;
		} else if (strcmp(driver, "direct3d12") == 0) {
			vxCreateInfo.code_size = ShaderVsDxilBytes.size();
			vxCreateInfo.code = ShaderVsDxilBytes.data();
			vxCreateInfo.format = SDL_GPU_SHADERFORMAT_DXIL;

			pxCreateInfo.code_size = ShaderPsDxilBytes.size();
			pxCreateInfo.code = ShaderPsDxilBytes.data();
			pxCreateInfo.format = SDL_GPU_SHADERFORMAT_DXIL;
		} else {
			throw sdl::SdlException("[Shader] Unsupported GPU driver for shader loading '{}'", driver);
		}
		vertexShader = createShader(context, vxCreateInfo);
		fragmentShader = createShader(context, pxCreateInfo);
	}

	void Shader::pushProjectionMatrix(SDL_GPUCommandBuffer* commandBuffer, const glm::mat4& modelMatrix) {
		// Maps to b0 in fragment shader
		SDL_PushGPUVertexUniformData(commandBuffer, 0, &modelMatrix, sizeof(modelMatrix));
	}

}
