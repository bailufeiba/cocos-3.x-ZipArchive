#include "ZipArchive.h"

ZipArchive::ZipArchive()
{
}


ZipArchive::~ZipArchive()
{
}

bool ZipArchive::ZipToPath(const char* zip, const char* path, const char* password /*= ""*/)
{
	unzFile zipFile = ZipArchive::unzipOpenFile(zip);
	if (zipFile == nullptr)
		return false;

	int ret = 0;
	do
	{
		std::string filename = ZipArchive::getCurrFileName( zipFile, password );
		if (filename.compare("") == 0)
		{
			cocos2d::log("ZipArchive::ZipToPath getCurrFileName failt !");
			return false;
		}
		cocos2d::log("ZipArchive::ZipToPath getCurrFileName filename = %s", filename.c_str());

		std::string fullPath = std::string(path) + cocos2d::StringUtils::format("%s", filename.c_str());
		if ( ZipArchive::isDirectory( fullPath.c_str() ) )
		{
			createDirectory(fullPath.c_str());
			ret = ZipArchive::getNextFile(zipFile);
			continue;
		}

		ZipArchive::writeFile( fullPath.c_str(), zipFile );

		ret =ZipArchive::getNextFile(zipFile);
	} while ( ret == UNZ_OK );

	cocos2d::unzClose(zipFile);
	cocos2d::log("ZipArchive::ZipToPath finished !");
	return true;
}

unzFile ZipArchive::unzipOpenFile(const char* zip)
{
	std::string zipFullPath = cocos2d::FileUtils::getInstance()->fullPathForFilename(zip);
	unzFile zipFile = cocos2d::unzOpen(zipFullPath.c_str());
	if( zipFile )
	{
		if (!ZipArchive::getGlobalInfo(zipFile))
			return nullptr;

		if (!ZipArchive::goToFirstFile(zipFile))
			return nullptr;

		return zipFile;
	}
	else
	{
		cocos2d::log("ZipArchive::unzipOpenFile open zip failt !");
		return nullptr;
	}
}

bool ZipArchive::getGlobalInfo(unzFile zipFile)
{
	cocos2d::unz_global_info globalInfo = { 0 };
	if (cocos2d::unzGetGlobalInfo(zipFile, &globalInfo) == UNZ_OK)
	{
		cocos2d::log("ZipArchive::getGlobalInfo unzipOpenFile %lu  entries in the zip file", globalInfo.number_entry);
		return true;
	}
	else
	{
		cocos2d::unzClose( zipFile );
		cocos2d::log("ZipArchive::getGlobalInfo unzipOpenFile failt" );
		return false;
	}
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

int ZipArchive::goToFirstFile(unzFile zipFile)
{
	int ret = cocos2d::unzGoToFirstFile(zipFile);
	if (ret != UNZ_OK)
	{
		cocos2d::unzClose(zipFile);
		cocos2d::log("ZipArchive::goToFirstFile goto first file failt !");
		return false;
	}
	return true;
}

bool ZipArchive::openCurrFile(unzFile zipFile, const char* password/* = ""*/)
{
	int ret = 0;
	if (strcmp(password, "") == 0)
		ret = cocos2d::unzOpenCurrentFile(zipFile);
	else
		ret = cocos2d::unzOpenCurrentFilePassword(zipFile, password);

	if (ret != UNZ_OK)
	{
		cocos2d::unzClose(zipFile);
		cocos2d::log("ZipArchive::openCurrFile open current file failt !");
		return false;
	}

	return true;
}

bool ZipArchive::getCurrFileInfo(unzFile zipFile,
	cocos2d::unz_file_info * fileInfo,
	char * szFileName, uLong fileNameBufferSize,
	void *extraField, uLong extraFieldBufferSize,
	char* szComment, uLong commentBufferSize)
{
	int ret = cocos2d::unzGetCurrentFileInfo(zipFile, fileInfo, szFileName, fileNameBufferSize, extraField, extraFieldBufferSize, szComment, commentBufferSize);
	if (ret != UNZ_OK)
	{
		cocos2d::unzCloseCurrentFile(zipFile);
		cocos2d::unzClose(zipFile);
		cocos2d::log("ZipArchive::getCurrFileInfo unzGetCurrentFileInfo failt !");
		return false;
	}
	return true;
}

std::string ZipArchive::getCurrFileName( unzFile zipFile, const char* password)
{
	if (!ZipArchive::openCurrFile(zipFile, password))
		return "";

	cocos2d::unz_file_info fileInfo = { 0 };
	if (!ZipArchive::getCurrFileInfo(zipFile, &fileInfo))
		return "";

	char* szFileName = new char[fileInfo.size_filename + 1];
	bool bSucceed = ZipArchive::getCurrFileInfo(zipFile, &fileInfo, szFileName, fileInfo.size_filename + 1, nullptr, 0, nullptr, 0);

	std::string strname = "";
	if (bSucceed)
	{
		szFileName[fileInfo.size_filename] = '\0';
		strname = std::string(szFileName);
	}
	delete[] szFileName;

	return strname;
}

bool ZipArchive::isDirectory(const char* path)
{
	int len = strlen( path );

	if (path[len - 1] == '/')
		return true;

	if ( path[len - 1] == '\\' )
		return true;

	return false;
}

int ZipArchive::getNextFile(unzFile zipFile)
{
	cocos2d::unzCloseCurrentFile(zipFile);
	return cocos2d::unzGoToNextFile(zipFile);
}

void ZipArchive::writeFile(const char* path, unzFile zipFile)
{
	FILE* fp = fopen(path, "wb");
	int nRead = 0;
	unsigned char buffer[4096] = { 0 };
	while (fp)
	{
		nRead = cocos2d::unzReadCurrentFile(zipFile, buffer, 4096);
		if (nRead > 0)
			fwrite(buffer, nRead, 1, fp);
		else if (nRead < 0)
			break;
		else
			break;
	}

	if (fp)
		fclose(fp);
}