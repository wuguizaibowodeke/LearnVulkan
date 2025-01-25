#include "model.h"

namespace ToyEngine
{
	ModelPtr Model::create()
	{
		return std::make_shared<Model>();
	}

	Model::Model()
	{
		m_Datas = {
			{{0.0f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{{0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
			{{-0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}
		};

	}

	Model::~Model()
	{

	}

	std::vector<VkVertexInputBindingDescription> Model::getBindingDescription() const
	{
		std::vector<VkVertexInputBindingDescription> bindingDescriptions{};
		bindingDescriptions.resize(1);
		bindingDescriptions[0].binding = 0;
		bindingDescriptions[0].stride = sizeof(Vertex);
		bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		return bindingDescriptions;
	}

	std::vector<VkVertexInputAttributeDescription> Model::getAttributeDescription() const
	{
		std::vector<VkVertexInputAttributeDescription> attributeDescriptions{};
		attributeDescriptions.resize(2);

		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(Vertex, color);

		return attributeDescriptions;
	}

	std::vector<Vertex> Model::getDatas() const
	{
		return m_Datas;
	}
} // ToyEngine