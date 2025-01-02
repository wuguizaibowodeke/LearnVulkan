#pragma once

#include "base.h"
#include "context.h"

namespace ToyEngine
{
	/**
	 * 1 attachment
	 * 	VkAttachmentDescription 描述一个颜色或者深度模板附件的属性，并不是一个真正的附件
	 * 	VkAttachmentReference 描述一个附件的索引，以及使用的图像布局
	 * 	VKSubPass 用于填写一个子pass的描述结构
	 * 	VkSubpassDependency 描述两个子pass之间的依赖关系
	 */
	class Subpass
	{
	 public:
		Subpass();

		~Subpass();

		void addColorAttachmentReference(VkAttachmentReference& ref);

		void addInputAttachmentReference(VkAttachmentReference& ref);

		void setDepthStencilAttachmentReference(VkAttachmentReference& ref);

		void buildSubpassDescription();

		[[nodiscard]] VkSubpassDescription getSubpassDescription() const;

	 private:
		VkSubpassDescription m_subpassDescription{};
		std::vector<VkAttachmentReference> m_colorAttachmentReferences;
		std::vector<VkAttachmentReference> m_inputAttachmentReferences;
		VkAttachmentReference m_depthStencilAttachmentReference{};
	};

	class Renderpass;
	using RenderpassPtr = std::shared_ptr<Renderpass>;
	class Renderpass
	{
	 public:
		static RenderpassPtr create(const VkDevice& device);

		Renderpass(const VkDevice& device);

		~Renderpass();

		void addAttachmentDescription(const VkAttachmentDescription& desc);

		void addSubpass(const Subpass& subpass);

		void addDependency(const VkSubpassDependency& dependency);

		void buildRenderpass();

		[[nodiscard]] VkRenderPass getRenderPass() const;

	 private:
		VkRenderPass m_renderPass{ VK_NULL_HANDLE };
		std::vector<VkAttachmentDescription> m_attachmentDescriptions;
		std::vector<Subpass> m_subpasses;
		std::vector<VkSubpassDependency> m_dependencies;
	};

} // ToyEngine


