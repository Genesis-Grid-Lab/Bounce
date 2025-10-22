#pragma once
#include "Render/Buffer.h"

using GLuint = unsigned int;

namespace Engine {

    class  GLVertexBuffer : public VertexBuffer
	{
	public:
        GLVertexBuffer(uint32_t size);
        GLVertexBuffer(float* vertices, uint32_t size);
        GLVertexBuffer(Vertex* vertices, uint32_t size);
		~GLVertexBuffer();

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void SetData(const void* data, uint32_t size) override;

		virtual const BufferLayout& GetLayout() const override{ return m_Layout; }
		virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }
    private:
		uint32_t m_RendererID;
		BufferLayout m_Layout;
	};


    class  GLIndexBuffer : public IndexBuffer
	{
	public:
        GLIndexBuffer(uint32_t* indices, uint32_t count);
		virtual ~GLIndexBuffer() override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual uint32_t GetCount() const override { return m_Count;}
        
    private:
		uint32_t m_RendererID;
		uint32_t m_Count;
	};
}