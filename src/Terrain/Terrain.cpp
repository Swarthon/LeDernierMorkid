#include "Terrain/Terrain.h"
#include "Terrain/TerrainShadowMapper.h"

#include <OgreHardwarePixelBuffer.h>
#include <OgreImage.h>
#include <OgreTextureManager.h>

#include <Compositor/OgreCompositorChannel.h>
#include <Compositor/OgreCompositorManager2.h>
#include <Compositor/OgreCompositorWorkspace.h>
#include <OgreCamera.h>
#include <OgreMaterialManager.h>
#include <OgreRenderTexture.h>
#include <OgreSceneManager.h>
#include <OgreTechnique.h>

Terrain::Terrain()
                : mWidth(0u),
                  mDepth(0u),
                  mDepthWidthRatio(1.0f),
                  mSkirtSize(10.0f),
                  mInvWidth(1.0f),
                  mInvDepth(1.0f),
                  mXZDimensions(Ogre::Vector2::UNIT_SCALE),
                  mXZInvDimensions(Ogre::Vector2::UNIT_SCALE),
                  mXZRelativeSize(Ogre::Vector2::UNIT_SCALE),
                  mHeight(1.0f),
                  mTerrainOrigin(Ogre::Vector3::ZERO),
                  mBasePixelDimension(256u) {
}
//-----------------------------------------------------------------------------------
Terrain::~Terrain() {
}
//-----------------------------------------------------------------------------------
Ogre::Image Terrain::loadImage(const Ogre::String& imageName) {
	Ogre::Image image;

	image.load(imageName, Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

	mWidth           = image.getWidth();
	mDepth           = image.getHeight();
	mDepthWidthRatio = mDepth / (float) (mWidth);
	mInvWidth        = 1.0f / mWidth;
	mInvDepth        = 1.0f / mDepth;

	if (Ogre::PixelUtil::getComponentCount(image.getFormat()) != 1) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
		            "Only grayscale images supported! " + imageName,
		            "Terrain::createHeightmap");
	}

	return image;
}
//-----------------------------------------------------------------------------------
void Terrain::createHeightmap(Ogre::Image& image) {
	mHeightMap.resize(mWidth * mDepth);

	const float maxValue    = powf(2.0f, (float) image.getBPP()) - 1.0f;
	const float invMaxValue = 1.0f / maxValue;

	if (image.getBPP() == 8) {
		const Ogre::uint8* RESTRICT_ALIAS data = reinterpret_cast<Ogre::uint8 * RESTRICT_ALIAS>(image.getData());
		for (Ogre::uint32 y = 0; y < mDepth; ++y) {
			for (Ogre::uint32 x                = 0; x < mWidth; ++x)
				mHeightMap[y * mWidth + x] = (data[y * mWidth + x] * invMaxValue) * mHeight;
		}
	}
	else if (image.getBPP() == 16) {
		const Ogre::uint16* RESTRICT_ALIAS data = reinterpret_cast<Ogre::uint16 * RESTRICT_ALIAS>(
		        image.getData());
		for (Ogre::uint32 y = 0; y < mDepth; ++y) {
			for (Ogre::uint32 x                = 0; x < mWidth; ++x)
				mHeightMap[y * mWidth + x] = (data[y * mWidth + x] * invMaxValue) * mHeight;
		}
	}
	else if (image.getFormat() == Ogre::PF_FLOAT32_R) {
		const float* RESTRICT_ALIAS data = reinterpret_cast<float * RESTRICT_ALIAS>(image.getData());
		for (Ogre::uint32 y = 0; y < mDepth; ++y) {
			for (Ogre::uint32 x                = 0; x < mWidth; ++x)
				mHeightMap[y * mWidth + x] = data[y * mWidth + x] * mHeight;
		}
	}

	calculateOptimumSkirtSize();
}
//-----------------------------------------------------------------------------------
void Terrain::createHeightmapTexture(const Ogre::String& imageName, const Ogre::Image& image) {
	destroyHeightmapTexture();

	if (image.getBPP() != 8 && image.getBPP() != 16 && image.getFormat() != Ogre::PF_FLOAT32_R) {
		OGRE_EXCEPT(Ogre::Exception::ERR_INVALIDPARAMS,
		            "Texture " + imageName + "must be 8 bpp, 16 bpp, or 32-bit Float",
		            "GraphicsTerrain::createHeightmapTexture");
	}

	//const Ogre::uint8 numMipmaps = image.getNumMipmaps();
	const Ogre::uint8 numMipmaps = 0u;

	Ogre::String heightMapTexName = "TerrainHeightMapTex";
	if (!(mHeightMapTex = Ogre::TextureManager::getSingleton().getByName(heightMapTexName))) {
		mHeightMapTex = Ogre::TextureManager::getSingleton().createManual(
		        heightMapTexName,
		        Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
		        Ogre::TEX_TYPE_2D,
		        (uint) image.getWidth(),
		        (uint) image.getHeight(),
		        numMipmaps,
		        image.getFormat(),
		        Ogre::TU_STATIC_WRITE_ONLY);
		for (Ogre::uint8 mip = 0; mip <= numMipmaps; ++mip) {
			Ogre::v1::HardwarePixelBufferSharedPtr pixelBufferBuf = mHeightMapTex->getBuffer(0, mip);
			const Ogre::PixelBox&                  currImage      = pixelBufferBuf->lock(Ogre::Box(0, 0, pixelBufferBuf->getWidth(), pixelBufferBuf->getHeight()),
			                                                       Ogre::v1::HardwareBuffer::HBL_DISCARD);
			Ogre::PixelUtil::bulkPixelConversion(image.getPixelBox(0, mip), currImage);
			pixelBufferBuf->unlock();
		}
	}
}
//-----------------------------------------------------------------------------------
void Terrain::destroyHeightmapTexture(void) {
	if (!mHeightMapTex.isNull()) {
		Ogre::ResourcePtr resPtr = mHeightMapTex;
		Ogre::TextureManager::getSingleton().remove(resPtr);
		mHeightMapTex.setNull();
	}
}
//-----------------------------------------------------------------------------------
void Terrain::calculateOptimumSkirtSize(void) {
	mSkirtSize = std::numeric_limits<float>::max();

	const Ogre::uint32 basePixelDimension = mBasePixelDimension;
	const Ogre::uint32 vertPixelDimension = static_cast<Ogre::uint32>(mBasePixelDimension * mDepthWidthRatio);

	for (size_t y = vertPixelDimension - 1u; y < mDepth - 1u; y += vertPixelDimension) {
		const size_t ny = y + 1u;

		bool  allEqualInLine = true;
		float minHeight      = mHeightMap[y * mWidth];
		for (size_t x = 0; x < mWidth; ++x) {
			const float minValue = Ogre::min(mHeightMap[y * mWidth + x],
			                                 mHeightMap[ny * mWidth + x]);
			minHeight = Ogre::min(minValue, minHeight);
			allEqualInLine &= mHeightMap[y * mWidth + x] == mHeightMap[ny * mWidth + x];
		}

		if (!allEqualInLine)
			mSkirtSize = Ogre::min(minHeight, mSkirtSize);
	}

	for (size_t x = basePixelDimension - 1u; x < mWidth - 1u; x += basePixelDimension) {
		const size_t nx = x + 1u;

		bool  allEqualInLine = true;
		float minHeight      = mHeightMap[x];
		for (size_t y = 0; y < mDepth; ++y) {
			const float minValue = Ogre::min(mHeightMap[y * mWidth + x],
			                                 mHeightMap[y * mWidth + nx]);
			minHeight = Ogre::min(minValue, minHeight);
			allEqualInLine &= mHeightMap[y * mWidth + x] == mHeightMap[y * mWidth + nx];
		}

		if (!allEqualInLine)
			mSkirtSize = Ogre::min(minHeight, mSkirtSize);
	}

	mSkirtSize /= mHeight;
}
//-----------------------------------------------------------------------------------
inline GridPoint Terrain::worldToGrid(const Ogre::Vector3& vPos) const {
	GridPoint   retVal;
	const float fWidth = static_cast<float>(mWidth);
	const float fDepth = static_cast<float>(mDepth);

	retVal.x = (Ogre::uint32) floorf(((vPos.x - mTerrainOrigin.x) * mXZInvDimensions.x) * fWidth);
	retVal.z = (Ogre::uint32) floorf(((vPos.z - mTerrainOrigin.z) * mXZInvDimensions.y) * fDepth);

	return retVal;
}
//-----------------------------------------------------------------------------------
inline Ogre::Vector2 Terrain::gridToWorld(const GridPoint& gPos) const {
	Ogre::Vector2 retVal;
	const float   fWidth = static_cast<float>(mWidth);
	const float   fDepth = static_cast<float>(mDepth);

	retVal.x = (gPos.x / fWidth) * mXZDimensions.x + mTerrainOrigin.x;
	retVal.y = (gPos.z / fDepth) * mXZDimensions.y + mTerrainOrigin.z;

	return retVal;
}
//-----------------------------------------------------------------------------------
void Terrain::load(Ogre::Image& image, const Ogre::Vector3 center, const Ogre::Vector3& dimensions) {
	mTerrainOrigin      = center - dimensions * 0.5f;
	mXZDimensions       = Ogre::Vector2(dimensions.x, dimensions.z);
	mXZInvDimensions    = 1.0f / mXZDimensions;
	mHeight             = dimensions.y;
	mBasePixelDimension = 64u;
	createHeightmap(image);

	mXZRelativeSize = mXZDimensions / Ogre::Vector2(static_cast<Ogre::Real>(mWidth),
	                                                static_cast<Ogre::Real>(mDepth));
}
//-----------------------------------------------------------------------------------
bool Terrain::getHeightAt(Ogre::Vector3& vPos) const {
	bool      retVal = false;
	GridPoint pos2D  = worldToGrid(vPos);

	if (pos2D.x < mWidth - 1 && pos2D.z < mDepth - 1) {
		const Ogre::Vector2 vPos2D = gridToWorld(pos2D);

		const float dx = (vPos.x - vPos2D.x) * mWidth * mXZInvDimensions.x;
		const float dz = (vPos.z - vPos2D.y) * mDepth * mXZInvDimensions.y;

		float       a, b, c;
		const float h00 = mHeightMap[pos2D.z * mWidth + pos2D.x];
		const float h11 = mHeightMap[(pos2D.z + 1) * mWidth + pos2D.x + 1];

		c = h00;
		if (dx < dz) {
			//Ogre::Plane eq: y = ax + bz + c
			//x=0 z=0 -> c		= h00
			//x=0 z=1 -> b + c	= h01 -> b = h01 - c
			//x=1 z=1 -> a + b + c  = h11 -> a = h11 - b - c
			const float h01 = mHeightMap[(pos2D.z + 1) * mWidth + pos2D.x];

			b = h01 - c;
			a = h11 - b - c;
		}
		else {
			//Ogre::Plane eq: y = ax + bz + c
			//x=0 z=0 -> c		= h00
			//x=1 z=0 -> a + c	= h10 -> a = h10 - c
			//x=1 z=1 -> a + b + c  = h11 -> b = h11 - a - c
			const float h10 = mHeightMap[pos2D.z * mWidth + pos2D.x + 1];

			a = h10 - c;
			b = h11 - a - c;
		}

		vPos.y = a * dx + b * dz + c + mTerrainOrigin.y;
		retVal = true;
	}

	return retVal;
}
//-----------------------------------------------------------------------------------
