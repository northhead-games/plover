#include "AssetLoader.h"
#include <iostream>
#include <cmath>

AssetLoader::AssetLoader() {
	assets = new std::ifstream("../resources/assets/assets.plv", std::ifstream::in | std::ifstream::binary);
	assets->read((char*)&hash, HASH_SIZE);

	for (size_t i = 0; i < hash.assetCount; i++)
	{
		Entry entry{};
		assets->read((char*)&entry, ENTRY_SIZE);
		
		tableOfContents.insert({entry.hash, entry});
	}
}

char* AssetLoader::loadTexture(std::string name, ImageInfo* info) {
	u64 assetHash = hashAsset(name);
	auto entryPair = tableOfContents.find(assetHash);

	if (entryPair != tableOfContents.end()) {
		Entry entry = entryPair->second;
		assets->seekg(entry.offset, std::ios_base::beg);
		assets->read((char*)info, sizeof(ImageInfo));
		char* texture = new char[entry.size - sizeof(ImageInfo)];
		assets->read(texture, entry.size - sizeof(ImageInfo));
		return texture;
	} else {
		std::cerr << "Asset " << name << " not found." << std::endl;
	}

	return NULL; 
}

u64 AssetLoader::hashAsset(std::string name) {
	u32 power = 1;
	u64 value = name.length();
	u64 assetLen = name.length();

	for (size_t i = 0; i < hash.startChars; i++) {
		value += (static_cast<u64>(name[i % assetLen]) - 33) * static_cast<u64>(std::pow(hash.prime, power++));
	}

	for (size_t i = 0; i < hash.endChars; i++) {
		value += (static_cast<u64>(name[(assetLen - i) % assetLen]) - 33) * static_cast<u64>(std::pow(hash.prime, power++));
	}

	return value;
}

AssetLoader::~AssetLoader() {

}
