#ifndef DDSHEADER_HPP
#define DDSHEADER_HPP

#include <cstdint>

// DDS pixel format
typedef struct DDSPixelFormat {
    int32_t dwSize;
    int32_t dwFlags;
    int32_t dwFourCC;
    int32_t dwRGBBitCount;
    int32_t dwRBitMask;
    int32_t dwGBitMask;
    int32_t dwBBitMask;
    int32_t dwABitMask;
} DDSPixelFormat;

// DX10 header (for BC7 format)
typedef struct DDSHeaderDXT10 {
    int32_t dxgiFormat;
    int32_t resourceDimension;
    int32_t miscFlag;
    int32_t arraySize;
    int32_t miscFlags2;
} DDSHeaderDXT10;

// DDS header
typedef struct DDSHeader {
    char dwMagic[4];
    int32_t dwSize;
    int32_t dwFlags;
    int32_t dwHeight;
    int32_t dwWidth;
    int32_t dwPitchOrLinearSize;
    int32_t dwDepth;
    int32_t dwMipMapCount;
    int32_t dwReserved1[11];
    DDSPixelFormat ddspf;
    int32_t dwCaps;
    int32_t dwCaps2;
    int32_t dwCaps3;
    int32_t dwCaps4;
    int32_t dwReserved2;
} DDSHeader;

#endif
