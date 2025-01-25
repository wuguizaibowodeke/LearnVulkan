#include "base.h"
namespace ToyEngine
{
	struct Vertex{
		glm::vec3 pos;
		glm::vec3 color;
	};

	class Model;
	using ModelPtr = std::shared_ptr<Model>;
	class Model
	{
	 public:
		static ModelPtr create();

		Model();

		~Model();

		[[nodiscard]] std::vector<VkVertexInputBindingDescription> getBindingDescription() const;

		[[nodiscard]] std::vector<VkVertexInputAttributeDescription> getAttributeDescription() const;

		[[nodiscard]] std::vector<Vertex> getDatas() const;

	 private:
		std::vector<Vertex> m_Datas{};
	};

} // ToyEngine


