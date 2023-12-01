#pragma once
#include "Shader.h"

#include "Textures.h"



class Material
{

private:

	
	
	float shininess;
	

public:
	
	


	Material(float shininess);
	~Material();

	Texture* diffuse; 
	Texture* specular;

	void SetMaterialProperties(Shader& shader) ;
	



};