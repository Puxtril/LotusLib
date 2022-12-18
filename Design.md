# Design

So you're not left wondering "Why the hell was it designed like this"

## Cache files

Looking at the Warframe cache folder, we see a bunch of .toc and .cache files. In this library, the organization of these two files is abstracted into a _CachePair_; _Packages_ which represent the grouping of the 3 pairs F, B, and H; and a _PackageCollection_ represents the entire directory of _Package_s.

Example: `H.Texture.toc`

_PackageCollection_ is a dictionary indexed by strings, which are the package names. In the above example, "Texture" is the package name. Example: `packageCol["Texture"];`
_Package_ is an array, indexed by the grouping H, F, and B. The enumeration is found in 'Package.h'. In the above example, the PackageTrioType is `H`. Example: `package[LotusLib::PackageTrioType::H];`
_CachePair_ is not indexable, it represents the direct interface for the cache files. Thus, this is the most important class. To separate uses, _CachePair_ is a base class, so typical usage would be to use _CachePairReader_ or _CachePairMeta_. If you wanted to add another interface, it would be trivial.

The above classes _PackageCollection_ and _Package_ are templates, meant to be instanciated by a _CachePair_ subclass. The idea is to choose the apprpriate _CachePair_ subclass, then create _PackageCollection_ to provide an easy interface to access all files in a cache directory. Like so: `LotusLib::CachePairCollection<LotusLib::CachePairReader>`

## Decompression

Warframe uses 2 types of decompression: LZ and Oodle. Before their [Ensmallening update](https://forums.warframe.com/topic/1223735-the-great-ensmallening/), LZ was used as the main compression method. There is a custom LZ decompressor implementation (taken from a Xentax user). Post-ensmallening, everything (including non-textures), started using Oodle compression. Well, mostly. Files are not compressed in one block, the files can have multiple compressed blocks, and each of those blocks may use either compression method. 99% of the time it's oodle, but there is the rare occurrance of LZ compression.

Many extractors supporting Oodle still use the dynamic Oodle library to link against (oodlecore_9.dll). I find dynamic libraries a pain in the ass, especially for end-users if they need to source the file. Thus, the developer should be responsible for sourcing the static library.
