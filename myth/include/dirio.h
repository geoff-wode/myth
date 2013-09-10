#if ! defined(__DIR_IO_H__)
#define __DIR_IO_H__

#include <string>
#include <vector>

// Load a given file into memory.
// Returns true if loaded successfully, otherwise false.
bool LoadFile(const std::string& filename, std::vector<char>& content);

// Return a vector of the filenames contained within the given directory, not
// including the special files "." and "..".
// The vector of files is modified on successful return only.
// Returns true if the directory exists, otherwise false.
bool ListFiles(const std::string& dirname, std::vector<std::string>& files);

#endif // __DIR_IO_H__
