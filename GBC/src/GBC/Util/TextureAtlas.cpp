#include "gbcpch.h"
#include "TextureAtlas.h"
#include "GBC/Rendering/RendererCapabilities.h"

namespace gbc
{
	TextureAtlas::TextureAtlas()
		: maxTextureSize(RendererCapabilities::GetMaxTextureSize()), root(new TextureNode(glm::ivec2(0), maxTextureSize)) {}

	TextureAtlas::~TextureAtlas()
	{
		Clear(root);
	}

	glm::ivec2 TextureAtlas::AddTexture(const Ref<LocalTexture2D>& texture, int32_t padding)
	{
		GBC_PROFILE_FUNCTION();

		glm::ivec2 textureSize{ texture->GetWidth() + padding, texture->GetHeight() + padding };

		while ((textureSize.x < atlasTextureSize.x || textureSize.y < atlasTextureSize.y) ||
			(atlasTextureSize.x < maxTextureSize.x || atlasTextureSize.y < maxTextureSize.y))
		{
			TextureNode* node = AddTexture(root, texture, textureSize);
			if (node != nullptr)
				return node->position;
			else
				atlasTextureSize <<= 1; // Keep the texture size a power of 2
		}

		// If this happens, the texture can not fit on the GPU, this is very bad, but also very unlikely
		return glm::ivec2(-1);
	}

	// From: https://straypixels.net/texture-packing-for-fonts/
	TextureAtlas::TextureNode* TextureAtlas::AddTexture(TextureNode* node, const Ref<LocalTexture2D>& texture, const glm::ivec2& size)
	{
		// If the node has a texture, it can only be a leaf node, so don't bother
		if (node->texture != nullptr)
			return nullptr;
		// If the node has both children, search them for an opening
		else if (node->left != nullptr && node->right != nullptr)
		{
			TextureNode* newLeftNode = AddTexture(node->left, texture, size);
			return newLeftNode != nullptr ? newLeftNode : AddTexture(node->right, texture, size);
		}
		// If an opening has been found
		else
		{
			// Get the real size of the opening
			glm::ivec2 realSize(node->size);
			if (node->position.x + node->size.x == maxTextureSize.x)
				realSize.x = atlasTextureSize.x - node->position.x;
			if (node->position.y + node->size.y == maxTextureSize.y)
				realSize.y = atlasTextureSize.y - node->position.y;

			// If the size of the opening (which may technically be the max texture size) is the same size as the texture
			if (node->size == size)
			{
				node->texture = texture;
				return node;
			}
			// This opening is not big enough
			else if (realSize.x < size.x || realSize.y < size.y)
				return nullptr;
			// This opening is bigger than the texture
			else
			{
				glm::ivec2 remainingSize = realSize - size;
				bool splitVertically = remainingSize.x < remainingSize.y;

				// If the opening is the same size as the texture
				if (remainingSize.x == 0 && remainingSize.y == 0)
					splitVertically = node->size.x <= node->size.y;

				// Get the right position and size of the opening and empty space
				if (splitVertically)
				{
					node->left = new TextureNode(node->position, { node->size.x, size.y });
					node->right = new TextureNode({ node->position.x, node->position.y + size.y }, { node->size.x, node->size.y - size.y });
				}
				else
				{
					node->left = new TextureNode(node->position, { size.x, node->size.y });
					node->right = new TextureNode({ node->position.x + size.x, node->position.y }, { node->size.x - size.x, node->size.y });
				}

				return AddTexture(node->left, texture, size);
			}
		}
	}

	Ref<LocalTexture2D> TextureAtlas::Create(int32_t channels)
	{
		GBC_PROFILE_FUNCTION();

		Ref<LocalTexture2D> atlasTexture = CreateRef<LocalTexture2D>(atlasTextureSize.x, atlasTextureSize.y, channels);
		PutTexture(root, atlasTexture);
		return atlasTexture;
	}

	void TextureAtlas::Clear()
	{
		Clear(root->left);
		root->left = nullptr;
		Clear(root->right);
		root->right = nullptr;
		atlasTextureSize = { 1, 1 };
	}

	void TextureAtlas::PutTexture(TextureNode* node, Ref<LocalTexture2D>& atlasTexture)
	{
		if (node && atlasTexture)
		{
			if (node->texture && *node->texture)
			{
				if (!atlasTexture->SetSubregion(*node->texture, node->position.x, node->position.y))
					atlasTexture = nullptr;
			}
			// Since a node with a texture has no children, don't try to copy their
			// non-existent textures, because they themselves don't exist.
			else
			{
				PutTexture(node->left, atlasTexture);
				PutTexture(node->right, atlasTexture);
			}
		}
	}

	void TextureAtlas::Clear(TextureNode* node)
	{
		if (node)
		{
			Clear(node->left);
			Clear(node->right);
			delete node;
		}
	}
}
