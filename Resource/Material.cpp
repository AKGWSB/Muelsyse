#include "Material.h"

Material::Material()
{
    
}

Material::~Material()
{

}

void Material::SetTexture(std::string name, Texture2D* tex)
{
    textures[name] = tex;
}