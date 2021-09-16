#ifndef BIMAGE_HPP
#define BIMAGE_HPP

#include <cstdint>

// Texture material kind enum
enum textureMaterialKind_t
{
	TMK_NONE = 0x0,
	TMK_ALBEDO = 0x1,
	TMK_SPECULAR = 0x2,
	TMK_NORMAL = 0x3,
	TMK_SMOOTHNESS = 0x4,
	TMK_COVER = 0x5,
	TMK_SSSMASK = 0x6,
	TMK_COLORMASK = 0x7,
	TMK_BLOOMMASK = 0x8,
	TMK_HEIGHTMAP = 0x9,
	TMK_DECALALBEDO = 0xA,
	TMK_DECALNORMAL = 0xB,
	TMK_DECALSPECULAR = 0xC,
	TMK_LIGHTPROJECT = 0xD,
	TMK_PARTICLE = 0xE,
	TMK_UNUSED_1 = 0xF,
	TMK_UNUSED_2 = 0x10,
	TMK_LIGHTMAP = 0x11,
	TMK_UI = 0x12,
	TMK_FONT = 0x13,
	TMK_LEGACY_FLASH_UI = 0x14,
	TMK_LIGHTMAP_DIRECTIONAL = 0x15,
	TMK_BLENDMASK = 0x16,
	TMK_COUNT = 0x17
};

// BIM header
typedef struct BIMHeader
{		
	char signature[3] = { 'B', 'I', 'M' };
	uint8_t version = 0x15;
	int32_t textureType = 0;
	int32_t textureMaterialKind;
	int32_t pixelWidth;
	int32_t pixelHeight;
	int32_t depth = 0;
	int32_t mipCount;
	int64_t mipLevel = 0;				
	float unkFloat1 = 1;
	uint8_t boolIsEnvironmentMap = 0;
	int32_t textureFormat;
	int32_t always7 = 7;
	int32_t nullPadding = 0;
	int16_t atlasPadding = 0;
	uint8_t boolIsStreamed = 0;
	uint8_t unkBool = 0;
	uint8_t boolNoMips = 0;
	uint8_t boolFFTBloom = 0;
	int32_t streamDBMipCount = 0;
} BIMHeader;

// BIM mipmap
typedef struct BIMMipMap
{
	int64_t mipLevel;
	int32_t mipPixelWidth;
	int32_t mipPixelHeight;
	int32_t unknownFlag = 1;
	int32_t decompressedSize;
	int32_t flagIsCompressed = 0;
	int32_t compressedSize;
	int32_t cumulativeSizeStreamDB;		
} BIMMipMap;

#endif
