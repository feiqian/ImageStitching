#ifndef __UTIL_H__
#define __UTIL_H__
#include <io.h>
#include <vector>
#include <string>
#include <list>

struct Utils
{
	static std::vector<std::string> getFiles(const std::string& folder, const std::string& fileExtension = std::string("*"));
};

#endif