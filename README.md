# LotusLib

This is a C++17 library for reading data from Evolution Engine game files.

Games supported:
* Warframe
* Soulframe
* Darkness II
* Star Trek (2013)
* **NOT** Dark Sector

# General Overview

## What this is, and what is isn't

It can access data stored within the .toc and .cache files. That's it. It cannot distinguish between 3D Models, Textures, and Audio clips. But this library will retrieve and decompress the raw data. 

## How to install

* Adding this project as a Git submodule is the easiest integration method for a project. `git submodule add https://github.com/Puxtril/LotusLib.git`
* Initilize all required submodules with `git submodule update --init --recursive LotusLib`.
* This builds using CMake. Ensure CMake is installed on your platform, then add this to your project's CMakeLists.txt: `add_subdirectory(path/to/LotusLib)`. When you create your executable, link with `target_link_libraries(<YOUR_EXE> LotusLib)`.
* Obtain Oodle
    * Option 1: Download from Github
        * Browse to `https://github.com/WorkingRobot/OodleUE/tree/main/Engine/Source/Runtime/OodleDataCompression/Sdks`
        * Click on the latest version
        * Download `lib/Linux/liboo2corelinux64.a` or `lib/Win64/oo2core_win64.lib`. If you wish to compile this project in Debug mode, download the debug binaries `lib/Linux/liboo2corelinux64_dbg.a` or `lib/Win64/oo2core_win64_debug.lib`
        * For the lazy, [Linux](https://github.com/WorkingRobot/OodleUE/raw/refs/heads/main/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.12/lib/Linux/liboo2corelinux64.a), [Linux-Debug](https://github.com/WorkingRobot/OodleUE/raw/refs/heads/main/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.12/lib/Linux/liboo2corelinux64_dbg.a), [Windows](https://github.com/WorkingRobot/OodleUE/raw/refs/heads/main/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.12/lib/Win64/oo2core_win64.lib), [Windows-Debug](https://github.com/WorkingRobot/OodleUE/raw/refs/heads/main/Engine/Source/Runtime/OodleDataCompression/Sdks/2.9.12/lib/Win64/oo2core_win64_debug.lib).
    * Option 2: Obtain directly from Unreal Engine
        * Download and install [Epic Launcher](https://store.epicgames.com)
        * Once downloaded find the SDK folder `Engine/Source/Runtime/OodleDataCompression/Sdks/X.X.X/lib`
    * Once downloaded, create a root folder in this project `bin` and place them inside. If I were on Linux, my folder structure would look like `bin/Linux/liboo2corelinux64.a` and `bin/Linux/liboo2corelinux64_dbg.a`.

## How to use

### Cache Files

Understanding the cache files will assist in understanding how to utilize this library. If you open the `Cache.Windows` directory inside your game installation, you'll see a bunch of files looking like this.

```
H.Texture.toc
- ------- ---
|    |     |
|    |     |- toc/cache are both represented by PackageSplit.
|    |
|    |- The package name. Source file is `Package`.
|
|- PackageSplit. The only 3 types are H(eader) B(ody) and F(ooter). Source file is `PackageSplit`.
```

The collection of these packages (the `Cache.Windows` directory) is represented as the `PackageCollection` object. So likely your first line of code will be creating an instance of `PackageCollection`.

### Virtual Files

Each Package contains virtual files that can be extracted, but they are split among the PackageSplits. For example, the virtual file `/Lotus/Characters/Tenno/Excalbiur/ExcaliburBody_skel.fbx` can be found inside the `Misc` Package and has data stored in the H/B/F splits. Other files like textures have data stored in the `Texture` package, also spread across the H/B/F splits.

Every virtual file has an entry in the H split. Inside the H split is an important structure - the CommonHeader. Every file will contain this structure, and the most important field is `type`. This is an enumeration that dictates the format of the remaining header data and the data stored inside the B/F splits (3D model/Texture/Shader/etc). (Note: Some files like DropTables are encrypted, and thus do not have a CommonHeader). The Reverse engineering of these file formats is what [Warframe-Exporter](https://github.com/Puxtril/Warframe-Exporter) does.

### Packages.bin

This is a special virtual file stored inside the `Misc` Package, specifically the H split. It's a complex Zstd-compressed structure that contains heirarchecal JSON-like data. Every virtual file stored inside packages has a corresponding entry inside `Packages.bin`, but not the reverse - `Packages.bin` contains many meta-files.

To read this data, read `/Packages.bin` from the `Misc` package and pass to the `PackagesBin` object's `initialize` method.

###  WARFRAME_PE?

Warframe changed compression methods from LZ to Oodle, dubbed "The Great Ensmallening", which took place late 2020 - early 2021. The change in compression methods changes the behavior of decompression, and I decided to represent this as a separate game altogether - "Warframe Pre-Ensmallening".

### The PackageCategory Enumeration

Over time, Warframe specifically has updated their package names - ex. `TextureDx9` -> `Texture`. Additionally, I've noticed overlap with format enumerations (stored inside the CommonHeader) between packages. It seems these enumerations are unique only within their own package category. So as an alternative to clients hard-coding both package names `TextureDx9` and `Texture`, you may match against `PackageCategory::TEXTURE` instead.


## Examples

<details>

<summary>Read all files in all packages</summary>

```cpp
#include "LotusLib/PackageCollection.h"

int main()
{
    const std::string cachePath = "C:\\Steam\\steamapps\\common\\Warframe\\Cache.Windows";

    LotusLib::Game game = LotusLib::guessGame(cachePath);
    LotusLib::PackageCollection pkgCollection(cachePath, game);

    for (LotusLib::Package& pkg : pkgCollection)
    {
        // Iterate over virtual files inside the H Split
        for (const LotusLib::FileNode& fileNode : pkg)
        {
            // Quickly get the CommonHeader
            LotusLib::CommonHeader cHeader = pkg.readCommonHeader(fileNode);

            // An even quicker method to read the format
            uint32_t cHeaderFormat = pkg.readCommonHeaderFormat(fileNode);

            // Get the full file entry - CommonHeader and H/B/F split
            LotusLib::FileEntry fullEntry = pkg.getFileEntry(fileNode);

            // Get the absolute path to the file
            std::string absPath = LotusLib::getFullPath(fileNode);
        }
    }
}
```

</details>


<details>

<summary>Load Packages.bin</summary>

```cpp
#include "LotusLib/PackageCollection.h"
#include "LotusLib/PackagesBin.h"

int main()
{
    const std::string cachePath = "C:\\Steam\\steamapps\\common\\Warframe\\Cache.Windows";

    LotusLib::Game game = LotusLib::guessGame(cachePath);
    LotusLib::PackageCollection pkgCollection(cachePath, game);

    LotusLib::Package miscPkg = pkgCollection.getPackage("Misc");
    std::vector<uint8_t> pkgsBinData = miscPkg.getFile(LotusLib::PkgSplitType::HEADER, "/Packages.bin");

    LotusLib::PackagesBin pkgsBin;
    pkgsBin.initilize(pkgsBinData);
    for (const auto& item : pkgsBin)
    {
        std::cout << "Entry path: " << item.first << std::endl;
        nlohmann::json params = pkgsBin.getParametersJson(item.first);
    }
}
```

</details>


## Misc

**Why not Dark Sector?**

The DS game files are very different from the games supported by this library. From my limited research, the `cache` files are just ZIP archives with a custom compression format. Theoretically I could squeeze DS support into this library, but IMO it doesn't make sense.