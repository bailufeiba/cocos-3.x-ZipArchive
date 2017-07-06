#pragma once
#include "cocos2d.h"
#include "unzip/unzip.h"

class ZipArchive
{
public:
	ZipArchive();
	~ZipArchive();

	static bool ZipToPath( const char* zip, const char* path, const char* password = "" );
private:
	static unzFile unzipOpenFile( const char* zip );

	static bool getGlobalInfo( unzFile zipFile );

	static int goToFirstFile( unzFile zipFile );

	static bool ZipArchive::openCurrFile(unzFile zipFile, const char* password = "");

	static bool getCurrFileInfo(unzFile zipFile,
		cocos2d::unz_file_info * fileInfo,
		char * szFileName = nullptr, uLong fileNameBufferSize = 0,
		void *extraField = nullptr, uLong extraFieldBufferSize = 0,
		char* szComment = nullptr, uLong commentBufferSize = 0);

	static std::string getCurrFileName( unzFile zipFile, const char* password = "" );

	static bool isDirectory(const char* path );

	static bool createDirectory(const char *path);

	static int getNextFile( unzFile zipFile );

	static void writeFile( const char* path, unzFile zipFile );

};

