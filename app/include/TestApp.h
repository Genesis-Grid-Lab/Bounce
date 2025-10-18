#pragma once
#include "Core/Application.h"
#include "TestLayer.h"

class TestApp : public Engine::Application {
public:
    TestApp()
        :Engine::Application("TestApp", {500, 500})
        {
            PushLayer(new TestLayer());
        }

    ~TestApp(){}
};