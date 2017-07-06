# cocos-3.x-ZipArchive
Zip based on cocos 3.x

### example 1
```
#include "ZipArchive.h"
ZipArchive::ZipToPath( "ziptest.zip", FileUtils::getInstance()->getWritablePath().c_str() );
```


### example 2
`if you use password in zip, please delete the code "#define NOUNCRYPT" in unzip.cpp on line 72`
```
#include "ZipArchive.h"
ZipArchive::ZipToPath( "ziptest.zip", FileUtils::getInstance()->getWritablePath().c_str(), "123" );
```
