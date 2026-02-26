#include "OgreIncludes.h"
#include "UIElement.h"

using namespace Ogre;

#pragma once
class RaycastGeometry
{
public:
	RaycastGeometry(SceneManager * mgr, Camera * cam);
	~RaycastGeometry();

	void SetTypeMask(uint32 m);
	void SetMask(uint32 m);
	MovableObject * RaycastFromPoint(Vector2 point, float &distance);
	MovableObject * RaycastFromPoint(Ray & ray, float &distance);
	MovableObject * RaycastFromPointGeometryOnly(Ray & ray, float &distance, float maxDistance, bool lightmapStaticOnly);

	static void GetMeshInformation(const MeshPtr mesh, size_t &vertex_count, Vector3* &vertices, size_t &index_count, unsigned long* &indices, const Vector3 &position, const Quaternion &orient, const Ogre::Vector3 &scale);

private:
	SceneManager * sceneManager;
	Camera * camera;

	uint32 typeMask = SceneManager::ENTITY_TYPE_MASK | SceneManager::FX_TYPE_MASK;
	uint32 mask = 1 << 1;
};

