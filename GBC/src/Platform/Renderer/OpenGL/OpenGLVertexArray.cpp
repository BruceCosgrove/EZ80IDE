#include "gbcpch.h"
#include "OpenGLVertexArray.h"
#include <glad/glad.h>

namespace gbc
{
	static constexpr GLenum GetOpenGLElementType(VertexBufferElementType type)
	{
		switch (type)
		{
			case gbc::VertexBufferElementType::Float:
			case gbc::VertexBufferElementType::Float2:
			case gbc::VertexBufferElementType::Float3:
			case gbc::VertexBufferElementType::Float4:
				return GL_FLOAT;
			case gbc::VertexBufferElementType::Int:
			case gbc::VertexBufferElementType::Int2:
			case gbc::VertexBufferElementType::Int3:
			case gbc::VertexBufferElementType::Int4:
				return GL_INT;
			case gbc::VertexBufferElementType::UInt:
			case gbc::VertexBufferElementType::UInt2:
			case gbc::VertexBufferElementType::UInt3:
			case gbc::VertexBufferElementType::UInt4:
				return GL_UNSIGNED_INT;
		}

		GBC_CORE_ASSERT(false, "Unknown Vetex Buffer Element Type!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &rendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &rendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(rendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		GBC_CORE_ASSERT(vertexBuffer != nullptr, "Vertex buffer is nullptr!");
		GBC_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size() != 0, "Vertex buffer has no layout!");

		glBindVertexArray(rendererID);
		vertexBuffer->Bind();
	
		const VertexBufferLayout& layout = vertexBuffer->GetLayout();
		for (const VertexBufferElement& element : layout)
		{
			GLenum elementType = GetOpenGLElementType(element.type);

			switch (element.type)
			{
				case VertexBufferElementType::Float:
				case VertexBufferElementType::Float2:
				case VertexBufferElementType::Float3:
				case VertexBufferElementType::Float4:
					glEnableVertexAttribArray(vertexAttributeIndex);
					glVertexAttribPointer(vertexAttributeIndex, element.count, elementType, element.normalized ? GL_TRUE : GL_FALSE, layout.GetStride(), (const void*)element.offset);
					vertexAttributeIndex++;
					break;
				case VertexBufferElementType::Int:
				case VertexBufferElementType::Int2:
				case VertexBufferElementType::Int3:
				case VertexBufferElementType::Int4:
				case VertexBufferElementType::UInt:
				case VertexBufferElementType::UInt2:
				case VertexBufferElementType::UInt3:
				case VertexBufferElementType::UInt4:
					glEnableVertexAttribArray(vertexAttributeIndex);
					glVertexAttribIPointer(vertexAttributeIndex, element.count, elementType, layout.GetStride(), (const void*)element.offset);
					vertexAttributeIndex++;
					break;
			}
		}

		vertexBuffers.push_back(vertexBuffer);
	}
}
