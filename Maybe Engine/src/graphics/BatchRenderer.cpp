#include "BatchRenderer.h"

#define MAX_SPRITES 60000
#define VERTEX_SIZE sizeof(float) * 9
#define BUFFER_SIZE VERTEX_SIZE * 4 * MAX_SPRITES
#define INDICES_SIZE MAX_SPRITES * 6
#define INDEX_BUFFER_SIZE INDICES_SIZE * sizeof(unsigned int)

namespace mb { namespace graphics {

	BatchRenderer::BatchRenderer(const Window& window)
		: m_VBOLayout(), m_VAO(), m_Shader("./res/batcher.vert", "./res/batcher.frag"),
		m_SpriteCount(0), m_TextureCount(0), m_View(1), m_Proj(1), window(window)
	{

		//GLCall(glGenBuffers(1, &m_VBO));
		//GLCall(glGenVertexArrays(1, &m_VAO));

		m_VBOLayout.Push<float>(3);
		m_VBOLayout.Push<float>(2);
		m_VBOLayout.Push<float>(1);
		m_VBOLayout.Push<float>(3);

		m_VBO = new Buffer(NULL, BUFFER_SIZE, GL_DYNAMIC_DRAW);
		m_VBO->SetLayout(m_VBOLayout);

		/*GLCall(glBindVertexArray(m_VAO));

		GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
		GLCall(glBufferData(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW));

		GLCall(glEnableVertexAttribArray(0));
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*) 0));
		GLCall(glEnableVertexAttribArray(1));
		GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*) (3 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(2));
		GLCall(glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*) (5 * sizeof(float))));
		GLCall(glEnableVertexAttribArray(3));
		GLCall(glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_SIZE, (const void*) (6 * sizeof(float))));

		GLCall(glBindVertexArray(0));
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0));*/

		unsigned int* indices = new unsigned int[INDICES_SIZE];
		unsigned int offset = 0;

		for (unsigned int i = 0; i < INDICES_SIZE; i += 6)
		{
			indices[  i  ] = offset;
			indices[i + 1] = offset + 1;
			indices[i + 2] = offset + 2;
			indices[i + 3] = offset + 2;
			indices[i + 4] = offset + 3;
			indices[i + 5] = offset + 0;

			offset += 4;
		}

		m_IBO = new IndexBuffer(indices, INDEX_BUFFER_SIZE);

		m_VAO.AddBuffer(m_VBO);

		delete indices;

		m_Proj = maths::Mat4::ortho(-window.GetSize().x / 2, window.GetSize().x / 2, -window.GetSize().y / 2, window.GetSize().y / 2, -5, 5);

		m_Shader.Bind();
		m_Shader.SetUniformMat4("u_View", m_View);
		m_Shader.SetUniformMat4("u_Proj", m_Proj);

		maths::Vec3 lightColor(0, 0, 0);
		maths::Vec3 lightPosition(0, 0, 0.025);

		m_Shader.SetUniformVec3("u_LightColor", lightColor);
		m_Shader.SetUniformVec3("u_LightPos", lightPosition);

		defaultUV[0] = {0, 0};
		defaultUV[1] = {0, 1};
		defaultUV[2] = {1, 1};
		defaultUV[3] = {1, 0};

		for (unsigned int i = 0; i < 32; i++)
		{
			std::stringstream ss;
			ss << "u_Textures[" << i << "]";
			m_Shader.SetUniform1i(ss.str(), i);
		}
	}

