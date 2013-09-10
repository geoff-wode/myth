#if ! defined(__PROGRAM__)
#define __PROGRAM__

#include <device.h>
#include <boost/shared_ptr.hpp>

class Program
{
public:
  Program(const char* const windowTitle);
  virtual ~Program();

  Device Device;

  void Run();
  void Quit();

protected:
  virtual void Initialise();
  virtual void LoadContent();
  virtual void Update(float elapsedMS);
  virtual void Render(float elapsedMS);

private:
  bool hasQuit;
  const char* const windowTitle;
  void EventLoop();
};

#endif // __PROGRAM__
