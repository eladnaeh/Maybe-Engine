#pragma once

#include "Window.h"
#include "Buffer.h"
#include "VertexArray.h"
#include "IndexBuffer.h"
#include "Shader.h"
#include "Sprite2D.h"
#include "Texture.h"

#include "..\utils\utils.h"
#include "..\platform\Entity.h"

#define MAX_TEXTURES 32

namespace mb { namespace graphics {

	struct VertexData
	{
		//maths::Vec3 position;
		float positionX, positionY, positionZ;
		maths::Vec2 uv;
		float tid;
		maths::Vec3 color;
	};

	class BatchRenderer
	{
	public:
		BatchRenderer(const Window& window);
		~BatchRenderer();

		void Begin();
		void Submit(const Sprite2D* sprite);
		void Submit(platform::Entity& entity);
		void End();
	private:
		void Flush();
	private:
		//unsigned int m_VAO;// , m_IBO;
		VertexArray m_VAO;
		BufferLayout m_VBOLayout;
		Buffer* m_VBO;

		//unsigned int m_VAO, m_VBO;

		IndexBuffer* m_IBO;
		VertexData* m_Buffer;
		Shader m_Shader;

		maths::Mat4 m_View, m_Proj;

		unsigned int m_SpriteCount;

		utils::Timer m_Timer;
		const Window& window;

		maths::Vec2 defaultUV[4];

		Texture* m_BoundTextures[MAX_TEXTURES];
		unsigned int m_TextureCount;
	};

} }