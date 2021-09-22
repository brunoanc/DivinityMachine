#include <iostream>
#include <vector>
#include <filesystem>
#include <algorithm>
#include <cstdio>
#include <cstdint>
#include <cstring>
#include <cmath>

// Prevent structs from having padding
#pragma pack(push)
#pragma pack(1)
#include "dds.hpp"
#include "bimage.hpp"
#pragma pack(pop)

#define POWER_OF_TWO(x) (1 << x)

namespace fs = std::filesystem;

// Check if string ends with substring
bool endsWith(const std::string& fullString, const std::string& suffix)
{
    if (fullString.length() >= suffix.length()) {
        return 0 == fullString.compare(fullString.length() - suffix.length(), suffix.length(), suffix);
    }
    else {
        return false;
    }
}

int main(int argc, char **argv)
{
    std::cout << "DivinityMachine by PowerBall253\n" << std::endl;

    // Display help
    if (argc == 1 || strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--help") == 0) {
        std::cout << "Usage:\n";
        std::cout << "DivinityMachine [textures to convert]" << std::endl;
        return 0;
    }

    // Iterate through passed arguments
    for (int i = 1; i < argc; i++) {
        // Read from dds file
        FILE *ddsFile = fopen(argv[i], "rb+");

        if (ddsFile == nullptr) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for reading." << std::endl;
            continue;
        }

        fseek(ddsFile, 0, SEEK_END);
        size_t ddsFileSize = ftell(ddsFile);
        fseek(ddsFile, 0, SEEK_SET);

        if (ddsFileSize < 128) {
            std::cerr << "ERROR: " << argv[i] << " is not a valid DDS file." << std::endl;
            continue;
        }

        DDSHeader ddsHeader;
        size_t ddsTextureSize = ddsFileSize - sizeof(ddsHeader);
        auto *ddsTexture = new uint8_t[ddsTextureSize];
        
        fread(&ddsHeader, sizeof(ddsHeader), 1, ddsFile);
        fread(ddsTexture, 1, ddsTextureSize, ddsFile);
        fclose(ddsFile);

        // Check magic
        if (memcmp(ddsHeader.dwMagic, "DDS ", 4) != 0) {
            std::cerr << "ERROR: " << argv[i] << " is not a valid DDS file." << std::endl;
            continue;
        }

        // Get BIM header from DDS header data
        BIMHeader bimHeader;
        bimHeader.pixelWidth = ddsHeader.dwWidth;
        bimHeader.pixelHeight = ddsHeader.dwHeight;
        bimHeader.mipCount = ddsHeader.dwMipMapCount;

        // Get the texture format
        // TODO: Add more texture formats
        if (memcmp(&ddsHeader.ddspf.dwFourCC, "DXT1", 4) == 0) {
            bimHeader.textureFormat = FMT_BC1_SRGB;
        }
        else if (memcmp(&ddsHeader.ddspf.dwFourCC, "ATI2", 4) == 0) {
            bimHeader.textureFormat = FMT_BC5;
        }
        else {
            std::cerr << "ERROR: " << argv[i] << " has an unsupported DDS format." << std::endl;
            continue;
        }

        // Get texture material type
        std::string ddsFileName = fs::path(argv[i]).filename().string();
        ddsFileName = ddsFileName.substr(0, ddsFileName.find_first_of(".$")); // Strip extensions and $ properties

        if (endsWith(ddsFileName, "_n") || endsWith(ddsFileName, "_Normal")) {
            bimHeader.textureMaterialKind = TMK_NORMAL;
        }
        else if (endsWith(ddsFileName, "_s")) {
            bimHeader.textureMaterialKind = TMK_SPECULAR;
        }
        else if (endsWith(ddsFileName, "_g")) {
            bimHeader.textureMaterialKind = TMK_SMOOTHNESS;
        }
        else if (endsWith(ddsFileName, "_e")) {
            bimHeader.textureMaterialKind = TMK_BLOOMMASK;
        }
        else if (endsWith(ddsFileName, "_h")) {
            bimHeader.textureMaterialKind = TMK_HEIGHTMAP;
        }
        else if (endsWith(ddsFileName, "_sss")) {
            bimHeader.textureMaterialKind = TMK_SSSMASK;
        }
        else {
            bimHeader.textureMaterialKind = TMK_ALBEDO;
        }

        // Get mipmaps
        std::vector<BIMMipMap> bimMipMaps(bimHeader.mipCount);

        for (int j = 0; j < bimHeader.mipCount; j++) {
            auto& mipMap = bimMipMaps[j];

            mipMap.mipLevel = j;
            mipMap.mipPixelWidth = ceil(static_cast<double>(bimHeader.pixelWidth) / POWER_OF_TWO(j));
            mipMap.mipPixelHeight = ceil(static_cast<double>(bimHeader.pixelHeight) / POWER_OF_TWO(j));

            // Formula from https://docs.microsoft.com/en-us/windows/win32/direct3ddds/dds-file-layout-for-textures
            mipMap.decompressedSize = std::max(1, (mipMap.mipPixelWidth + 3) / 4)
                * std::max(1, (mipMap.mipPixelHeight + 3) / 4) * (bimHeader.textureFormat == 0x21 ? 8 : 16);
            mipMap.compressedSize = mipMap.decompressedSize;

            mipMap.cumulativeSizeStreamDB = j == 0 ? 0 : bimMipMaps[j - 1].cumulativeSizeStreamDB + bimMipMaps[j - 1].decompressedSize;
        }

        // Write new BIM file
        const std::string exportedTgaFileName = fs::path(argv[i]).replace_extension(".tga").string();
        FILE *exportedTgaFile = fopen(exportedTgaFileName.c_str(), "wb");

        if (exportedTgaFile == nullptr) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for writing." << std::endl;
            continue;
        }

        fwrite(&bimHeader, sizeof(bimHeader), 1, exportedTgaFile);
        fwrite(bimMipMaps.data(), sizeof(BIMMipMap), bimMipMaps.size(), exportedTgaFile);

        if (bimHeader.textureFormat == FMT_BC5) {
            const uint8_t endBytes[16] = { 0x87, 0x86, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24, 0x86, 0x85, 0x49, 0x92, 0x24, 0x49, 0x92, 0x24 };
            fwrite(ddsTexture, 1, ddsTextureSize - 16, exportedTgaFile);
            fwrite(endBytes, 1, sizeof(endBytes), exportedTgaFile);
        }
        else {
            const uint8_t nullBytes[4] = { 0 };
            fwrite(ddsTexture, 1, ddsTextureSize - 4, exportedTgaFile);
            fwrite(nullBytes, 1, sizeof(nullBytes), exportedTgaFile); 
        }

        std::cout << "Successfully converted " << argv[i] << " to TGA." << std::endl;
    }
}
