#include "renderpass.h"
#include "logger.h"

namespace ToyEngine
{
	Subpass::Subpass()
	{

	}

	Subpass::~Subpass()
	{

	}

	void Subpass::addColorAttachmentReference(VkAttachmentReference& ref)
	{
		m_colorAttachmentReferences.push_back(ref);
	}

	void Subpass::addInputAttachmentReference(VkAttachmentReference& ref)
	{
		m_inputAttachmentReferences.push_back(ref);
	}

	void Subpass::setDepthStencilAttachmentReference(VkAttachmentReference& ref)
	{
		m_depthStencilAttachmentReference = ref;
	}

	void Subpass::buildSubpassDescription()
	{
		if (m_colorAttachmentReferences.empty())
		{
			LOG_E("Color attachment reference is empty.");
			throw std::runtime_error("Color attachment reference is empty.");
		}

		m_subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		m_subpassDescription.colorAttachmentCount = static_cast<uint32_t>(m_colorAttachmentReferences.size());
		m_subpassDescription.pColorAttachments = m_colorAttachmentReferences.data();
		m_subpassDescription.inputAttachmentCount = static_cast<uint32_t>(m_inputAttachmentReferences.size());

		if (m_inputAttachmentReferences.empty())
		{
			m_subpassDescription.pInputAttachments = nullptr;
		}
		else
		{
			m_subpassDescription.pInputAttachments = m_inputAttachmentReferences.data();
		}

		//m_subpassDescription.pDepthStencilAttachment = &m_depthStencilAttachmentReference;
		m_subpassDescription.pDepthStencilAttachment = nullptr;
	}

	VkSubpassDescription Subpass::getSubpassDescription() const
	{
		return m_subpassDescription;
	}

	RenderpassPtr Renderpass::create(VkDevice const& device)
	{
		return std::make_shared<Renderpass>(device);
	}

	Renderpass::Renderpass(VkDevice const& device)
	{

	}

	Renderpass::~Renderpass()
	{
		if (m_renderPass != VK_NULL_HANDLE)
		{
			vkDestroyRenderPass(vkContext.vk_device, m_renderPass, nullptr);
		}
	}

	void Renderpass::addAttachmentDescription(const VkAttachmentDescription& desc)
	{
		m_attachmentDescriptions.push_back(desc);
	}

	void Renderpass::addSubpass(const Subpass& subpass)
	{
		m_subpasses.push_back(subpass);
	}

	void Renderpass::addDependency(const VkSubpassDependency& dependency)
	{
		m_dependencies.push_back(dependency);
	}

	void Renderpass::buildRenderpass()
	{
		if (m_subpasses.empty() || m_attachmentDescriptions.empty() || m_dependencies.empty())
		{
			LOG_E("Subpass, attachment descriptions or dependencies is empty.");
			throw std::runtime_error("Not enough element to build renderpass.");
		}

		//upwrap
		std::vector<VkSubpassDescription> subpassDescriptions;
		for (int i = 0; i < m_subpasses.size(); i++)
		{
			subpassDescriptions.push_back(m_subpasses[i].getSubpassDescription());
		}

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(m_attachmentDescriptions.size());
		renderPassInfo.pAttachments = m_attachmentDescriptions.data();
		renderPassInfo.dependencyCount = static_cast<uint32_t>(m_dependencies.size());
		renderPassInfo.pDependencies = m_dependencies.data();
		renderPassInfo.subpassCount = static_cast<uint32_t>(subpassDescriptions.size());
		renderPassInfo.pSubpasses = subpassDescriptions.data();

		VkDevice device = vkContext.vk_device;
		if (vkCreateRenderPass(vkContext.vk_device, &renderPassInfo, nullptr, &m_renderPass) != VK_SUCCESS)
		{
			LOG_E("Failed to create render pass.");
			throw std::runtime_error("Failed to create render pass.");
		}
		else
		{
			LOG_I("Renderpass created successfully.");
		}
	}

	VkRenderPass Renderpass::getRenderPass() const
	{
		return m_renderPass;
	}
} // ToyEngine