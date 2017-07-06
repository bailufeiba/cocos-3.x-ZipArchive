#pragma once
#include "unzip/unzip.h"

class ZipArchive
{
public:
	ZipArchive();
	~ZipArchive();

	static bool ZipToPath( const char* zip, const char* path, const char* password = "" );
private:
	static unzFile unzipOpenFile( const char* zip );
	static bool createDirectory(const char *path);

};

