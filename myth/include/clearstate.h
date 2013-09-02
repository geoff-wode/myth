#if ! defined(__CLEAR_STATE__)
#define __CLEAR_STATE__

#include <glm/glm.hpp>

// Describes a complete set of state parameters for clearing a buffer.
struct ClearState
{
  ClearState()
    : colour(0,0,0,1), colourMask(true),
      depth(1.0f), depthMask(true)
  {
  }

  glm::vec4 colour;
  glm::bvec4 colourMask;

  float depth;
  bool depthMask;
};

#endif // __CLEAR_STATE__
