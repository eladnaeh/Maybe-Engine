#pragma once

#include <iostream>
#include <unordered_map>

#include "../maths/maths.h"

namespace mb { namespace graphics {
	class Shader 
	{
	public:
		Shader(const char* vertexFile, const char* fragmentFile);
		~Shader();

		void Bind() const;
		void Unbind() const;

		inline unsigned int GetID() const { return m_ID; }

		void SetUniform3f(const std::string& name, float v0, float v1, float v2);
		void SetUniformVec3(const std::string& name, const maths::Vec3& v);
	private:
		int GetUniformLocation(const std::string& name);
	private:
		std::unordered_map<std::string, int> m_UniformLocationsCache;
		std::string m_VertexSource, m_FragmentSource;
		unsigned int m_ID;
	};
} }