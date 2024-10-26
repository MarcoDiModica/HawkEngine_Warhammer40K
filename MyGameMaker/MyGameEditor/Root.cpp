#include "Root.h"


std::shared_ptr<GameObject> Root::CreateMeshObject(std::string path)
{
    //std::vector<MeshBufferedData> meshes = meshLoader->LoadMesh(path);
    //std::vector<std::shared_ptr<Texture>> textures = meshLoader->LoadTexture(path);

    //if (!meshes.empty())
    //{
    //    std::string name = path.substr(path.find_last_of("\\/") + 1, path.find_last_of('.') - path.find_last_of("\\/") - 1);

    //    //Take name before editing for meshData lookUp
    //    std::string folderName = "Library/Meshes/" + name + "/";

    //    name = GenerateUniqueName(name);

    //    // Create emptyGO parent if meshes >1
    //    bool isSingleMesh = meshes.size() > 1 ? false : true;
    //    std::shared_ptr<GameObject> emptyParent = isSingleMesh ? nullptr : CreateEmptyGO();
    //    if (!isSingleMesh) emptyParent.get()->SetName(name);

    //    std::vector<std::string> fileNames;

    //    uint fileCount = 0;

    //    for (const auto& entry : fs::directory_iterator(folderName))
    //    {
    //        if (fs::is_regular_file(entry))
    //        {
    //            std::string path = entry.path().filename().string();
    //            fileNames.push_back(entry.path().string());
    //            fileCount++;
    //        }
    //    }

    //    for (auto& mesh : meshes)
    //    {
    //        std::shared_ptr<GameObject> meshGO = std::make_shared<GameObject>(mesh.meshName);
    //        meshGO.get()->AddComponent<Transform>();
    //        meshGO.get()->AddComponent<Mesh>();
    //        //meshGO.get()->AddComponent<Texture>(); // hekbas: must implement

    //        meshGO.get()->GetComponent<Mesh>()->mesh = mesh;
    //        meshGO.get()->GetComponent<Mesh>()->mesh.texture = textures[mesh.materialIndex];
    //        //meshGO.get()->GetComponent<Texture>() = &meshGO.get()->GetComponent<Mesh>()->mesh.texture;

    //        //Load MeshData from custom files
    //        for (const auto& file : fileNames)
    //        {
    //            std::string fileName = file.substr(file.find_last_of("\\/") + 1, file.find_last_of('.') - file.find_last_of("\\/") - 1);
    //            if (fileName == mesh.meshName)
    //            {
    //                MeshData mData = meshLoader->deserializeMeshData(file);

    //                meshGO.get()->GetComponent<Mesh>()->meshData = mData;
    //                meshGO.get()->GetComponent<Mesh>()->path = file;
    //            }

    //        }

    //        // hekbas: need to set Transform?

    //        meshGO.get()->GetComponent<Mesh>()->GenerateAABB();

    //        if (isSingleMesh)
    //        {
    //            meshGO.get()->parent = rootSceneGO;
    //            rootSceneGO.get()->children.push_back(meshGO);
    //        }
    //        else
    //        {
    //            meshGO.get()->parent = emptyParent;
    //            emptyParent.get()->children.push_back(meshGO);

    //        }
    //    }

    //    /* if (!textures.empty())
    //    {
    //         if (meshes.size() == 1)
    //         {
    //             ComponentTexture* texture = (ComponentTexture*)GetComponent(ComponentType::TEXTURE);
    //             textures->setTexture((*meshes.begin())->texture);
    //             defaultTexture = texture->getTexture()->path;
    //         }
    //         else
    //         {
    //             for (auto i = meshes.begin(); i != meshes.end(); ++i)
    //             {
    //                 ComponentTexture* texturePart = (ComponentTexture*)GOPart->GetComponent(ComponentType::TEXTURE);
    //                 texturePart->setTexture((*i)->texture);
    //                 defaultTexture = texturePart->getTexture()->path;
    //             }
    //         }
    //     }
    // }*/


    

    return nullptr;
}