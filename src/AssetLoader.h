#pragma once
#include "lapwing.h"
#include <unordered_map>
#include <string>
#include <fstream>

struct AssetLoader {
	std::unordered_map<u64, Entry> tableOfContents;
	std::ifstream* assets;
	Hash hash;

	AssetLoader();

	char* loadTexture(std::string name, ImageInfo* info);
	u64 hashAsset(std::string name);

	~AssetLoader();
};