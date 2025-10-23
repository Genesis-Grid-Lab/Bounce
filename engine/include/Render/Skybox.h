#pragma once

#include "Render/Mesh.h"
#include "Render/Shader.h"
#include "Render/Texture.h"
#include "Render/Camera.h"

namespace Engine {

    class Skybox {
    public:
        virtual ~Skybox() = default;        
        virtual void Draw(const Ref<Shader>& shader, const glm::mat4& view, const glm::mat4& projection) = 0;

    };

}