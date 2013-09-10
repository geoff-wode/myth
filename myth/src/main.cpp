#include <SDL.h>
#include <myth.h>
#include <debug.h>

//--------------------------------------------------------

FILE* debug::logFile;

//--------------------------------------------------------

int main(int argc, char* argv[])
{
  debug::logFile = fopen("stderr.txt", "wb");

  Myth program("Myth");

  program.Run();

  return 0;
}
