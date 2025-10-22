#pragma once

namespace Engine {

class Texture2D {
public:
  virtual ~Texture2D() = default;

  virtual bool LoadFromFile(const char *path, bool flipY = true);

  virtual bool Create(uint32_t width, uint32_t height, int channels,
                      const void *pixels) = 0;

  virtual void Bind(uint32_t slot) const = 0;
  virtual uint32_t Width() const = 0;
  virtual uint32_t Height() const = 0;
};
}
