#pragma once

#include "GBC/Core/Core.h"
#include <cstdint>
#include <string>
#include <vector>

namespace gbc
{
	enum class BufferUsage
	{
		StreamDraw,  StreamRead,  StreamCopy,
		StaticDraw,  StaticRead,  StaticCopy,
		DynamicDraw, DynamicRead, DynamicCopy
	};

	enum class VertexBufferElementType : uint8_t
	{
		Int, Int2, Int3, Int4,
		UInt, UInt2, UInt3, UInt4,
		Float, Float2, Float3, Float4
	};

	struct VertexBufferElement
	{
		constexpr VertexBufferElement(VertexBufferElementType type, const std::string& name, bool normalized = false) noexcept;

		VertexBufferElementType type;
		std::string name;
		bool normalized;
		uint32_t count;
		uint32_t size;
		uint64_t offset = 0;
	};

	class VertexBufferLayout
	{
	public:
		constexpr VertexBufferLayout() noexcept = default;
		constexpr VertexBufferLayout(std::initializer_list<VertexBufferElement> elements) noexcept;
	public:
		constexpr const std::vector<VertexBufferElement>& GetElements() const noexcept;
		constexpr uint32_t GetStride() const noexcept;
	public:
		constexpr auto begin() noexcept;
		constexpr auto begin() const noexcept;
		constexpr auto end() noexcept;
		constexpr auto end() const noexcept;
		constexpr auto rbegin() noexcept;
		constexpr auto rbegin() const noexcept;
		constexpr auto rend() noexcept;
		constexpr auto rend() const noexcept;
		constexpr auto cbegin() const noexcept;
		constexpr auto cend() const noexcept;
		constexpr auto crbegin() const noexcept;
		constexpr auto crend() const noexcept;
	private:
		std::vector<VertexBufferElement> elements;
		uint32_t stride = 0;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t size) = 0;

		virtual const VertexBufferLayout& GetLayout() const = 0;
		virtual void SetLayout(const VertexBufferLayout& layout) = 0;

		static Ref<VertexBuffer> Create(uint32_t size, const void* data = nullptr, BufferUsage usage = BufferUsage::StaticDraw);
	};

	enum class IndexBufferElementType
	{
		UInt32,
		UInt16,
		UInt8,
	};

	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetData(const void* data, uint32_t count) = 0;

		virtual uint32_t GetCount() const = 0;
		virtual IndexBufferElementType GetType() const = 0;

		static Ref<IndexBuffer> Create(uint32_t count, const void* data = nullptr, BufferUsage usage = BufferUsage::StaticDraw, IndexBufferElementType type = IndexBufferElementType::UInt32);
	};

	class UniformBuffer
	{
	public:
		virtual ~UniformBuffer() = default;

		virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) = 0;

		static Ref<UniformBuffer> Create(uint32_t size, uint32_t binding, const void* data = nullptr, BufferUsage usage = BufferUsage::StaticDraw);
	};
}

#include "Buffer.inl"
