#include <iostream>
#include <vector>
#include <filesystem>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <cmath>

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

        if (ddsFile == NULL) {
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
        size_t ddsTextureSize = ddsFileSize - sizeof(ddsHeader) - 4;
        uint8_t *ddsTexture = new uint8_t[ddsTextureSize];
        
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
        bimHeader.textureFormat = 0x21; // TODO: Get texture format from file

        // Get texture material type
        std::string ddsFileName = fs::path(argv[i]).stem().string();

        if (endsWith(ddsFileName, "_n")) {
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

        for (int i = 0; i < bimHeader.mipCount; i++) {
            auto& mipMap = bimMipMaps[i];

            mipMap.mipLevel = i;
            mipMap.mipPixelWidth = ceil((double)bimHeader.pixelWidth / POWER_OF_TWO(i));
            mipMap.mipPixelHeight = ceil((double)bimHeader.pixelHeight / POWER_OF_TWO(i));

            const int decMipSize = mipMap.mipPixelWidth * mipMap.mipPixelHeight / 2; // TODO: Support other formats
            mipMap.decompressedSize = decMipSize > 8 ? decMipSize : 8; // Make it at least 8
            mipMap.compressedSize = mipMap.decompressedSize;

            mipMap.cumulativeSizeStreamDB = i == 0 ? 0 : bimMipMaps[i - 1].cumulativeSizeStreamDB + bimMipMaps[i - 1].decompressedSize;
        }

        // Write new BIM file
        const std::string exportedTgaFileName = fs::path(argv[i]).replace_extension(".tga").string();
        FILE *exportedTgaFile = fopen(exportedTgaFileName.c_str(), "wb");

        if (exportedTgaFile == NULL) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for writing." << std::endl;
            continue;
        }

        uint8_t nullBytes[4] = { 0 };
        fwrite(&bimHeader, sizeof(bimHeader), 1, exportedTgaFile);
        fwrite(bimMipMaps.data(), sizeof(BIMMipMap), bimMipMaps.size(), exportedTgaFile);
        fwrite(ddsTexture, 1, ddsTextureSize, exportedTgaFile);
        fwrite(nullBytes, 1, sizeof(nullBytes), exportedTgaFile);

        std::cout << "Succesfully converted " << argv[i] << " to TGA." << std::endl;
    }
}
