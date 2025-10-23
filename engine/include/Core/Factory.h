#pragma once
#include "FactoryDesc.h"
#include "Render/GraphicsContext.h"
#include "Render/RendererAPI.h"
#include "Window/Window.h"
#include "Render/Buffer.h"
#include "Render/Shader.h"
#include "Render/VertexArray.h"
#include "Render/Texture.h"
#include "Render/Skybox.h"

namespace Engine {

class Factory {
public:
  static void SetDesc(const FactoryDesc &desc);
  static FactoryDesc GetDesc();
  static WindowAPI GetWindowAPI() { return s_Desc.Window_API; }
  static GraphicsAPI GetGraphicsAPI() { return s_Desc.Graphics_API; }

  static Scope<Window> NewWindow();
  static Scope<GraphicsContext> Create(void* window);
  static Scope<RendererAPI> CreateRendererAPI();

  static Ref<Shader> CreateShader(const std::string& filepath);
  static Ref<Shader> CreateShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
  static Ref<Skybox> CreateSkybox(const std::vector<std::string>& faces);
  static Ref<VertexArray> Create();

  static Ref<VertexBuffer> Create(uint32_t size);
  static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
  static Ref<VertexBuffer> Create(Vertex *vertices, uint32_t size);

  static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

 private:
  static FactoryDesc s_Desc;
};
}
