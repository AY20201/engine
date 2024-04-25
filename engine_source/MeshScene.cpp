#include"../engine_headers/MeshScene.h"

MeshScene::MeshScene(Transform transform, std::vector<Behavior*> behavior, std::vector<const char*> lodFilenames, Shader& shader, Material* material, bool separatedGOs, bool hasColliders, bool hasBoxColliders, bool triplanar, bool useMips)
{
	MeshScene::transform = transform;
	MeshScene::shader = shader;
	MeshScene::material = material;
	MeshScene::triplanar = triplanar;
	MeshScene::useMips = useMips;
	MeshScene::separatedGOs = separatedGOs;

	for (unsigned int i = 0; i < lodFilenames.size(); i++)
	{
		lodMeshes.push_back(std::vector<Mesh>{});
		ImportMeshes(lodFilenames[i], i);
	}
	if (!separatedGOs)
	{
		GameObject* instantiatedGO = new GameObject(transform.position, transform.rotation, transform.scale, lodMeshes[0], std::vector<CollisionMesh>{ }, behavior[0]);
		sceneGameObjects.push_back(instantiatedGO);

		if (hasColliders)
		{
			for (unsigned int i = 0; i < lodMeshes[lodMeshes.size() - 1].size(); i++)
			{
				Mesh sceneMesh = lodMeshes[lodMeshes.size() - 1][i];
				CollisionMesh newColMesh(sceneMesh.vertices, sceneMesh.indices, transform.matrix, sceneGameObjects[0], hasBoxColliders);
				sceneCollisionMeshes.push_back(newColMesh);
				instantiatedGO->collisionMeshes.push_back(newColMesh);
			}
		}
	}
	else
	{
		for (unsigned int i = 0; i < lodMeshes[0].size(); i++)
		{			
			GameObject* instantiatedGO = new GameObject(transform.position, transform.rotation, transform.scale, std::vector<Mesh>{ lodMeshes[0][i] }, std::vector<CollisionMesh>{ }, behavior[i]);
			sceneGameObjects.push_back(instantiatedGO);

			if (hasColliders)
			{
				Mesh sceneMesh = lodMeshes[lodMeshes.size() - 1][i];
				CollisionMesh newColMesh(sceneMesh.vertices, sceneMesh.indices, transform.matrix, sceneGameObjects[i], hasBoxColliders);
				sceneCollisionMeshes.push_back(newColMesh);
				instantiatedGO->collisionMeshes.push_back(newColMesh);
			}
		}
	}
}

void MeshScene::CreateCachedMesh(Transform transform, bool hasColliders)
{
	for (int i = 0; i < sceneGameObjects.size(); i++)
	{
		GameObject* existingGameObject = sceneGameObjects[i];
		GameObject* newGameObject = new GameObject(transform.position, transform.rotation, transform.scale, existingGameObject->meshes, existingGameObject->collisionMeshes, existingGameObject->behavior);
	
		/*
		if (hasColliders)
		{
			if (!separatedGOs)
			{
				for (int j = 0; j < sceneCollisionMeshes.size(); j++)
				{
					CollisionMesh newColMesh(sceneCollisionMeshes[j].vertices, sceneCollisionMeshes[j].indices, transform.matrix, sceneGameObjects[0], true);
				}
			}
			else
			{
				CollisionMesh newColMesh(sceneCollisionMeshes[i].vertices, sceneCollisionMeshes[i].indices, transform.matrix, sceneGameObjects[i], true);
			}
		}
		*/
	}

	
}


void MeshScene::ImportMeshes(const char* filename, unsigned int activeLod)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	if (scene)
	{
		if (material != nullptr)
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				InitializeMesh(activeLod, scene->mMeshes[i], material);
			}
		}
		else
		{
			if (sceneMaterials.size() == 0)
			{
				InitializeMaterials(scene, std::string(filename));
			}

			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				InitializeMesh(activeLod, scene->mMeshes[i], sceneMaterials[scene->mMeshes[i]->mMaterialIndex]);
			}
		}
	}
	else
	{
		std::cout << importer.GetErrorString() << std::endl;
	}
}

void MeshScene::InitializeMesh(unsigned int activeLod, const aiMesh* mesh, Material* material)
{
	std::vector<Vertex> vertices;
	std::vector<GLuint> indices;

	const aiVector3D Vec0(0.0f, 0.0f, 0.0f);

	for (unsigned int i = 0; i < mesh->mNumVertices; i++)
	{
		const aiVector3D pos = mesh->mVertices[i];
		const aiVector3D texcoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : Vec0;
		const aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[i] : Vec0;
		const aiVector3D tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : Vec0;

		Vertex newVert = Vertex{ 
			glm::vec3(pos.x, pos.y,  pos.z), 
			glm::vec2(texcoord.x, texcoord.y), 
			glm::vec3(normal.x, normal.y, normal.z),
			glm::vec3(tangent.x, tangent.y, tangent.z) 
		};

		vertices.push_back(newVert);
	}

	for (unsigned int i = 0; i < mesh->mNumFaces; i++)
	{
		const aiFace& face = mesh->mFaces[i];
		indices.push_back(face.mIndices[0]);
		indices.push_back(face.mIndices[1]);
		indices.push_back(face.mIndices[2]);
	}

	Mesh newMesh(vertices, indices, material, true, triplanar, true);
	lodMeshes[activeLod].push_back(newMesh);
}

