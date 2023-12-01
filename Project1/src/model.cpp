#include "model.h"



aiMesh* g_mesh;

Model::Model()
{

}

Model::Model(const Model& copyModel)
{
    meshes = copyModel.meshes;
    directory = copyModel.directory;
    isVisible = copyModel.isVisible;
    isWireFrame = copyModel.isWireFrame;
    modelPath = copyModel.modelPath;

}

Model::Model( std::string const& path, bool isTextureFlip, bool isTransparancy, bool isCutOut)
           
{

    this->modelPath = path;
    this->isTextureFlipped = isTextureFlip;
    this->isTransparant = isTransparancy;
    this->isCutOut = isCutOut;
    loadModel(path);
    std::cout << path << std::endl;
}



void Model::Draw(Shader& shader)
{
    shader.Bind();
   
    shader.setMat4("model", transform.GetModelMatrix());
   
    if (!isVisible)
    {
        return;
    }
    for (unsigned int i = 0; i < meshes.size(); i++)
    {
        meshes[i]->SetTransparency(isTransparant);
        meshes[i]->SetCutOff(isCutOut);
        meshes[i]->meshDraw(shader);
    }
}

void Model::loadModel(std::string const& path)
{
    this->modelPath = path;
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
   
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) 
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    directory = path.substr(0, path.find_last_of('/'));

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
       
        g_mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(g_mesh, scene));
    }
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
    std::cout << " Loaded  Model file  : " << directory << " Mesh count : " << scene->mNumMeshes << std::endl;
}

std::shared_ptr<Mesh> Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
     std::vector<Vertex> vertices;
     std::vector<unsigned int> indices;
     std::vector<Texture*> textures;

    

    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; 
        // positions
        vector.x = mesh->mVertices[i].x ;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;

       

        // normals
        if (mesh->HasNormals())
        {
          
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) 
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;

            
        }
        else
        {
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        }

        if (mesh->HasVertexColors(0))
        {
            aiColor4D color = mesh->mColors[0][i];
            vertex.vRgb = glm::vec4(color.r, color.g, color.b,color.a);
        }
        else
        {
            vertex.vRgb = glm::vec4(1.0f, 1.0f, 1.0f,1.0f);
        }

        vertices.push_back(vertex);
    }

    



    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
   
    // 1. diffuse maps
    std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "material.diffuse");
    textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "material.specular");
    textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    if ( alphaMask && alphaMask->id != 0 )
    {
        textures.push_back(alphaMask);
        std::cout << "Alpha pushed : " << alphaMask->path << std::endl;
    }
    
    // return a mesh object created from the extracted mesh data
    return std::make_shared<Mesh>(vertices, indices, textures);
 }


std::vector<Texture*> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{


    std::vector<Texture*> textures;
    if (mat->GetTextureCount(type) == 0)
    {
        std::string path = "";
        switch (type)
        {
        case aiTextureType_DIFFUSE:
            path = "Textures/DefaultTextures/Default_Diffuse.png";
            break;
        case aiTextureType_SPECULAR:
            path = "Textures/DefaultTextures/Default_Specular.jpg";
            break;
        }

        Texture* defaultTexture = new Texture(path.c_str());
        defaultTexture->type = typeName;



        std::cout << "Texture loading : " << defaultTexture->path << std::endl;
        std::cout << "Texture type : " << defaultTexture->type << std::endl;
        textures.push_back(defaultTexture);
        textures_loaded.push_back(defaultTexture);
    }
    else
    {
        for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);

            /* if (this->isTextureFlipped)
             {
                 stbi_set_flip_vertically_on_load(true);

             }
             else
             {
                 stbi_set_flip_vertically_on_load(false);

             }*/

            bool skip = false;
            for (unsigned int j = 0; j < textures_loaded.size(); j++)
            {
                if (std::strcmp(textures_loaded[j]->path, str.C_Str()) == 0)
                {
                    textures.push_back(textures_loaded[j]);
                    skip = true;
                    break;
                }
            }
            if (!skip)
            {
                Texture* texture = new Texture();
                texture->id = texture->TextureFromFile(str.C_Str(), this->directory);
                texture->type = typeName;
                std::cout << "Texture Loaded: " << texture->type << std::endl;
                texture->path = str.C_Str();
                textures.push_back(texture);
                textures_loaded.push_back(texture);
            }
        }


    }
    return textures;
}

 Texture Model::LoadDefaultTexture(aiTextureType type, std::string typeName)
 {
     std::string path = "";
     switch (type)
     {
     case aiTextureType_DIFFUSE:
         path = "Textures/DefaultTextures/Default_Diffuse.png";
         break;
     case aiTextureType_SPECULAR:
         path = "Textures/DefaultTextures/Default_Specular.jpg";
         break;
     }

     Texture* defaultTexture = new Texture(path.c_str());
     defaultTexture->type = typeName.c_str();

     std::cout << "Default Texture Loaded: " << defaultTexture->path << std::endl;
     return *defaultTexture;
 }




