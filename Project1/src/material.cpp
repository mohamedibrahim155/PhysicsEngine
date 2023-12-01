#include "material.h"



Material::Material( float shininess)
{
   
    this->shininess = shininess;
   
   
}

Material::~Material()
{

    //need to do 
}
   

void Material::SetMaterialProperties(Shader& shader) 
{

    
    shader.setFloat("material.shininess", this->shininess);
    

}
