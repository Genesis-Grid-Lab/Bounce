#include "Render/glad/GLRendererAPI.h"
#include "glad/glad.h"
#include "Render/VertexArray.h"

namespace Engine {

    void OpenGLMessageCallback(
		unsigned source,
		unsigned type,
		unsigned id,
		unsigned severity,
		int length,
		const char* message,
		const void* userParam)
	{
		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:         E_CORE_CRITICAL(message); return;
			case GL_DEBUG_SEVERITY_MEDIUM:       E_CORE_ERROR(message); return;
			case GL_DEBUG_SEVERITY_LOW:          E_CORE_WARN(message); return;
			case GL_DEBUG_SEVERITY_NOTIFICATION: E_CORE_TRACE(message); return;
		}
		
		E_CORE_ASSERT(false, "Unknown severity level!");
	}

	void GLRendererAPI::Init()
	{
        E_CORE_INFO("[RendererAPI] Init");
#ifdef E_DEBUG
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(OpenGLMessageCallback, nullptr);
		
		glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_NOTIFICATION, 0, NULL, GL_FALSE);
	#endif

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);
	}

	void GLRendererAPI::SetViewport(glm::vec4& view)
	{
		glViewport(view.x, view.y, view.z, view.w);
	}

    // void GLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	// {
	// 	glViewport(x, y, width, height);
	// }

	void GLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void GLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void GLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount)
	{
		uint32_t count = indexCount ? indexCount : vertexArray->GetIndexBuffer()->GetCount();
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}