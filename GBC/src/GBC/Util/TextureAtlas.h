#pragma once

#include "GBC/Rendering/LocalTexture2D.h"
#include <glm/glm.hpp>

namespace gbc
{
	class TextureAtlas
	{
	public:
		TextureAtlas();
		~TextureAtlas();

		// Returns the texture coordinates, in pixel space, of the texture atlas.
		// These coordinates are for the texture returned by Create(). After you
		// have added all of your textures, before you even call Create(), you
		// can get normalized texture coordinates by dividing this return value
		// by GetTextureSize(). Don't forget to cast to a float!
		glm::ivec2 AddTexture(const Ref<LocalTexture2D>& texture, int32_t padding = 0);

		// Returns a texture that has the smallest possible size that contains
		// every texture provided through addTexture. The returned textures will
		// have an aspect ratio of 1, and their size will be constrained to
		// powers of 2, e.g.: 1x1, 2x2, 4x4... 256x256, 512x512, 1024x1024...
		Ref<LocalTexture2D> Create(int32_t channels);

		// Clears this texture atlas, removing all references to textures and
		// destroying internal texture atlas representation.
		void Clear();

		constexpr const glm::ivec2& GetTextureSize() const noexcept { return atlasTextureSize; }
	private:
		struct TextureNode
		{
			constexpr TextureNode(const glm::ivec2& position, const glm::ivec2& size) noexcept
				: position(position), size(size) {}

			Ref<LocalTexture2D> texture = nullptr;

			// These are unnormalized texture coordinates and sizes
			glm::ivec2 position;
			glm::ivec2 size;

			TextureNode* left = nullptr;
			TextureNode* right = nullptr;
		};

		TextureNode* AddTexture(TextureNode* node, const Ref<LocalTexture2D>& texture, const glm::ivec2& size);
		void PutTexture(TextureNode* node, Ref<LocalTexture2D>& atlasTexture);
		void Clear(TextureNode* node);

		glm::ivec2 maxTextureSize;
		TextureNode* root;
		glm::ivec2 atlasTextureSize{ 1 };
	};
}
