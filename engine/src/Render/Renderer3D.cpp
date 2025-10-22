#include "epch.h"
#include "Render/Renderer3D.h"
#include "Core/Factory.h"

//temp
#include <glad/glad.h>

namespace Engine {

    static Ref<Shader> m_Shader;    
    static Ref<Shader> m_ShaderSimple;    
    static Ref<Shader> m_LightShader;
    static Ref<Shader> m_SkyShader;
    static glm::vec3 m_LightPos; 
    static Ref<Mesh> s_CubeMesh;
    static Ref<Mesh> s_SphereMesh;

    //TODO: remove
static GLuint lineVAO = 0, lineVBO = 0;

    Ref<Mesh> GenerateSphereMesh(uint32_t sectorCount = 36, uint32_t stackCount = 18) {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        float x, y, z, xy;                              // vertex position
        float nx, ny, nz, lengthInv = 1.0f;             // vertex normal
        float s, t;                                     // vertex texCoord

        float sectorStep = 2 * glm::pi<float>() / sectorCount;
        float stackStep = glm::pi<float>() / stackCount;
        float sectorAngle, stackAngle;

        for (uint32_t i = 0; i <= stackCount; ++i) {
            stackAngle = glm::pi<float>() / 2 - i * stackStep; // from pi/2 to -pi/2
            xy = cos(stackAngle);                              // r * cos(u)
            z = sin(stackAngle);                               // r * sin(u)

            for (uint32_t j = 0; j <= sectorCount; ++j) {
                sectorAngle = j * sectorStep;

                x = xy * cos(sectorAngle);                     // x = r * cos(u) * cos(v)
                y = xy * sin(sectorAngle);                     // y = r * cos(u) * sin(v)
                nx = x; ny = y; nz = z;

                Vertex v;
                v.Position = glm::vec3(x, y, z);
                v.Normal = glm::normalize(glm::vec3(nx, ny, nz));
                v.TexCoords = glm::vec2((float)j / sectorCount, (float)i / stackCount);
                v.Tangent = glm::vec3(1, 0, 0);    // placeholder
                v.Bitangent = glm::vec3(0, 1, 0);  // placeholder
                vertices.push_back(v);
            }
        }

        // index generation
        for (uint32_t i = 0; i < stackCount; ++i) {
            uint32_t k1 = i * (sectorCount + 1);     // beginning of current stack
            uint32_t k2 = k1 + sectorCount + 1;      // beginning of next stack

            for (uint32_t j = 0; j < sectorCount; ++j, ++k1, ++k2) {
                if (i != 0)
                    indices.insert(indices.end(), { k1, k2, k1 + 1 });
                if (i != (stackCount - 1))
                    indices.insert(indices.end(), { k1 + 1, k2, k2 + 1 });
            }
        }

        std::vector<TextureMesh> noTextures;
        return CreateRef<Mesh>(vertices, indices, noTextures);
    }

