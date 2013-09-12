#if ! defined(__MYTH__)
#define __MYTH__

#include <program.h>
#include <effect.h>
#include <buffers/vertexbuffer.h>
#include <buffers/indexbuffer.h>
#include <boost/shared_ptr.hpp>
#include <model.h>

class Myth : public Program
{
public:
  Myth(const char* const windowTitle);
  virtual ~Myth();

protected:
  virtual void Initialise();
  virtual void LoadContent();
  virtual void Update(float elapsedMS);
  virtual void Render(float elapsedMS);

private:
  boost::shared_ptr<Effect> effect;
  std::vector<glm::mat4> transforms;
  std::vector<boost::shared_ptr<Model>> models;
  RenderState renderState;
  ClearState clearState;
};

#endif // __MYTH__
