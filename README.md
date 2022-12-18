# LotusLib

This is a C++17 library for reading data from Warframe's files stored in the Cache.Windows folder. Data stored in these cache files contain compressed data in a directory-like structure. See below for an example to retrieve a file's data.

## What this is, and what is isn't

This will allow you to access files stored within the .toc and .cache files. That's it. It doesn't have any knowledge of the file types. It cannot distinguish between 3D Models, Textures, and Audio clips. But this library will retrieve and decompress the raw data for these files. 

## How to install

* Adding this project as a Git submodule is the easiest integration method for a project. `git submodule add https://github.com/Puxtril/LotusLib.git`
* The only required dependency is __spdlog__. To download it, execute `git submodule update --init --recursive`
* This builds using CMake. Ensure CMake is installed on your platform, then add these directives to CMakeLists.txt: `add_subdirectory(path/to/LotusLib)` and `include_directory(path/to/LotusLib/include)`. To compile into an executable, you will also need the `find_library()` directive for Oodle (see below).
* This library uses [Oodle for decompression.](www.radgametools.com/oodle.html) While it's possible to use a dynamic library, this utilizes a static Oodle library. To avoid copyright issues, the static library is not included in this repository. The easiest way to obtain the library is from Unreal Engine: download the engine via Epic Games Launcher and browse to _<InstallFolder>/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.5/lib_

## Documentation

Documentation can be found on [lotuslib.puxtril.com](https://lotuslib.puxtril.com)

Here's a quick example:

```cpp
#include "PackageCollection.h"
#include "Package.h"
#include "CachePairReader.h"
#include "FileNode.h"

using namespace LotusLib;

int main()
{
    PackageCollection<CachePairReader> collection("C:\\Steam\\steamapps\\common\\Warframe\\Cache.Windows", true);

    // Get the file entry for Excalibur's model
    const FileEntries::FileNode* excal = collection["Misc"][PackageTrioType::H]->getFileEntry("/Lotus/Characters/Tenno/Excalibur_body.fbx");
    
    // Get a total file count
    // Variables are extra verbose for clarity
    size_t fileCount = 0;
    for (std::pair<const std::string, Package<CachePairReader>>& pkg : collection)
    {
        for (std::shared_ptr<CachePairReader>& pair : std::get<1>(pkg))
        {
            for (const FileEntries::FileNode* file : *pair)
            {
                fileCount++;
            }
        }
    }
}
```
