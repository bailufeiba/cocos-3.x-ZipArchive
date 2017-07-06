#include "ZipArchive.h"
#include "cocos2d.h"

ZipArchive::ZipArchive()
{
}


ZipArchive::~ZipArchive()
{
}

bool ZipArchive::ZipToPath(const char* zip, const char* path, const char* password /*= ""*/)
{
	std::string zipFullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(zip);
	unzFile zipFile = ZipArchive::unzipOpenFile(zipFullPath.c_str());
	if (zipFile == nullptr)
	{
		cocos2d::log( "ZipArchive::ZipToPath open zip failt !" );
		return false;
	}

	int ret = cocos2d::unzGoToFirstFile( zipFile );
	if( ret != UNZ_OK )
	{
		cocos2d::unzClose(zipFile);
		cocos2d::log("ZipArchive::ZipToPath goto first file failt !");
		return false;
	}

	do
	{
		if ( strcmp(password, "") == 0 )
			ret = cocos2d::unzOpenCurrentFile(zipFile);
		else
			ret = cocos2d::unzOpenCurrentFilePassword (zipFile, password );

		if ( ret != UNZ_OK )
		{
			cocos2d::unzClose(zipFile);
			cocos2d::log("ZipArchive::ZipToPath open current file failt !");
			return false;
		}

		cocos2d::unz_file_info fileInfo = { 0 };
		ret = unzGetCurrentFileInfo(zipFile, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0);
		if ( ret != UNZ_OK )
		{
			cocos2d::unzCloseCurrentFile(zipFile);
			cocos2d::unzClose( zipFile );
			cocos2d::log("ZipArchive::ZipToPath unzGetCurrentFileInfo failt !");
			return false;
		}

		char* filename = new char[fileInfo.size_filename + 1];
		cocos2d::unzGetCurrentFileInfo(zipFile, &fileInfo, filename, fileInfo.size_filename + 1, nullptr, 0, nullptr, 0);
		filename[fileInfo.size_filename] = '\0';
		cocos2d::log("ZipArchive::ZipToPath unzGetCurrentFileInfo filename = %s", filename );

		bool isDirectory = false;
		if (filename[fileInfo.size_filename - 1] == '/' || filename[fileInfo.size_filename - 1] == '\\')
			isDirectory = true;

		std::string fullPath = path + cocos2d::StringUtils::format("%s", filename);
		delete[] filename;
		if (isDirectory)
		{
			createDirectory(fullPath.c_str());
			cocos2d::unzCloseCurrentFile(zipFile);
			ret = cocos2d::unzGoToNextFile(zipFile);
			continue;
		}

		FILE* fp = fopen(fullPath.c_str(), "wb");
		int nRead = 0;
		unsigned char buffer[4096] = { 0 };
		while (fp) 
		{
			nRead = cocos2d::unzReadCurrentFile(zipFile, buffer, 4096);
			if (nRead > 0)
				fwrite(buffer, nRead, 1, fp);
			else if ( nRead < 0 )
				break;
			else
				break;
		}

		if (fp)
			fclose(fp);
		cocos2d::unzCloseCurrentFile(zipFile);
		ret = cocos2d::unzGoToNextFile(zipFile);
	} while (ret == UNZ_OK && UNZ_OK != UNZ_END_OF_LIST_OF_FILE);

	cocos2d::unzClose(zipFile);
	cocos2d::log("ZipArchive::ZipToPath finished !");
	return true;
}

unzFile ZipArchive::unzipOpenFile(const char* zip)
{
	unzFile zipFile = cocos2d::unzOpen( zip );
	if( zipFile )
	{
		cocos2d::unz_global_info globalInfo = { 0 };
		if ( cocos2d::unzGetGlobalInfo(zipFile, &globalInfo) == UNZ_OK )
			cocos2d::log("ZipArchive::unzipOpenFile %lu  entries in the zip file", globalInfo.number_entry);
	}
	return zipFile;
}

bool ZipArchive::createDirectory(const char *path)
{
#if (CC_TARGET_PLATFORM != CC_PLATFORM_WIN32)
	mode_t processMask = umask(0);
	int ret = mkdir(path, S_IRWXU | S_IRWXG | S_IRWXO);
	umask(processMask);
	if (ret != 0 && (errno != EEXIST))
	{
		return false;
	}

	return true;
#else
	BOOL ret = CreateDirectoryA(path, NULL);
	if (!ret && ERROR_ALREADY_EXISTS != GetLastError())
	{
		return false;
	}
	return true;
#endif
}