namespace gbc
{
	static constexpr uint32_t GetElementCount(VertexBufferElementType type)
	{
		switch (type)
		{
			case VertexBufferElementType::Int:    return 1;
			case VertexBufferElementType::Int2:   return 2;
			case VertexBufferElementType::Int3:   return 3;
			case VertexBufferElementType::Int4:   return 4;
			case VertexBufferElementType::UInt:   return 1;
			case VertexBufferElementType::UInt2:  return 2;
			case VertexBufferElementType::UInt3:  return 3;
			case VertexBufferElementType::UInt4:  return 4;
			case VertexBufferElementType::Float:  return 1;
			case VertexBufferElementType::Float2: return 2;
			case VertexBufferElementType::Float3: return 3;
			case VertexBufferElementType::Float4: return 4;
		}

		GBC_CORE_ASSERT(false, "Unknown Vertex Buffer Element Type!");
		return 0;
	}

	static constexpr uint32_t GetElementSize(VertexBufferElementType type)
	{
		switch (type)
		{
			case VertexBufferElementType::Int:    return 4;
			case VertexBufferElementType::Int2:   return 8;
			case VertexBufferElementType::Int3:   return 12;
			case VertexBufferElementType::Int4:   return 16;
			case VertexBufferElementType::UInt:   return 4;
			case VertexBufferElementType::UInt2:  return 8;
			case VertexBufferElementType::UInt3:  return 12;
			case VertexBufferElementType::UInt4:  return 16;
			case VertexBufferElementType::Float:  return 4;
			case VertexBufferElementType::Float2: return 8;
			case VertexBufferElementType::Float3: return 12;
			case VertexBufferElementType::Float4: return 16;
		}

		GBC_CORE_ASSERT(false, "Unknown Vertex Buffer Element Type!");
		return 0;
	}

	constexpr VertexBufferElement::VertexBufferElement(VertexBufferElementType type, const std::string& name, bool normalized) noexcept
		: type(type), name(name), normalized(normalized), count(GetElementCount(type)), size(GetElementSize(type)) {}

	constexpr VertexBufferLayout::VertexBufferLayout(std::initializer_list<VertexBufferElement> elements) noexcept
		: elements(elements)
	{
		uint64_t offset = 0;

		for (VertexBufferElement& element : this->elements)
		{
			element.offset = offset;
			offset += element.size;
			stride += element.size;
		}
	}

	constexpr const std::vector<VertexBufferElement>& VertexBufferLayout::GetElements() const noexcept { return elements; }
	constexpr uint32_t VertexBufferLayout::GetStride() const noexcept { return stride; }

	constexpr auto VertexBufferLayout::begin() noexcept { return elements.begin(); }
	constexpr auto VertexBufferLayout::begin() const noexcept { return elements.begin(); }
	constexpr auto VertexBufferLayout::end() noexcept { return elements.end(); }
	constexpr auto VertexBufferLayout::end() const noexcept { return elements.end(); }
	constexpr auto VertexBufferLayout::rbegin() noexcept { return elements.rbegin(); }
	constexpr auto VertexBufferLayout::rbegin() const noexcept { return elements.rbegin(); }
	constexpr auto VertexBufferLayout::rend() noexcept { return elements.rend(); }
	constexpr auto VertexBufferLayout::rend() const noexcept { return elements.rend(); }
	constexpr auto VertexBufferLayout::cbegin() const noexcept { return elements.cbegin(); }
	constexpr auto VertexBufferLayout::cend() const noexcept { return elements.cend(); }
	constexpr auto VertexBufferLayout::crbegin() const noexcept { return elements.crbegin(); }
	constexpr auto VertexBufferLayout::crend() const noexcept { return elements.crend(); }

	static constexpr uint32_t GetIndexBufferElementSize(IndexBufferElementType type)
	{
		switch (type)
		{
			case IndexBufferElementType::UInt32: return sizeof(uint32_t);
			case IndexBufferElementType::UInt16: return sizeof(uint16_t);
			case IndexBufferElementType::UInt8:  return sizeof(uint8_t);
		}

		GBC_CORE_ASSERT(false, "Unknown Index Buffer Element Type!");
		return 0;
	}
}
