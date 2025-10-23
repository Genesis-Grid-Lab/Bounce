#include "epch.h"
#include "Render/glad/GLTexture.h"
#include "Core/E_Assert.h"
#include <stb_image.h>
#include <glad/glad.h>

// tmp
#include <GLFW/glfw3.h>

namespace Engine {
	GLTexture2D::GLTexture2D(unsigned int id){
		m_RendererID = id;
	}

	GLTexture2D::GLTexture2D(uint32_t width, uint32_t height)
		: m_Width(width), m_Height(height)
	{
		

		m_InternalFormat = GL_RGBA8;
		m_DataFormat = GL_RGBA;

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	GLTexture2D::GLTexture2D(uint32_t width, uint32_t height, GLenum internalFormat, GLenum dataFormat)
		: m_Width(width), m_Height(height),  m_InternalFormat(internalFormat), m_DataFormat(dataFormat)
	{
		

		glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
		glTextureStorage2D(m_RendererID, 1, m_InternalFormat, m_Width, m_Height);

		glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	// GLTexture2D::GLTexture2D(Ref<Framebuffer>& buffer){
		
	// 	E_CORE_INFO("Creating Texture2D from Framebuffer");
	// 	m_RendererID = buffer->GetColorAttachmentRendererID();
	// 	m_Width = buffer->GetSpecification().Width;
	// 	m_Height = buffer->GetSpecification().Height;
	// 	m_IsLoaded = true;
	// }

	GLTexture2D::GLTexture2D(const std::string& path)
		: m_Path(path)
	{
		

		int width, height, channels;
		stbi_set_flip_vertically_on_load(0);
		stbi_uc* data = nullptr;
		{						
			data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		}
			
		if (data)
		{
			m_IsLoaded = true;

			m_Width = width;
			m_Height = height;

			GLenum internalFormat = 0, dataFormat = 0;
			if (channels == 4)
			{
				internalFormat = GL_RGBA8;
				dataFormat = GL_RGBA;
			}
			else if (channels == 3)
			{
				internalFormat = GL_RGB8;
				dataFormat = GL_RGB;
			}

			m_InternalFormat = internalFormat;
			m_DataFormat = dataFormat;

			E_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

			glCreateTextures(GL_TEXTURE_2D, 1, &m_RendererID);
			glTextureStorage2D(m_RendererID, 1, internalFormat, m_Width, m_Height);

			glTextureParameteri(m_RendererID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTextureParameteri(m_RendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTextureParameteri(m_RendererID, GL_TEXTURE_WRAP_T, GL_REPEAT);

			glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, dataFormat, GL_UNSIGNED_BYTE, data);

			stbi_image_free(data);			
		}
		else{
			E_CORE_WARN("Could not load {0}", path);
		}
	}

	GLTexture2D::~GLTexture2D()
	{
	  
	  
          if (m_RendererID) {
	    if(glfwGetCurrentContext())
	      glDeleteTextures(1, &m_RendererID);
          }

	  m_RendererID = 0;       
	}

	void GLTexture2D::SetData(void* data, uint32_t size)
	{
		

		uint32_t bpp = m_DataFormat == GL_RGBA ? 4 : 3;
		E_CORE_ASSERT(size == m_Width * m_Height * bpp, "Data must be entire texture!");
		glTextureSubImage2D(m_RendererID, 0, 0, 0, m_Width, m_Height, m_DataFormat, GL_UNSIGNED_BYTE, data);
	}

	void GLTexture2D::Bind(uint32_t slot) const
	{		
		
		E_CORE_ASSERT(m_RendererID != 0, "Texture RendererID is 0!");
		glBindTextureUnit(slot, m_RendererID);
		// glActiveTexture(GL_TEXTURE0 + slot);                // Set the active texture slot
    	// glBindTexture(GL_TEXTURE_2D, m_RendererID);         // Bind the texture
	}
}