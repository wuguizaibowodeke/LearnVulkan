#pragma once

#include "base.h"
#include "context.h"

namespace ToyEngine
{
	class Shader;
	using ShaderPtr = std::shared_ptr<Shader>;
	class Shader
	{
	 public:
		static ShaderPtr create(const VkDevice& device,
			const std::string& vertexShaderPath,
			const std::string& entryPoint,
			VkShaderStageFlagBits stage);

		Shader(const VkDevice& device,
			const std::string& vertexShaderPath,
			const std::string& entryPoint,
			VkShaderStageFlagBits stage);

		~Shader();

		[[nodiscard]] VkShaderModule getShaderModule() const;

		[[nodiscard]] const std::string& getEntryPoint() const;

		[[nodiscard]] VkShaderStageFlagBits getStage() const;

	 private:
		VkShaderModule m_shaderModule{ VK_NULL_HANDLE };
		std::string m_entryPoint;
		VkShaderStageFlagBits m_stage;
	};

} // ToyEngine


