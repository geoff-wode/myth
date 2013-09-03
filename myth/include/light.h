// Defines a single light source (either point- or directed).

#if ! defined(__LIGHT__)
#define __LIGHT__

#include <glm/glm.hpp>

struct Light
{
  Light()
    : position(0), colourAttenuation(0), enabled(false)
  {
  }

  Light(bool enabled, const glm::vec4& position)
    : position(position), colourAttenuation(1,1,1,0.2), enabled(enabled)
  {
  }

  glm::vec4 position;           // if w == 0, light is directional, else positional
  glm::vec4 colourAttenuation;  // rgb = light intensity, a = attenuation
  bool enabled;
};

#endif // __LIGHT__
