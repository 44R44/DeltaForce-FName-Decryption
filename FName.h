extern uintptr_t global_base_address;
extern std::unordered_map<uint32_t, std::string> FNameCache;
extern krnl_driver::c_driver* mem;

inline void DecryptFNameEntry(char* name, int nameLength, bool isWide) {
    if (!name || nameLength <= 0) return;
    
    int key;
    switch (nameLength % 9) {
    case 0: key = (nameLength + (nameLength & 0x1F) + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 1: key = (nameLength + (nameLength ^ 0xDF) + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 2: key = (nameLength + (nameLength | 0xCF) + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 3: key = (33 * nameLength + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 4: key = (nameLength + (nameLength >> 2) + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 5: key = (3 * (nameLength - (isWide ? 0 : 41))) | 0x7F; break;
    case 6: key = (nameLength + ((4 * nameLength) | 5) + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 7: key = (nameLength + ((nameLength >> 4) | 7) + (isWide ? 128 : 0x80)) | 0x7F; break;
    case 8: key = (nameLength + (nameLength ^ 0xC) + (isWide ? 128 : 0x80)) | 0x7F; break;
    default: key = (nameLength + (nameLength ^ 0x40) + (isWide ? 128 : 0x80)) | 0x7F; break;
    }
    
    if (isWide) {
        uint16_t* wide_name = reinterpret_cast<uint16_t*>(name);
        for (int i = 0; i < nameLength; i += 2) {
            wide_name[i] ^= key;
        }
    } else {
        for (int i = 0; i < nameLength; i++) {
            name[i] ^= key;
        }
    }
}

inline std::string Get_name(uint32_t ComparisonIndex) {
    auto it = FNameCache.find(ComparisonIndex);
    if (it != FNameCache.end()) {
        return it->second;
    }
    
    static const auto dwName = global_base_address + 0x1311DD40;
    
    auto chunkOffset = ComparisonIndex >> 18;
    auto nameOffset = ComparisonIndex & 0x3FFFF;

    auto blockPtr = mem->read<uintptr_t>(dwName + 8 * chunkOffset + 8);
    auto namePoolChunk = blockPtr + (2 * nameOffset);

    auto nameEntry = mem->read<uint16_t>(namePoolChunk);
    auto nameLength = nameEntry >> 6;
    auto isWide = nameEntry & 1;

    if (!nameLength || nameLength >= 256) {
        FNameCache[ComparisonIndex] = "";
        return "";
    }

    char name[256] = { 0 };
    for (int i = 0; i < nameLength * (isWide ? 2 : 1); i++) {
        name[i] = mem->read<char>(namePoolChunk + 2 + i);
    }

    DecryptFNameEntry(name, nameLength, isWide);
    std::string result = std::string(name, nameLength);
    FNameCache[ComparisonIndex] = result;
    return result;
}

inline std::string getClassName(uintptr_t actor) {
    if (!actor) return "null";
    
    uintptr_t object_class = mem->read<uintptr_t>(actor + 0x10); 
    if (!object_class) return "null";
    
    uint32_t ComparisonIndex = mem->read<uint32_t>(object_class + 0x24);
    return Get_name(ComparisonIndex);
}
