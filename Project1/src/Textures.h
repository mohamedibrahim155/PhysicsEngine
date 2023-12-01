#pragma once

#include <string>
#include "Renderer.h"
#include <vector>



#include <iostream>


class Texture
{
public:
	Texture() {}
	Texture(const char* filePath);
	~Texture() {}
	unsigned int id;
	const char* path;
	std::string type;
	void LoadTexture(const char* filePath, std::string _type);

	unsigned int TextureFromFile(const char* path, const std::string& directory);

	unsigned int loadCubemap(std::vector<std::string> faces);

private:
	unsigned int loadTexture(char const* path);
};