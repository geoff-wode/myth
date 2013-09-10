// Defines a single light source (either point- or directed).

#if ! defined(__LIGHT__)
#define __LIGHT__

#include <glm/glm.hpp>

struct Light
{
  Light() : enabled(false)
  {
  }

  Light(bool enabled, const glm::vec4& position, const glm::vec3& colour, float attenuation)
    : enabled(enabled), position(position), colour(colour), attenuation(attenuation)
  {
  }

  bool      enabled;
  bool      directional;  // if true, "position" represents the direction of light rays "from infinity"
  glm::vec3 position;     // if directional == true, the direction the light is coming _from_, else it's world position
  glm::vec3 colour;
  float     attenuation;  // determines the "fall off with distance" (ignored for directional lights)
};

#endif // __LIGHT__
