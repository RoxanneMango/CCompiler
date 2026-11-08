#ifndef FILE_IO_H
#define FILE_IO_H

#include "string_utils.h"

static const int _StdHeaderLocationNum = 4;
static char * _StandardLibraryHeaderLocations[] = 
{
	"/usr/lib/gcc/x86_64-linux-gnu/9/include",
	"/usr/local/include",
	"/usr/include/x86_64-linux-gnu",
	"/usr/include"
};

int filterOutCommentsFromFile(const char * filePATH, const char * outPATH);
int readFromFile(const char * filePATH, _LinkedStringList * fileList);
int getFilesFromDirectory(_LinkedStringList * list, const char * PATH);

bool fileExistsInDirectory(const char * PATH, const char * file);

// Check whether the included file exists
int validateInclude(const char * fileName);

#endif // FILE_IO_H