	void BatchRenderer::Begin()
	{
		GLCall(m_Buffer = (VertexData*) glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
	}

	void BatchRenderer::Submit(const Sprite2D* sprite, const maths::Vec2& position, const maths::Vec2& scale, const RotationData& rotation)
	{
		const maths::Vec2& size = sprite->GetSize();
		const maths::Vec3& color = sprite->GetColor();

		int tid = 0;
		bool existingTexture = false;

		if (sprite->HasTexture())
		{
			for (unsigned int i = 0; i < m_TextureCount; i++)
			{
				if (sprite->GetTexture() == m_BoundTextures[i])
				{
					tid = i;
					existingTexture = true;
					break;
				}
			}

			if (!existingTexture)
			{
				if (m_TextureCount >= 32)
				{
					End();
					Begin();
				}

				m_BoundTextures[m_TextureCount] = sprite->GetTexture();
				tid = m_TextureCount;
				m_TextureCount++;
			}
		}
		else
		{
			tid = -1;
		}

		const float realTID = (float)tid / 32.0f;

		maths::Mat4 modelMat;
		modelMat = maths::Mat4::translate(modelMat, maths::Vec3(position, 0));
		if(scale.x != 1 || scale.y != 1) modelMat = maths::Mat4::scale(modelMat, maths::Vec3(scale, 0));
		if(rotation.angle != 0) modelMat = maths::Mat4::rotate(modelMat, maths::radians(rotation.angle), rotation.axis);

		maths::Vec4 vertices[4] = {
			{ -size.x / 2, -size.y / 2, 0, 1 },
			{ -size.x / 2, size.y / 2, 0, 1 },
			{ size.x / 2, size.y / 2, 0, 1 },
			{ size.x / 2, -size.y / 2, 0, 1 }
		};

		for (unsigned int i = 0; i < 4; i++)
			vertices[i] *= modelMat;

		m_Buffer->positionX = vertices[0].x;
		m_Buffer->positionY = vertices[0].y;
		m_Buffer->color = color;
		m_Buffer->uv = defaultUV[0];
		m_Buffer->tid = realTID;
		m_Buffer++;

		m_Buffer->positionX = vertices[1].x;
		m_Buffer->positionY = vertices[1].y;
		m_Buffer->color = color;
		m_Buffer->uv = defaultUV[1];
		m_Buffer->tid = realTID;
		m_Buffer++;

		m_Buffer->positionX = vertices[2].x;
		m_Buffer->positionY = vertices[2].y;
		m_Buffer->color = color;
		m_Buffer->uv = defaultUV[2];
		m_Buffer->tid = realTID;
		m_Buffer++;

		m_Buffer->positionX = vertices[3].x;
		m_Buffer->positionY = vertices[3].y;
		m_Buffer->color = color;
		m_Buffer->uv = defaultUV[3];
		m_Buffer->tid = realTID;
		m_Buffer++;

		m_SpriteCount++;
	}

	void BatchRenderer::Submit(const Sprite2D* sprite)
	{
		Submit(sprite, sprite->transform.position, sprite->transform.scale, { sprite->transform.rotationAxis, sprite->transform.rotationAngle });
	}

	void BatchRenderer::Submit(platform::Entity& entity)
	{
		const auto* render = entity.GetComponent<platform::RenderComponent>();
		const auto* transform = entity.GetComponent<platform::TransformComponent>();

		Submit(&render->sprite, transform->position, transform->scale, { transform->rotationAxis, transform->rotationAngle });
	}

	void BatchRenderer::Submit(platform::TransformComponent* transform, platform::RenderComponent* render)
	{
		Submit(&render->sprite, transform->position, transform->scale, { transform->rotationAxis, transform->rotationAngle });
	}
	
	//void BatchRenderer::Submit(const Light& light)
	//{
	//	m_Shader.SetUniformVec3("u_LightPos", light.GetPosition());
	//	m_Shader.SetUniformVec3("u_LightColor", light.GetColor());

	//	//utils::Log::Debug("Submitting Light with position {}, {}, {}", light.GetPosition().x, light.GetPosition().y, light.GetPosition().z);
	//}

	void BatchRenderer::End()
	{
		GLCall(glUnmapBuffer(GL_ARRAY_BUFFER));
		Flush();
	}

	void BatchRenderer::Flush()
	{
		m_Shader.Bind();

		for (unsigned int i = 0; i < m_TextureCount; i++)
			m_BoundTextures[i]->Bind(i);

		m_VAO.Bind();
		m_IBO->Bind();

		GLCall(glDrawElements(GL_TRIANGLES, 6 * m_SpriteCount, GL_UNSIGNED_INT, NULL));

		m_SpriteCount = 0;
		m_TextureCount = 0;
	}

	BatchRenderer::~BatchRenderer()
	{
		delete m_VBO;
		delete m_IBO;
	}

} }

/*


GLCall(glGenBuffers(1, &m_IBO));
GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO));
GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, INDEX_BUFFER_SIZE, indices, GL_STATIC_DRAW));
GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));


*/