#pragma once

#include "Render/Texture.h"

typedef unsigned int GLenum;

namespace Engine {

    class  GLTexture2D : public Texture2D
	{
	public:
		GLTexture2D(unsigned int id);
        GLTexture2D(uint32_t width, uint32_t height);
		GLTexture2D(uint32_t width, uint32_t height, GLenum internalFormat, GLenum dataFormat);
        GLTexture2D(const std::string& path);
		// GLTexture2D(Ref<Framebuffer>& buffer);
        virtual ~GLTexture2D() override;

        virtual uint32_t GetWidth() const override { return m_Width;  }
        virtual uint32_t GetHeight() const override { return m_Height; }
        virtual uint32_t GetRendererID() const override { return m_RendererID; }
        
        virtual void SetData(void* data, uint32_t size) override;

        virtual void Bind(uint32_t slot = 0) const override;

        virtual bool IsLoaded() const override { return m_IsLoaded; }

        virtual bool operator==(const Texture& other) const override
        {
            return m_RendererID == ((GLTexture2D&)other).m_RendererID;
        }
		std::string m_Path;
		std::string type;
	private:
		bool m_IsLoaded = false;
		uint32_t m_Width, m_Height;
		uint32_t m_RendererID;
		GLenum m_InternalFormat, m_DataFormat;
	};
}