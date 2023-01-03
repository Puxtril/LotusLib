# LotusLib

This is a C++17 library for reading data from Warframe's files stored in the Cache.Windows folder. Data stored in these cache files contain compressed data in a directory-like structure. See below for an example to retrieve a file's data.

# General Overview

## What this is, and what is isn't

This will allow you to access files stored within the .toc and .cache files. That's it. It doesn't have any knowledge of the file types. It cannot distinguish between 3D Models, Textures, and Audio clips. But this library will retrieve and decompress the raw data for these files. 

## How to install

* Adding this project as a Git submodule is the easiest integration method for a project. `git submodule add https://github.com/Puxtril/LotusLib.git`
* The only required dependency is __spdlog__. To download it, execute `git submodule update --init --recursive`
* This builds using CMake. Ensure CMake is installed on your platform, then add these directives to CMakeLists.txt: `add_subdirectory(path/to/LotusLib)` and `include_directory(path/to/LotusLib/include)`. To compile into an executable, you will also need the `find_library()` directive for Oodle (see below).
* This library uses [Oodle for decompression.](www.radgametools.com/oodle.html). To avoid copyright issues, the static library is not included in this repository. The easiest way to obtain Oodle is from Unreal Engine: download Unreal via the Epic Games Launcher and browse to _<InstallFolder>/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.5/lib_

As a reminder: You do not need the static Oodle library to compile as a library, only to compile as an executable.

# Documentation

Documentation can be found on [lotuslib.puxtril.com](https://lotuslib.puxtril.com)

## How to use

1. Once you have everything installed, you need to create a PackageCollection to start reading files. You currently have 2 options for the CachePairs: _CachePairReader_ and _CachePairMeta_.
 * If you want to read file data, use _CachePairReader_. This requires cache files to be present.
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

Each interface has a linear iterator, making iteration very easy. 

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

## Great! What can I do with these files?

That's up to you! Within the cache files are 3D models, Textures, Audio files, Maps, AI scripts, Shaders, Animations, and much more. While you may see common file extensions like .fbx, .png, and .wav, they are certianly not stored in that format. Likely they are stored on the original servers in this format, but once packaged into the cache files, they are converted to something else. __Most__ files are stored in a custom format, so no existing program (like Blender) can read the data. You will need to write a converter to a standard format. This is not an easy task, and falls under the general term of "Reverse Engineering". Crack open your hex editor and have fun!

The folks on Xentax have already done some of this work. Personally I have written an extractor for 3D Models (some errors) and Textures (fully functional). I have not released this code, but plan on releasing in another open-source project. If you would like to assist, or would simply like the file definitions for these, please contact me on Discord or Email. I also have partial definitions for Animations and Maps.

Other than Reverse Engineering, you can simply collect metadata on the current cache. You can take snapshots, look at historical versions of Warframe and see how the data has changed. Warframe has gone undergone many revisions, so plenty to analyze there!

# Design of Warframe, and design of LotusLib

So you're not left wondering "Why the hell was it designed like this"

## Cache files

Looking at the Warframe cache folder, we see a bunch of .toc and .cache files. Toc files only contain a tree-heirarchy of files/folders. They contain file sizes and offsets of the binary data in the matching cache file. These files should always be paired together, so they are abstracted into the _CachePair_ class. Since there are multiple ways to interface with these files, _CachePair_ is a base class. For reading data in the cache file, use _CachePairReader_ and if you only have the toc file, use _CachePairMeta_. _CachePairMeta_ is slightly misleading because it doesn't require cache files.

_CachePair_s are the lowest interface the user should interact with, but to make accessing easier, containers are built on top of the _CachePair_ for easy, logical access. _Package_ represent the triple pairs of H, F, and B. Within a package, the triples share a name. _PackageCollection_ is a collection of _Package_s, indexed by their names.

Let's take an example file name. This is straight from the Cache.Windows folder:

H.Texture.toc
\- \-\-\-\-\-\-\- \-\-\-
|&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|
|&nbsp;&nbsp;&nbsp;&nbsp;|&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;|\-Table of Contents file. There will always be a matching .cache file.
|&nbsp;&nbsp;&nbsp;&nbsp;|
|&nbsp;&nbsp;&nbsp;&nbsp;|\-Package name. There is no defined limit of these.
|
|\-PackageTrioType. Always an H, usually a matching B and F, but both not required.

## Common Header

Most files in the H PackagePair will start with a CommonHeader struct. Within this struct is an enumeration that defines the content of the files definition in H, F, and B. 

## Decompression

Warframe uses 2 types of decompression: LZ and Oodle. Before their [Ensmallening update](https://forums.warframe.com/topic/1223735-the-great-ensmallening/), LZ was used as the main compression method. There is a custom LZ decompressor implementation (taken from a Xentax user). Post-ensmallening, everything (including non-textures), started using Oodle compression. Well, mostly. Files are not compressed in one block, the files can have multiple compressed blocks, and each of those blocks may use either compression method. 99% of the time it's oodle, but there is the rare occurrance of LZ compression.

Many extractors supporting Oodle still use the dynamic Oodle library to link against (oodlecore_9.dll). I find dynamic libraries a pain in the ass, especially for end-users if they need to source the file. Thus, the developer should be responsible for sourcing the static library.
