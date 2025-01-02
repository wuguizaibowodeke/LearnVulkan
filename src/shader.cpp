#include "shader.h"

namespace ToyEngine
{
	static std::vector<char> readFile(const std::string& filename)
	{
		std::ifstream file(filename.c_str(),std::ios::ate | std::ios::binary | std::ios::in);

		if(!file)
		{
			throw std::runtime_error("Failed to open file.");
		}

		const size_t fileSize = file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);
		file.close();

		return buffer;
	}

	ShaderPtr Shader::create(const VkDevice& device,
		const std::string& vertexShaderPath,
		const std::string& entryPoint,
		VkShaderStageFlagBits stage)
	{
		return std::make_shared<Shader>(device, vertexShaderPath, entryPoint, stage);
	}

	Shader::Shader(const VkDevice& device,
		const std::string& vertexShaderPath,
		const std::string& entryPoint,
		VkShaderStageFlagBits stage)
	{
		m_stage = stage;
		m_entryPoint = entryPoint;

		std::vector<char> codeBuffer = readFile(vertexShaderPath);
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = codeBuffer.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(codeBuffer.data());

		if(vkCreateShaderModule(device, &createInfo, nullptr, &m_shaderModule) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create shader module.");
		}
	}

	Shader::~Shader()
	{
		if(m_shaderModule != VK_NULL_HANDLE)
		{
			vkDestroyShaderModule(vkContext.vk_device, m_shaderModule, nullptr);
		}
	}

	VkShaderModule Shader::getShaderModule() const
	{
		return m_shaderModule;
	}

	const std::string& Shader::getEntryPoint() const
	{
		return m_entryPoint;
	}

	VkShaderStageFlagBits Shader::getStage() const
	{
		return m_stage;
	}
} // ToyEngine