    Ref<Mesh> GenerateCubeMesh() {
        std::vector<Vertex> vertices;
        std::vector<uint32_t> indices;

        glm::vec3 positions[] = {
            {-0.5f, -0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, // front
            {-0.5f, -0.5f, -0.5f}, {-0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, // back
            {-0.5f,  0.5f, -0.5f}, {-0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f,  0.5f, -0.5f}, // top
            {-0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f, -0.5f}, { 0.5f, -0.5f,  0.5f}, {-0.5f, -0.5f,  0.5f}, // bottom
            { 0.5f, -0.5f, -0.5f}, { 0.5f,  0.5f, -0.5f}, { 0.5f,  0.5f,  0.5f}, { 0.5f, -0.5f,  0.5f}, // right
            {-0.5f, -0.5f, -0.5f}, {-0.5f, -0.5f,  0.5f}, {-0.5f,  0.5f,  0.5f}, {-0.5f,  0.5f, -0.5f}  // left
        };

        glm::vec3 normals[] = {
            { 0,  0,  1}, { 0,  0, -1}, { 0,  1,  0}, { 0, -1,  0}, { 1,  0,  0}, {-1,  0,  0}
        };

        glm::vec2 uvs[] = {
            {0, 0}, {1, 0}, {1, 1}, {0, 1}
        };

        uint32_t faceIndices[] = {
            0, 1, 2, 2, 3, 0,
            4, 5, 6, 6, 7, 4,
            8, 9,10,10,11, 8,
           12,13,14,14,15,12,
           16,17,18,18,19,16,
           20,21,22,22,23,20
        };

        for (int face = 0; face < 6; ++face) {
            for (int i = 0; i < 4; ++i) {
                Vertex v;
                v.Position = positions[face * 4 + i];
                v.Normal = normals[face];
                v.TexCoords = uvs[i];
                v.Tangent = glm::vec3(1, 0, 0);     // Placeholder
                v.Bitangent = glm::vec3(0, 1, 0);   // Placeholder
                vertices.push_back(v);
            }
        }

        for (int i = 0; i < 36; ++i)
            indices.push_back(faceIndices[i]);

        std::vector<TextureMesh> noTextures; // Can be replaced later

        return CreateRef<Mesh>(vertices, indices, noTextures);
    }

    void Renderer3D::Init(){
        m_Shader = Factory::CreateShader("Data/Shaders/model.glsl");
        m_ShaderSimple = Factory::CreateShader("Data/Shaders/Simplemodel.glsl");
        m_LightShader = Factory::CreateShader("Data/Shaders/BasicLight.glsl");
        m_SkyShader = Factory::CreateShader("Data/Shaders/skybox.glsl");

        s_CubeMesh = GenerateCubeMesh();
        s_SphereMesh = GenerateSphereMesh();

         float quadVertices[] = {
			// positions   // texCoords
			-1.0f, -1.0f,  0.0f, 0.0f,
			1.0f, -1.0f,  1.0f, 0.0f,
			1.0f,  1.0f,  1.0f, 1.0f,
			-1.0f,  1.0f,  0.0f, 1.0f
		};

        uint32_t indices[] = { 0, 1, 2, 2, 3, 0 };

         glGenVertexArrays(1, &lineVAO);
		glGenBuffers(1, &lineVBO);
		glBindVertexArray(lineVAO);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 2, nullptr, GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
		glBindVertexArray(0);
    }

    void Renderer3D::Shutdown(){
         m_Shader.reset();    
      m_ShaderSimple.reset();    
      m_LightShader.reset();
      m_SkyShader.reset();          
    //   m_LightModel.reset();   
    //   m_Skybox.reset(); 
      s_CubeMesh.reset(); 
      s_SphereMesh.reset();         
    //   s_Animator.reset()
    }

    void Renderer3D::BeginCamera(const Camera3D& camera)
	{        
		m_Shader->Bind();
        m_Shader->SetMat4("u_View", camera.GetViewMatrix());
        m_Shader->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_Shader->SetFloat3("u_ViewPos", camera.GetPosition());
        
        m_Shader->SetFloat3("u_LightPos", m_LightPos);             
        m_Shader->SetInt("u_EntityID", -1);
        m_LightShader->Bind();
        m_LightShader->SetMat4("u_View", camera.GetViewMatrix());
        m_LightShader->SetMat4("u_Projection", camera.GetProjectionMatrix());  
        
        m_ShaderSimple->Bind();
        m_ShaderSimple->SetMat4("u_View", camera.GetViewMatrix());
        m_ShaderSimple->SetMat4("u_Projection", camera.GetProjectionMatrix());
        m_ShaderSimple->SetFloat3("u_ViewPos", camera.GetPosition());
        
        m_ShaderSimple->SetFloat3("u_LightPos", m_LightPos);             
        m_ShaderSimple->SetInt("u_EntityID", -1);
        
        m_SkyShader->Bind();
        // m_Skybox->Draw(m_SkyShader, camera.GetViewMatrix(), camera.GetProjectionMatrix());
	}

     void Renderer3D::EndCamera(){
        
    }

     void Renderer3D::RenderLight(const glm::vec3& pos){        
        m_LightShader->Bind();
        m_LightPos = pos;  
        glm::mat4 imodel = glm::mat4(1.0f);
        imodel = glm::translate(imodel, pos);
        imodel = glm::rotate(imodel, glm::radians(0.0f), glm::vec3(0, 1, 0));
        imodel = glm::scale(imodel, glm::vec3(1));        

        m_LightShader->SetMat4("u_Model", imodel);     
                

        // m_LightModel->Draw(m_LightShader);
    }

    void Renderer3D::DrawCube(const glm::mat4& transform, const glm::vec3& color, const float transparancy){        
        m_ShaderSimple->Bind();
        m_ShaderSimple->SetMat4("u_Model", transform);
        m_ShaderSimple->SetFloat3("u_Color", color); 
        // m_ShaderSimple->SetInt("u_EntityID", entityID);
        m_ShaderSimple->SetFloat("u_Transparancy", transparancy);        
        s_CubeMesh->Draw(m_ShaderSimple);
        m_ShaderSimple->Unbind();
    }

    void Renderer3D::DrawCube(const glm::vec3& position, const glm::vec3& size, const glm::vec3& color, const float transparancy) {
        
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)
                            * glm::scale(glm::mat4(1.0f), size);        
        DrawCube(transform, color, transparancy);
    }
}