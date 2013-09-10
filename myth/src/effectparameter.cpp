#include <memory>
#include <effectparameter.h>
#include <gl_loader/gl_loader.h>

static bool CacheValue(const void* const value, void* const cache, size_t size)
{
  if (0 != std::memcmp(value, cache, size))
  {
    std::memcpy(cache, value, size);
    return true;
  }
  return false;
}

EffectParameter::EffectParameter()
  : tainted(false)
{
}

EffectParameter& EffectParameter::operator=(int value)              { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(unsigned int value)     { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(float value)            { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(const glm::vec2& value) { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(const glm::vec3& value) { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(const glm::vec4& value) { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(const glm::mat2& value) { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(const glm::mat3& value) { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }
EffectParameter& EffectParameter::operator=(const glm::mat4& value) { tainted |= CacheValue(&value, cache, sizeof(value)); return *this; }

