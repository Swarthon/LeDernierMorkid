<p align="center"><img src="https://github.com/Swarthon/LeDernierMorkid/blob/master/Media/Logo/logo.png?raw=true"></p>

<p align="center">An open source and free role-playing game</p>

---

## Introduction

This is the official repository of Le Dernier Morkid, an open source role-playing game written in C++.
It aims to provide a full open source game which would be free.

Yet is this project under development, I only work on it on my spare time, therefore is its evolution a bit slow.

## Branches

Branches | Information | Build Status
---------|-------------|-------------
**master** | the actual release version | [![Build Status](https://travis-ci.org/Swarthon/LeDernierMorkid.svg?branch=master)](https://travis-ci.org/Swarthon/LeDernierMorkid)
**OldVersion** | the old version, yet abandonned | [![Build Status](https://travis-ci.org/Swarthon/LeDernierMorkid.svg?branch=OldVersion)](https://travis-ci.org/Swarthon/LeDernierMorkid)
**v0-0** | version 0.0 | [![Build Status](https://travis-ci.org/Swarthon/LeDernierMorkid.svg?branch=v0-0)](https://travis-ci.org/Swarthon/LeDernierMorkid)
**v0-1** | version 0.1 | [![Build Status](https://travis-ci.org/Swarthon/LeDernierMorkid.svg?branch=v0-1)](https://travis-ci.org/Swarthon/LeDernierMorkid)
**v0-2** | version 0.2 | [![Build Status](https://travis-ci.org/Swarthon/LeDernierMorkid.svg?branch=v0-2)](https://travis-ci.org/Swarthon/LeDernierMorkid)

## Requirements

You will need a C++ compiler, see [Compiling](#compiling) section for more informations.  The project need the [OGRE library](http://ogre3d.org), which itself needs the [SDL2 library](http://libsdl.org/). To compile them, search on their website or use prebuilt files.

It has already been tested on Linux and Windows. I highly recommend to use the GNU Compiler (MinGW on Windows), with wich it has already been compiled.

## Dependencies

As written below in [Requirements](#requirement), this project has a few dependencies :
 - **Ogre3D**, version 2.1, the components you need are HlmsPbs and HlmsUnlit. The version 2.1 is not released yet but you can download it by doing `hg clone https://bitbucket.org/sinbad/ogre && cd ogre/ && hg pull && hg update v2-1`, assuming you have Mercurial.
 - **SDL2**, the library used by Ogre to get the input. This library is very common and you can download it either from you package manager either from [the official site](http://libsdl.org/)
 - **CEGUI**, version 0.8, a library which provide Ogre3D support for GUI. The version 0.8, which is yet only doesn't compile for Ogre 2.1, so I use a patch found [here](http://www.ogre3d.org/forums/viewtopic.php?f=25&t=82911&start=25), which gives [this link](https://pastebin.com/BB7CmUtw). To download the sources, the patch and apply it. `hg clone https://bitbucket.org/cegui/cegui && cd cegui/ && hg pull && hg update v0-8 && wget https://pastebin.com/raw/BB7CmUtw -O cegui.patch && patch -p1 < cegui.patch`, assuming you have Mercurial, wget and patch.

## Compiling

Before trying to compile the project itself, check if you have all the dependencies, see [Dependencies](#dependencies). To compile this code, you will need to run cmake. Use either `cmake-gui` or `cmake` to configure the project and then compile it. If you have trouble in doing it, just contact me, I will try to answer you as fast as possible, for more information see [Contacting](#contacting).

## Running

Once you compiled this program, move to the directory where you built it. Then run `LeDernierMorkid`.

## Bugs

Remember that this project is under development and bugs may be frequent. If you discover one, post an issue. You can also try to repair it yourself. You can contact me too, see [Contacting](#contacting)

## Contacting

In order to contact me, you can send me an email at swarthon.gokan@gmail.com.

## License

This program and the [logo](#logo) are released under the GPL v3 license. See LICENSE file.

## Logo

The logo has been designed and created by [24PaH](http://github.com/24PaH). Thanks to him !
