#pragma once
#include "FactoryDesc.h"
#include "Render/GraphicsDevice.h"
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
  static Scope<GraphicsDevice> NewGraphicsDevice();

  static Ref<Shader> CreateShader(const std::string& filepath);
  static Ref<Buffer> CreateBuffer(BufferType type);
  static Ref<VertexArray> CreateVertexArray();

 private:
  static FactoryDesc s_Desc;
};
}
