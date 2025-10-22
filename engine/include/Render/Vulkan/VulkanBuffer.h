#include "Render/Buffer.h"

namespace Engine {

  class VulkanBuffer : public Buffer {
  public:
    VulkanBuffer(BufferType type);
    ~VulkanBuffer() override;

    BufferType Type() const override { return m_Type; }
    void SetData(const void *data, std::size_t bytes,
                 bool dynamic = false) override;
    void UpdateSubData(std::size_t byteOffset, const void *data,
                       std::size_t bytes) override;
    void Bind() const override;
    void Unbind() const override;

  private:
    BufferType m_Type;
  };
}
