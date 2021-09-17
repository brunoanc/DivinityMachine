#ifndef BIMAGE_HPP
#define BIMAGE_HPP

#include <cstdint>

// Texture material kind enum
enum textureMaterialKind
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

// DDS texture formats used by bimage
enum textureFormat
{
  FMT_NONE 				= 0x0,
  FMT_RGBA32F 			= 0x1,
  FMT_RGBA16F			= 0x2,
  FMT_RGBA8 			= 0x3,
  FMT_ARGB8 			= 0x4,
  FMT_ALPHA 			= 0x5,
  FMT_L8A8_DEPRECATED 	= 0x6,
  FMT_RG8 				= 0x7,
  FMT_LUM8_DEPRECATED	= 0x8,
  FMT_INT8_DEPRECATED 	= 0x9,
  FMT_BC1 				= 0xA,
  FMT_BC3 				= 0xB,
  FMT_DEPTH 			= 0xC,
  FMT_DEPTH_STENCIL 	= 0xD,
  FMT_X32F 				= 0xE,
  FMT_Y16F_X16F			= 0xF,
  FMT_X16 				= 0x10,
  FMT_Y16_X16 			= 0x11,
  FMT_RGB565			= 0x12,
  FMT_R8 				= 0x13,
  FMT_R11FG11FB10F 		= 0x14,
  FMT_X16F 				= 0x15,
  FMT_BC6H_UF16			= 0x16,
  FMT_BC7 				= 0x17,
  FMT_BC4 				= 0x18,
  FMT_BC5 				= 0x19,
  FMT_RG16F 			= 0x1A,
  FMT_R10G10B10A2 		= 0x1B,
  FMT_RG32F 			= 0x1C,
  FMT_R32_UINT 			= 0x1D,
  FMT_R16_UINT 			= 0x1E,
  FMT_DEPTH16 			= 0x1F,
  FMT_RGBA8_SRGB 		= 0x20,
  FMT_BC1_SRGB			= 0x21,
  FMT_BC3_SRGB 			= 0x22,
  FMT_BC7_SRGB 			= 0x23,
  FMT_BC6H_SF16 		= 0x24,
  FMT_ASTC_4X4 			= 0x25,
  FMT_ASTC_4X4_SRGB 	= 0x26,
  FMT_ASTC_5X4 			= 0x27,
  FMT_ASTC_5X4_SRGB 	= 0x28,
  FMT_ASTC_5X5 			= 0x29,
  FMT_ASTC_5X5_SRGB 	= 0x2A,
  FMT_ASTC_6X5 			= 0x2B,
  FMT_ASTC_6X5_SRGB 	= 0x2C,
  FMT_ASTC_6X6 			= 0x2D,
  FMT_ASTC_6X6_SRGB 	= 0x2E,
  FMT_ASTC_8X5 			= 0x2F,
  FMT_ASTC_8X5_SRGB 	= 0x30,
  FMT_ASTC_8X6 			= 0x31,
  FMT_ASTC_8X6_SRGB 	= 0x32,
  FMT_ASTC_8X8 			= 0x33,
  FMT_ASTC_8X8_SRGB 	= 0x34,
  FMT_DEPTH32F 			= 0x35,
  FMT_BC1_ZERO_ALPHA 	= 0x36,
  FMT_NEXTAVAILABLE		= 0x37
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
