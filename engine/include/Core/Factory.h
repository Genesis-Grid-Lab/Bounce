#pragma once
#include "FactoryDesc.h"
#include "Render/GraphicsContext.h"
#include "Render/RendererAPI.h"
#include "Window/Window.h"
#include "Render/Buffer.h"
#include "Render/Shader.h"
#include "Render/VertexArray.h"

namespace Engine {

class Factory {
public:
  static void SetDesc(const FactoryDesc &desc);
  static WindowAPI GetWindowAPI() { return s_Desc.Window_API; }
  static GraphicsAPI GetGraphicsAPI() { return s_Desc.Graphics_API; }

  static Scope<Window> NewWindow();
  static Scope<GraphicsContext> CreateContext();
  static Scope<RendererAPI> CreateRendererAPI();

  static Ref<Shader> CreateShader(const std::string& filepath);
  static Ref<VertexArray> Create();

  static Ref<VertexBuffer> Create(uint32_t size);
  static Ref<VertexBuffer> Create(float* vertices, uint32_t size);
  static Ref<VertexBuffer> Create(Vertex *vertices, uint32_t size);

  static Ref<IndexBuffer> Create(uint32_t* indices, uint32_t count);

 private:
  static FactoryDesc s_Desc;
};
}
