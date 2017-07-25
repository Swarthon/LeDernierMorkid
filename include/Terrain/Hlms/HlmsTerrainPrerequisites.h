/*
-----------------------------------------------------------------------------
This source file is part of OGRE
    (Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
-----------------------------------------------------------------------------
*/
#ifndef _HLMSTERRAINPREREQUISITES_H_
#define _HLMSTERRAINPREREQUISITES_H_

enum TerrainTextureTypes {
	TERRAIN_DIFFUSE,
	TERRAIN_DETAIL_WEIGHT,
	TERRAIN_DETAIL0,
	TERRAIN_DETAIL1,
	TERRAIN_DETAIL2,
	TERRAIN_DETAIL3,
	TERRAIN_DETAIL0_NM,
	TERRAIN_DETAIL1_NM,
	TERRAIN_DETAIL2_NM,
	TERRAIN_DETAIL3_NM,
	TERRAIN_DETAIL_ROUGHNESS0,
	TERRAIN_DETAIL_ROUGHNESS1,
	TERRAIN_DETAIL_ROUGHNESS2,
	TERRAIN_DETAIL_ROUGHNESS3,
	TERRAIN_DETAIL_METALNESS0,
	TERRAIN_DETAIL_METALNESS1,
	TERRAIN_DETAIL_METALNESS2,
	TERRAIN_DETAIL_METALNESS3,
	TERRAIN_REFLECTION,
	NUM_TERRAIN_TEXTURE_TYPES
};

class HlmsTerrain;

#endif
