#include "epch.h"
#include "Render/Texture.h"
#include "Core/E_Assert.h"
#include "Render/Renderer.h"
#include "Render/glad/GLTexture.h"
#include "Core/Factory.h"
namespace Engine {
	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height)
	{
		switch (Factory::GetDesc().Graphics_API)
		{
			case GraphicsAPI::None:    E_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::OpenGL:  return CreateRef<GLTexture2D>(width, height);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Factory::GetDesc().Graphics_API)
		{
			case GraphicsAPI::None:    E_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::OpenGL:  return CreateRef<GLTexture2D>(path);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
	Ref<Texture2D> Texture2D::Create(unsigned int id)
	{
		switch (Factory::GetDesc().Graphics_API)
		{
			case GraphicsAPI::None:    E_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::OpenGL:  return CreateRef<GLTexture2D>(id);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(uint32_t width, uint32_t height, GLenum internalFormat, GLenum dataFormat)
	{
		switch (Factory::GetDesc().Graphics_API)
		{
			case GraphicsAPI::None:    E_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case GraphicsAPI::OpenGL:  return CreateRef<GLTexture2D>(width, height, internalFormat, dataFormat);
		}

		E_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	// Ref<Texture2D> Texture2D::Create(Ref<Framebuffer>& buffer)
	// {
	// 	switch (Factory::GetDesc().Graphics_API)
	// 	{
	// 		case GraphicsAPI::None:    E_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
	// 		case GraphicsAPI::OpenGL:  return CreateRef<GLTexture2D>(buffer);
	// 	}

	// 	E_CORE_ASSERT(false, "Unknown RendererAPI!");
	// 	return nullptr;
	// }
}