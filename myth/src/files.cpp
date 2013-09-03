#include <stdio.h>
#include <dirent.h>
#include <files.h>
#include <debug.h>

//-----------------------------------------------------------------------

bool LoadFile(const std::string& filename, std::vector<char>& content)
{
	errno = 0;
	FILE* in = fopen(filename.c_str(), "rb");
	if (NULL != in)
	{
		fseek(in, 0, SEEK_END);
		const long size = ftell(in);
		content.resize(size);
		rewind(in);
		fread(content.data(), sizeof(char), content.size(), in);
		fclose(in);
	}
	else
	{
		LOG("%s - errno: %s\n", filename.c_str(), strerror(errno));
	}
	return (NULL != in);
}

//-----------------------------------------------------------------------

bool ListFiles(const std::string& dirname, std::vector<std::string>& files)
{
	const std::string path(dirname + "\\");

	DIR* dir = opendir(path.c_str());
	if (NULL != dir)
	{
		files.clear();
		for (struct dirent* ent = readdir(dir); NULL != ent; ent = readdir(dir))
		{
			if ((0 != strcmp(ent->d_name, ".")) && (0 != strcmp(ent->d_name, "..")))
			{
				files.push_back(path + ent->d_name);
			}
		}
		closedir(dir);
	}
	return (NULL != dir);
}
