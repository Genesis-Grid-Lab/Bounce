#pragma message("Compiling GLBuffer.cpp")
#include "Render/glad/GLBuffer.h"
#include "glad/glad.h"

//tmp
#include <GLFW/glfw3.h>

namespace Engine {

    GLVertexBuffer::GLVertexBuffer(uint32_t size)
	{				
		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
	}

	GLVertexBuffer::GLVertexBuffer(float* vertices, uint32_t size)
	{

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	GLVertexBuffer::GLVertexBuffer(Vertex* vertices, uint32_t size)
	{

		glCreateBuffers(1, &m_RendererID);
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_STATIC_DRAW);
	}

	GLVertexBuffer::~GLVertexBuffer()
	{		
		
          if (m_RendererID) {
	    if(glfwGetCurrentContext())
	      glDeleteBuffers(1, &m_RendererID);
          }
	  
	  m_RendererID = 0;
                
	}

	void GLVertexBuffer::Bind() const
	{

		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	}

	void GLVertexBuffer::Unbind() const
	{

		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void GLVertexBuffer::SetData(const void* data, uint32_t size)
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
	}

	/////////////////////////////////////////////////////////////////////////////
	// IndexBuffer //////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////

	GLIndexBuffer::GLIndexBuffer(uint32_t* indices, uint32_t count)
		: m_Count(count)
	{

		glCreateBuffers(1, &m_RendererID);
		
		// GL_ELEMENT_ARRAY_BUFFER is not valid without an actively bound VAO
		// Binding with GL_ARRAY_BUFFER allows the data to be loaded regardless of VAO state. 
		glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
		glBufferData(GL_ARRAY_BUFFER, count * sizeof(uint32_t), indices, GL_STATIC_DRAW);
	}

	GLIndexBuffer::~GLIndexBuffer()
	{		
          if (m_RendererID) {
	    if(glfwGetCurrentContext())
	      glDeleteBuffers(1, &m_RendererID);
          }

	  m_RendererID = 0;
	}

	void GLIndexBuffer::Bind() const
	{

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
	}

	void GLIndexBuffer::Unbind() const
	{

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}
}