void MeshScene::InitializeMaterials(const aiScene* scene, const std::string& filename)
{
	std::string directory = filename.substr(0, filename.find_last_of("/"));

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* aiMat = scene->mMaterials[i];

		Texture* albedo = InitializeTexture(aiMat, aiTextureType_DIFFUSE, directory);
		Texture* normalMap = InitializeTexture(aiMat, aiTextureType_HEIGHT, directory);
		//Texture* specularMap = InitializeTexture(aiMat, aiTextureType_SPECULAR, directory);

		aiColor3D diffuseColor(1.0, 1.0, 1.0);
		aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);
		aiColor3D ambientColor(1.0, 1.0, 1.0);
		aiMat->Get(AI_MATKEY_COLOR_AMBIENT, ambientColor);

		float textureScale = ambientColor.r;

		Material* newMaterial = new Material(shader, TexParam{ "albedo", albedo, textureScale, glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b) }, TexParam{ "normalMap", normalMap, textureScale }/*, TexParam{"specMap", specularMap, 1.0}*/, false);
		sceneMaterials.push_back(newMaterial);
	}
}

Texture* MeshScene::InitializeTexture(const aiMaterial* mat, aiTextureType type, std::string dir)
{
	if (mat->GetTextureCount(type) > 0)
	{
		aiString path;

		if (mat->GetTexture(type, 0, &path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS)
		{
			std::string fullPath = dir + std::string("/") + std::string(path.data);
			Texture* matTex = new Texture(fullPath.c_str(), GL_TEXTURE_2D, GL_LINEAR, GL_RGB, GL_UNSIGNED_BYTE, useMips);
			return matTex;
		}
	}
	else
	{
		switch (type)
		{
			case aiTextureType_DIFFUSE:
				return Texture::defaultAlbedo;
			case aiTextureType_HEIGHT:
				return Texture::defaultNormalMap;
			case aiTextureType_NORMALS:
				return Texture::defaultNormalMap;
			//case aiTextureType_SPECULAR:
				//return Texture::defaultSpecMap;
		}
	}

	return new Texture();
}
/*
std::vector<Mesh> MeshScene::ImportMeshScene(const char* filename)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

	const aiVector3D Vec0(0.0f, 0.0f, 0.0f);
	std::vector<Mesh> importedMeshes;
	std::vector<Material*> importedMaterials;

	std::string filenameStr = std::string(filename);
	std::string directory = filenameStr.substr(0, filenameStr.find_last_of("/"));

	for (unsigned int i = 0; i < scene->mNumMaterials; i++)
	{
		const aiMaterial* aiMat = scene->mMaterials[i];

		Texture* albedo = InitializeTexture(aiMat, aiTextureType_DIFFUSE, directory);
		Texture* normalMap = InitializeTexture(aiMat, aiTextureType_HEIGHT, directory);
		//Texture* specularMap = InitializeTexture(aiMat, aiTextureType_SPECULAR, directory);

		aiColor3D diffuseColor(1.0, 1.0, 1.0);
		aiMat->Get(AI_MATKEY_COLOR_DIFFUSE, diffuseColor);

		Shader dummyShader("../../engine_resource/Shaders/postprocess.vert", "../../engine_resource/Shaders/postprocess.frag");
		Material* newMaterial = new Material(dummyShader, TexParam{"albedo", albedo, 1.0, glm::vec3(diffuseColor.r, diffuseColor.g, diffuseColor.b)}, TexParam{"normalMap", normalMap, 1.0}/*, TexParam{"specMap", specularMap, 1.0}, false);
		importedMaterials.push_back(newMaterial);
	}

	for (unsigned int i = 0; i < scene->mNumMeshes; i++)
	{
		std::vector<Vertex> vertices;
		std::vector<GLuint> indices;

		aiMesh* mesh = scene->mMeshes[i];

		for (unsigned int i = 0; i < mesh->mNumVertices; i++)
		{
			const aiVector3D pos = mesh->mVertices[i];
			const aiVector3D texcoord = mesh->HasTextureCoords(0) ? mesh->mTextureCoords[0][i] : Vec0;
			const aiVector3D normal = mesh->HasNormals() ? mesh->mNormals[i] : Vec0;
			const aiVector3D tangent = mesh->HasTangentsAndBitangents() ? mesh->mTangents[i] : Vec0;

			Vertex newVert = Vertex{
				glm::vec3(pos.x, pos.y,  pos.z),
				glm::vec2(texcoord.x, texcoord.y),
				glm::vec3(normal.x, normal.y, normal.z),
				glm::vec3(tangent.x, tangent.y, tangent.z)
			};

			vertices.push_back(newVert);
		}

		for (unsigned int i = 0; i < mesh->mNumFaces; i++)
		{
			const aiFace& face = mesh->mFaces[i];
			indices.push_back(face.mIndices[0]);
			indices.push_back(face.mIndices[1]);
			indices.push_back(face.mIndices[2]);
		}

		Mesh newMesh(vertices, indices, importedMaterials[scene->mMeshes[i]->mMaterialIndex], true, false, true);
		importedMeshes.push_back(newMesh);
	}

	return importedMeshes;
}
*/
void MeshScene::Clear()
{
	for (int i = 0; i < sceneGameObjects.size(); i++)
	{	
		delete sceneGameObjects[i];
	}
	sceneGameObjects.clear();

	for (unsigned int i = 0; i < sceneMaterials.size(); i++)
	{
		//sceneMaterials[i]->Clear();
		delete sceneMaterials[i];
	}
	sceneMaterials.clear();
}