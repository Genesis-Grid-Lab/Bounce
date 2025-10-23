#include "epch.h"
#include "Window/GLFW/GlfwWindow.h"
#include "Render/Renderer.h"
#include "Core/Input.h"
#include "Core/E_Assert.h"
#include "Events/ApplicationEvent.h"
#include "Events/MouseEvent.h"
#include "Events/KeyEvent.h"
#include <GLFW/glfw3.h>
#include "Core/Factory.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Engine {

    glm::vec2 Window::s_LastMousePosition = glm::vec2(0.0f);
    glm::vec2 Window::s_MouseDelta = glm::vec2(0.0f);

    static uint8_t s_GLFWwindowCount = 0;

    static void GLFWErrorCallback(int error, const char* desc){
        E_CORE_ERROR("GLFW Error ({0}): {1}", error, desc);
    }

    GlfwWindow::GlfwWindow(const WindowProps& props){        
        Init(props);
    }

    GlfwWindow::~GlfwWindow(){        
	    E_CORE_INFO("Shutting down window...");
        Shutdown();
    }

    void GlfwWindow::Init(const WindowProps& props){        
        m_Data.Title = props.m_Title;
        m_Data.Width = props.m_Width;
        m_Data.Height = props.m_Height;

        E_CORE_INFO("Creating window {0} ({1}, {2})", props.m_Title, props.m_Width, props.m_Height);

        if( s_GLFWwindowCount == 0){
            int success = glfwInit();
            E_CORE_ASSERT(success, "Could not initialize GLFW!");
            glfwSetErrorCallback(GLFWErrorCallback);
        }

        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        #if defined(UE_DEBUG)
            if (Renderer::GetAPI() == RendererAPI::API::OpenGL)
                glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
        #endif

        m_Window = glfwCreateWindow((int)props.m_Width, (int)props.m_Height, m_Data.Title.c_str(), nullptr, nullptr);
        ++s_GLFWwindowCount;
        
        m_Context = Factory::Create(m_Window);
        m_Context->Init();

        glfwSetWindowUserPointer(m_Window, &m_Data);
        SetVSync(true);

        GLFWimage images[1]; images[0].pixels = stbi_load("Data/images/logo.png", &images[0].width, &images[0].height, 0, 4); //rgba channels 
        glfwSetWindowIcon(m_Window, 1, images); 
        stbi_image_free(images[0].pixels);

        // Set GLFW callbacks
        glfwSetWindowSizeCallback(m_Window, [](GLFWwindow* window, int width, int height){
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            data.Width = width;
            data.Height = height;

            WindowResizeEvent event(width, height);
            data.EventCallback(event);
        });

        glfwSetWindowCloseCallback(m_Window, [](GLFWwindow* window)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
            WindowCloseEvent event;
            data.EventCallback(event);
        });

        glfwSetKeyCallback(m_Window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS:
                {
                    KeyPressedEvent event(key, 0);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    KeyReleasedEvent event(key);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_REPEAT:
                {
                    KeyPressedEvent event(key, 1);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetCharCallback(m_Window, [](GLFWwindow* window, unsigned int keycode)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            KeyTypedEvent event(keycode);
            data.EventCallback(event);
        });

        glfwSetMouseButtonCallback(m_Window, [](GLFWwindow* window, int button, int action, int mods)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            switch (action)
            {
                case GLFW_PRESS:
                {
                    MouseButtonPressedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
                case GLFW_RELEASE:
                {
                    MouseButtonReleasedEvent event(button);
                    data.EventCallback(event);
                    break;
                }
            }
        });

        glfwSetScrollCallback(m_Window, [](GLFWwindow* window, double xOffset, double yOffset)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseScrolledEvent event((float)xOffset, (float)yOffset);
            data.EventCallback(event);
        });

        glfwSetCursorPosCallback(m_Window, [](GLFWwindow* window, double xPos, double yPos)
        {
            WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

            MouseMovedEvent event((float)xPos, (float)yPos);
            data.EventCallback(event);
        });
        
    }

    void GlfwWindow::Shutdown(){        
        glfwDestroyWindow(m_Window);
        --s_GLFWwindowCount;

        if(s_GLFWwindowCount == 0){
            glfwTerminate();
        }
    }

    void GlfwWindow::OnUpdate(){

        glfwPollEvents();
        m_Context->SwapBuffers();

        glm::vec2 currentPos = Input::GetMousePosition();
        s_MouseDelta = currentPos - s_LastMousePosition;
        s_LastMousePosition = currentPos;
    }

    void GlfwWindow::SetVSync(bool enabled){

        if(enabled)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        m_Data.VSync = enabled;
    }

    bool GlfwWindow::IsVSync() const{
        return m_Data.VSync;
    }

}