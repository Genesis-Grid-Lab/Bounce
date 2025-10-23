#pragma once

#include "Core/Types.h"
#include "Events/Event.h"
#include "Render/GraphicsContext.h"

namespace Engine {    

    struct WindowProps{

        std::string m_Title;
        uint32_t m_Width;
        uint32_t m_Height;
    
        WindowProps(const std::string& title = "engine", 
                              uint32_t width = 1600,
                            uint32_t height = 900)
            : m_Title(title), m_Width(width), m_Height(height){
    
            }
    };
    
    class  Window
    {
    public:
        using EventCallbackFn = std::function<void(Event&)>;
    
        virtual ~Window() = default;
    
        virtual void OnUpdate() = 0;
    
        uint32_t GetWidth() const { return m_Data.Width;}
        uint32_t GetHeight() const { return m_Data.Height;}
    
        //Window attributes
        void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback;}
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;

       protected:
        virtual void Init(const WindowProps& props) = 0;
        virtual void Shutdown() = 0;
    
    private:
        Scope<GraphicsContext> m_Context;
    
        struct WindowData{
            std::string Title;
            uint32_t Width, Height;
            bool VSync;
    
            EventCallbackFn EventCallback;
        };
    
        WindowData m_Data;

        friend class GlfwWindow;

        // TODO: change
        static glm::vec2 s_LastMousePosition;
    public:
        static glm::vec2 s_MouseDelta;
    };
}
