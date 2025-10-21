#include "Render/glad/GLRendererAPI.h"
#include "glad/glad.h"
#include "Render/VertexArray.h"

namespace Engine {

    void GLRendererAPI::Init() {
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}
void GLRendererAPI::SetViewport(glm::vec4& view){ glViewport(view.x,view.y,view.z,view.w); }
void GLRendererAPI::SetClearColor(glm::vec4& color){ glClearColor(color.r,color.g,color.b,color.a); }
void GLRendererAPI::Clear(){ glClear(GL_COLOR_BUFFER_BIT); }
void GLRendererAPI::EnableBlend(bool enable){ if(enable) glEnable(GL_BLEND); else glDisable(GL_BLEND); }
void GLRendererAPI::DrawIndexed(const VertexArray& /*vao*/, std::uint32_t count){
    glDrawElements(GL_TRIANGLES, (GLsizei)count, GL_UNSIGNED_INT, (const void*)0);
}
}