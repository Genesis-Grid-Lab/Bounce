#pragma once
#include "Render/VertexArray.h"
using GLuint = unsigned int;

namespace Engine {
    class GLVertexArray : public VertexArray {
public:
    GLVertexArray();
    ~GLVertexArray() override;

    void Bind() const override;
    void Unbind() const override;

    void EnableAttrib(unsigned int index, int comps, unsigned int type,
                      bool normalized, int stride, std::size_t offset) override;

private:
    GLuint m_Id = 0;
};
}