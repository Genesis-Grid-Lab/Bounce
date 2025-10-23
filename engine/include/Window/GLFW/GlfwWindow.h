#pragma once

#include "Window/Window.h"

struct GLFWwindow;

namespace Engine {    

    class  GlfwWindow : public Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;
    
        GlfwWindow(const WindowProps& props);
        ~GlfwWindow();
    
        virtual void OnUpdate() override;
    
        uint32_t GetWidth() const { return m_Data.Width;}
        uint32_t GetHeight() const { return m_Data.Height;}
    
        //Window attributes
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback;}
        void SetVSync(bool enabled);
        bool IsVSync() const;
    
        void* GetNativeWindow() const { return m_Window;}
    
    protected:
        void Init(const WindowProps& props);
        void Shutdown();
    
    private:
      GLFWwindow* m_Window;          
    };
}
