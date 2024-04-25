#ifndef MESHIMPORTER_CLASS_H
#define MESHIMPORTER_CLASS_H

#include<assimp/Importer.hpp>
#include<assimp/scene.h>
#include<assimp/postprocess.h>

#include"GameObject.h"
#include"CollisionMesh.h"
#include"CollisionSolver.h"

class MeshScene
{
	public:

		std::vector<std::vector<Mesh>> lodMeshes;// sceneMeshes;
		std::vector<Material*> sceneMaterials;

		std::vector<CollisionMesh> sceneCollisionMeshes;

		std::vector<GameObject*> sceneGameObjects;

		Transform transform;
		Shader shader;
		Material* material;

		bool triplanar = false;
		bool useMips = false;
		bool topLayer = false;
		bool separatedGOs;

		MeshScene(Transform transform, std::vector<Behavior*> behavior, std::vector<const char*> lodFilenames, Shader& shader, Material* material, bool separatedGOs = false, bool hasColliders = true, bool hasBoxColliders = true, bool triplanar = true, bool useMips = true);
		void ImportMeshes(const char* filename, unsigned int activeLod);
		void CreateCachedMesh(Transform transform, bool hasColliders);
		void Clear();

		//static std::vector<Mesh> ImportMeshScene(const char* filename); //just return the meshes, no materials

		//static std::vector<CollisionMesh> ImportCollisionMesh(const char* filename);
		//void ImportCollisionMeshes();

	private:

		void InitializeMesh(unsigned int activeLod, const aiMesh* mesh, Material* material);
		void InitializeMaterials(const aiScene* scene, const std::string& filename);
		Texture* InitializeTexture(const aiMaterial* mat, aiTextureType type, std::string dir);

		//void InitializeCollisionMesh(const aiMesh* mesh);

};

#endif

