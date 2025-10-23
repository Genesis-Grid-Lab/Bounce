#pragma once

#include "Core/Types.h"
#include "Window/Window.h"
#include "Core/Timestep.h"
#include "Core/Layer.h"
#include "Core/LayerStack.h"
#include "Events/Event.h"
#include "Events/ApplicationEvent.h"

int main(int argc, char** argv);


namespace Engine {

  enum class LayerActionType {
    Push,
    Pop
  };

  struct LayerAction {
    LayerActionType Type;
    Layer* LayerPtr;
  };

  class Application {
  public:
    Application(const std::string& name = "Application", const glm::vec2& size = glm::vec2(100));
    virtual ~Application();

    void OnEvent(Event& e);

    void PushLayer(Layer* layer);
    void PushOverlay(Layer* layer);
    void PopLayer(Layer* layer);

    void Close();

    Window& GetWindow() { return *m_Window;}
    static Application& Get() { return *s_Instance;}

    void QueueLayerAction(LayerActionType type, Layer* layer) {
      m_LayerActionQueue.push({ type, layer });
    }

  private:
    void Run();
    bool OnWindowClose(WindowCloseEvent& e);
    bool OnWindowResize(WindowResizeEvent& e);
  private:
    Scope<Window> m_Window;

    bool m_Running = true;
    bool m_Minimized = false;
    LayerStack m_LayerStack;
    float m_LastFrameTime = 0.0f;
    std::queue<LayerAction> m_LayerActionQueue;        
  private:
    friend int ::main(int argc, char** argv);
    static Application *s_Instance;
  };
}
