example 1:
#include "ZipArchive.h"
ZipArchive::ZipToPath( "ziptest.zip", FileUtils::getInstance()->getWritablePath().c_str() );


if you use password in zip, please delete the code "#define NOUNCRYPT" in unzip.cpp on line 72
example 2:
#include "ZipArchive.h"
ZipArchive::ZipToPath( "ziptest.zip", FileUtils::getInstance()->getWritablePath().c_str(), "123" );