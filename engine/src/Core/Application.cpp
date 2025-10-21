#include "epch.h"
#include "Core/Application.h"
#include "Core/E_Assert.h"
#include "Core/Factory.h"
#include "Core/Timestep.h"

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
    s_Instance = this;
    m_Window = Factory::NewWindow();

    m_GraphicsDevice = Factory::NewGraphicsDevice();

    if(!m_GraphicsDevice->Initialize(*m_Window)){
        E_CORE_ERROR("Failed to init Graphics device");
    }
    
    m_Window->Events().Subscribe<EWindowResize>([this](const EWindowResize& e) { OnWindowResize(e);});
    m_Window->Events().Subscribe<EWindowClose>([this](const EWindowClose&) { OnWindowClose();});        
  }

  Application::~Application(){
    E_CORE_INFO("[APPLICATION] Shutting down");
    
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

  void Application::Run(){
    while(m_Running){
      // float time = (float)
      Timestep timestep = m_LastFrameTime;
      // m_LastFrameTime = time;

      if(!m_Minimized){
	for(Layer* layer : m_LayerStack){
	  layer->OnUpdate(timestep);
	}
      }
      m_Window->PollEvents();
      m_Window->SwapBuffers();

      m_GraphicsDevice->EndFrame();

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

  void Application::OnWindowClose() {
    E_CORE_INFO("[APPLICATION] Closing");
    m_Running = false;        
  }
  void Application::OnWindowResize(const EWindowResize& e){
    E_CORE_INFO("[APPLICATION] Resizing");
    if(e.fbWidth == 0 || e.fbHeight == 0){
      m_Minimized = true;            
    }

    m_Minimized = false;
  }

  

}
