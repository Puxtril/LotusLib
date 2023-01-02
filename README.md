# LotusLib

This is a C++17 library for reading data from Warframe's files stored in the Cache.Windows folder. Data stored in these cache files contain compressed data in a directory-like structure. See below for an example to retrieve a file's data.

# General Overview

## What this is, and what is isn't

This will allow you to access files stored within the .toc and .cache files. That's it. It doesn't have any knowledge of the file types. It cannot distinguish between 3D Models, Textures, and Audio clips. But this library will retrieve and decompress the raw data for these files. 

## How to install

* Adding this project as a Git submodule is the easiest integration method for a project. `git submodule add https://github.com/Puxtril/LotusLib.git`
* The only required dependency is __spdlog__. To download it, execute `git submodule update --init --recursive`
* This builds using CMake. Ensure CMake is installed on your platform, then add these directives to CMakeLists.txt: `add_subdirectory(path/to/LotusLib)` and `include_directory(path/to/LotusLib/include)`. To compile into an executable, you will also need the `find_library()` directive for Oodle (see below).
* This library uses [Oodle for decompression.](www.radgametools.com/oodle.html) While it's possible to use a dynamic library, this utilizes a static Oodle library. To avoid copyright issues, the static library is not included in this repository. The easiest way to obtain the library is from Unreal Engine: download the engine via Epic Games Launcher and browse to _<InstallFolder>/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.5/lib_

# Documentation

Documentation can be found on [lotuslib.puxtril.com](https://lotuslib.puxtril.com)

## How to use

1. Once you have everything installed, you need to create a PackageCollection to start reading files. You have 2 options for the CachePairs: _CachePairReader_ and _CachePairMeta_.
* If you want to read file data, use _CachePairReader_. This requires .cache files to be present.
* If you only have the .toc files, use _CachePairMeta_. 
* If you have other needs, subclass _CachePair_ and create your own interface.

1. Determine the filepath of your _Cache.Windows_ folder. If you installed through Steam, it's most likely in "C:\Steam\steamapps\common\Warframe\Cache.Windows".
1. Determine if your installation is pre/post Ensmallening. This is pretty easy - [if the installation was updated after September 9th, 2020; it's post](https://forums.warframe.com/topic/1223735-the-great-ensmallening/).

Now you can create a collection like so:
```cpp
#include "PackageCollection.h"
#include "Package.h"
#include "CachePairReader.h"

using namespace LotusLib;

int main()
{
    PackageCollection<CachePairReader> collection("C:\\Steam\\steamapps\\common\\Warframe\\Cache.Windows", true);

    // Get the file entry for Excalibur's model
    const FileEntries::FileNode* excal = collection["Misc"][PackageTrioType::H]->getFileEntry("/Lotus/Characters/Tenno/Excalibur/ExcaliburBody_skel.fbx");
    
    // Get the header and body data for Excalibur's model.
    // Note the different _PackageTrioType_
    std::unique_ptr<char[]> rawHeadData = collection["Misc"][PackageTrioType::H]->getDataAndDecompress("/Lotus/Characters/Tenno/Excalibur/ExcaliburBody_skel.fbx");
    std::unique_ptr<char[]> rawBodyData = collection["Misc"][PackageTrioType::B]->getDataAndDecompress("/Lotus/Characters/Tenno/Excalibur/ExcaliburBody_skel.fbx");
}
```

## Iterating over files

Each interface has an std::vector iterator, making iteration very easy. 

Here's an example that counts every file in the archives:
```cpp
#include "PackageCollection.h"
#include "Package.h"
#include "CachePairReader.h"
#include "FileNode.h"

using namespace LotusLib;

int main()
{
    PackageCollection<CachePairReader> collection("C:\\Steam\\steamapps\\common\\Warframe\\Cache.Windows", true);

    
    // Get a total file count
    // Variables are extra verbose for clarity
    size_t fileCount = 0;
    for (Package<CachePairReader>& pkg : collection)
    {
        for (std::shared_ptr<CachePairReader>& pair : pkg)
        {
            for (const FileEntries::FileNode file : *pair)
            {
                fileCount++;
            }
        }
    }
}
```

# Design

So you're not left wondering "Why the hell was it designed like this"

## Cache files

Looking at the Warframe cache folder, we see a bunch of .toc and .cache files. In this library, the organization of these two files is abstracted into a _CachePair_; _Packages_ which represent the grouping of the 3 pairs F, B, and H; and a _PackageCollection_ represents the entire directory of _Package_s.

Let's take an example file name. This is straight from the Cache.Windows folder: `H.Texture.toc`


_PackageCollection_ is a dictionary indexed by strings, which are the package names. In the above example, "Texture" is the package name. Example: `packageCol["Texture"];`
_Package_ is an array, indexed by the grouping H, F, and B. The enumeration is found in 'Package.h'. In the above example, the PackageTrioType is `H`. Example: `package[LotusLib::PackageTrioType::H];`
_CachePair_ is not indexable, it represents the direct interface for the cache files. Thus, this is the most important class. To separate uses, _CachePair_ is a base class, so typical usage would be to use _CachePairReader_ or _CachePairMeta_. If you wanted to add another interface, it would be trivial.

The above classes _PackageCollection_ and _Package_ are templates, meant to be instanciated by a _CachePair_ subclass. The idea is to choose the apprpriate _CachePair_ subclass, then create _PackageCollection_ to provide an easy interface to access all files in a cache directory. Like so: `LotusLib::CachePairCollection<LotusLib::CachePairReader>`

## Decompression

Warframe uses 2 types of decompression: LZ and Oodle. Before their [Ensmallening update](https://forums.warframe.com/topic/1223735-the-great-ensmallening/), LZ was used as the main compression method. There is a custom LZ decompressor implementation (taken from a Xentax user). Post-ensmallening, everything (including non-textures), started using Oodle compression. Well, mostly. Files are not compressed in one block, the files can have multiple compressed blocks, and each of those blocks may use either compression method. 99% of the time it's oodle, but there is the rare occurrance of LZ compression.

Many extractors supporting Oodle still use the dynamic Oodle library to link against (oodlecore_9.dll). I find dynamic libraries a pain in the ass, especially for end-users if they need to source the file. Thus, the developer should be responsible for sourcing the static library.
