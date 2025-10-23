#include "epch.h"
#include "Core/Application.h"
#include "Core/E_Assert.h"
#include "Core/Factory.h"
#include "Core/Timestep.h"

#include "Render/RenderCommand.h"
#include "Render/Renderer.h"

// temp
#include <GLFW/glfw3.h>
#include <fstream>

namespace Engine {

  Application* Application::s_Instance = nullptr;

  Application::Application(const std::string& name, const glm::vec2& size){
    Log::Init();
    E_CORE_ASSERT(!s_Instance, "Application already exists!");
    E_CORE_INFO("[APPLICATION] Starting");
    FactoryDesc desc;
    desc.Title = name;
    desc.DisplaySize = size;
    desc.Window_API = WindowAPI::GLFW;
    desc.Graphics_API = GraphicsAPI::OpenGL;
    Factory::SetDesc(desc);
    E_CORE_INFO("[APPLICATION] Settings desc");
    s_Instance = this;
    m_Window = Factory::NewWindow();
    m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

    Renderer::Init();
           
  }

  Application::~Application() {
    E_CORE_INFO("[APPLICATION] Shutting down");
    Renderer::Shutdown();
    
  }

  void Application::PushLayer(Layer* layer)
  {    
    E_CORE_INFO("[APPLICATION] Pushing layer");
    m_LayerStack.PushLayer(layer);
    layer->OnAttach();
  }

  void Application::PushOverlay(Layer* layer)
  {
    E_CORE_INFO("[APPLICATION] Pushing overlay");
    m_LayerStack.PushOverlay(layer);
    layer->OnAttach();
  }

  void Application::PopLayer(Layer* layer) {
    E_CORE_INFO("[APPLICATION] Pop layer");
    m_LayerStack.PopLayer(layer);
  }

  void Application::Close(){
    E_CORE_INFO("[APPLICATION] Closing");
    m_Running = false;
  }

   void Application::OnEvent(Event& e){
       E_CORE_INFO("[APPLICATION] Event, {}", e.GetName());
       EventDispatcher dispatcher(e);
       dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
       dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

       for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
           if (e.Handled)
               break;
           (*it)->OnEvent(e);
       }
    }

  void Application::Run(){
    while(m_Running){
        float time = (float)glfwGetTime();
        Timestep timestep = m_LastFrameTime;
        m_LastFrameTime = time;
        glm::vec4 clearColor = {0.5f, 0.3f, 0.1f, 1.0f};
        RenderCommand::SetClearColor(clearColor);
        RenderCommand::Clear();

        if (!m_Minimized) {
            for (Layer* layer : m_LayerStack) {
                layer->OnUpdate(timestep);
            }
        }

      m_Window->OnUpdate();

      while (!m_LayerActionQueue.empty()) {
        LayerAction action = m_LayerActionQueue.front();
        m_LayerActionQueue.pop();
            
        if (action.Type == LayerActionType::Push) {
        PushLayer(action.LayerPtr);
        } else if (action.Type == LayerActionType::Pop) {
        PopLayer(action.LayerPtr);
        delete action.LayerPtr;
        }
      }
    }
    
  }

bool Application::OnWindowClose(WindowCloseEvent& e){

    m_Running = false;
    return true;
}

bool Application::OnWindowResize(WindowResizeEvent& e){
    E_CORE_INFO("[APPLICATION] resize Width:{}, Height:{}", e.GetWidth(), e.GetHeight());
    if(e.GetWidth() == 0 || e.GetHeight() == 0){
        m_Minimized = true;
        return false;
    }

    m_Minimized = false;

    Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());

    return false;
}

  

}
