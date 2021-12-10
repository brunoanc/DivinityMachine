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

// Undefine max to prevent namespace conflicts with MSVC
#ifdef max
#undef max
#endif

// Use bitwise operation to get power of two
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

// Check if string contains substring
inline bool contains(const std::string& fullString, const std::string& subString)
{
    return fullString.find(subString) != std::string::npos;
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

    // Count failures
    int failures = 0;

    // Iterate through passed arguments
    for (int i = 1; i < argc; i++) {
        // Read from dds file
        FILE *ddsFile = fopen(argv[i], "rb+");

        if (ddsFile == nullptr) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for reading." << std::endl;
            failures += 1;
            continue;
        }

        fseek(ddsFile, 0, SEEK_END);
        size_t ddsFileSize = ftell(ddsFile);
        fseek(ddsFile, 0, SEEK_SET);

        if (ddsFileSize < sizeof(DDSHeader)) {
            std::cerr << "ERROR: " << argv[i] << " is not a valid DDS file." << std::endl;
            failures += 1;
            continue;
        }

        DDSHeader ddsHeader;
        size_t ddsTextureSize = ddsFileSize - sizeof(DDSHeader);
        bool isDXT10Format = false;

        // Read Standard DDSHeader
        fread(&ddsHeader, sizeof(DDSHeader), 1, ddsFile);

        // Check for DXT10 Header, read it if present
        if (memcmp(&ddsHeader.ddspf.dwFourCC, "DX10", 4) == 0) {
            isDXT10Format = true;
            DDSHeaderDXT10 ddsHeaderDXT10;
            fread(&ddsHeaderDXT10, sizeof(DDSHeaderDXT10), 1, ddsFile);
            ddsTextureSize -= sizeof(ddsHeaderDXT10);
        }

        // Read texture data
        auto *ddsTexture = new uint8_t[ddsTextureSize];
        fread(ddsTexture, 1, ddsTextureSize, ddsFile);
        fclose(ddsFile);

        // Check magic
        if (memcmp(ddsHeader.dwMagic, "DDS ", 4) != 0) {
            std::cerr << "ERROR: " << argv[i] << " is not a valid DDS file." << std::endl;
            failures += 1;
            continue;
        }

        // Get BIM header from DDS header data
        BIMHeader bimHeader;
        bimHeader.pixelWidth = ddsHeader.dwWidth;
        bimHeader.pixelHeight = ddsHeader.dwHeight;
        bimHeader.mipCount = ddsHeader.dwMipMapCount;

        // Get the texture format
        if (memcmp(&ddsHeader.ddspf.dwFourCC, "DXT1", 4) == 0) {
            bimHeader.textureFormat = FMT_BC1_SRGB;
        }
        else if (memcmp(&ddsHeader.ddspf.dwFourCC, "ATI2", 4) == 0) {
            bimHeader.textureFormat = FMT_BC5;
        }
        else if (isDXT10Format) {
            bimHeader.textureFormat = FMT_BC7;
        }
        else {
            std::cerr << "ERROR: " << argv[i] << " has an unsupported DDS format." << std::endl;
            failures += 1;
            continue;
        }

        // Get texture material type
        std::string ddsFileName = fs::path(argv[i]).filename().string();
        std::string strippedDollarSection = ddsFileName.substr(0, ddsFileName.find_first_of("$"));

        // For DXT10, don't strip extensions until after TMK check
        if (!isDXT10Format) {
            ddsFileName = strippedDollarSection.substr(0, ddsFileName.find_first_of("."));
        }
        
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
        else if (contains(ddsFileName, "mtlkind=ui")) {
            bimHeader.textureMaterialKind = TMK_UI;
        }
        else if (contains(ddsFileName, "mtlkind=decalnormal")) {
            bimHeader.textureMaterialKind = TMK_DECALNORMAL;
        }
        else if (contains(ddsFileName, "mtlkind=decalalbedo")) {
            bimHeader.textureMaterialKind = TMK_DECALALBEDO;
        }
        else if (contains(ddsFileName, "mtlkind=decalspecular")) {
            bimHeader.textureMaterialKind = TMK_DECALSPECULAR;
        }
        else if (contains(ddsFileName, "mtlkind=particle")) {
            bimHeader.textureMaterialKind = TMK_PARTICLE;
        }
        else if (isDXT10Format) {
            bimHeader.textureMaterialKind = TMK_NONE;
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
                * std::max(1, (mipMap.mipPixelHeight + 3) / 4) * (bimHeader.textureFormat == FMT_BC1_SRGB ? 8 : 16);
            mipMap.compressedSize = mipMap.decompressedSize;

            mipMap.cumulativeSizeStreamDB = j == 0 ? 0 : bimMipMaps[j - 1].cumulativeSizeStreamDB + bimMipMaps[j - 1].decompressedSize;
        }

        // Write new BIM file
        std::string exportedTgaFileName;

        // Check if the file uses .png extension
        if (endsWith(strippedDollarSection, ".png")) {
            exportedTgaFileName = fs::path(argv[i]).replace_extension(".png").string();
        }
        else {
            exportedTgaFileName = fs::path(argv[i]).replace_extension(".tga").string();
        }

        FILE *exportedTgaFile = fopen(exportedTgaFileName.c_str(), "wb");

        if (exportedTgaFile == nullptr) {
            std::cerr << "ERROR: Failed to open " << argv[i] << " for writing." << std::endl;
            failures += 1;
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

    return failures;
}
