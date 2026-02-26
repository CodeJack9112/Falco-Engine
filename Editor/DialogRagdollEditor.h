#pragma once

#include <OgreSceneNode.h>
#include <string>
#include <math.h>

#include "../Engine/CharacterJoint.h"

class BoneInfo
{
public:
	BoneInfo() = default;

	string name;

	SceneNode* anchor;
	CharacterJoint* joint;
	BoneInfo* parent;
	int direction = 0;

	Vector3 minLimit;
	Vector3 maxLimit;
	float swingLimit;

	Vector3 axis;
	Vector3 normalAxis;

	float radiusScale;
	std::string colliderType;

	std::vector<BoneInfo*> children;
	float density;
	float summedMass;// The mass of this and all children bodies
};

struct Bounds
{
private:
	Vector3 m_Center = Vector3::ZERO;
	Vector3 m_Extents = Vector3(1, 1, 1);
	
	Vector3 Vector3Min(Vector3 lhs, Vector3 rhs)
	{
		return Vector3(std::min(lhs.x, rhs.x), std::min(lhs.y, rhs.y), std::min(lhs.z, rhs.z));
	}

	Vector3 Vector3Max(Vector3 lhs, Vector3 rhs)
	{
		return Vector3(std::max(lhs.x, rhs.x), std::max(lhs.y, rhs.y), std::max(lhs.z, rhs.z));
	}

public:
	Bounds() = default;

	Vector3 getCenter() { return m_Center; }
	Vector3 getExtents() { return m_Extents; }

	void setCenter(Vector3 value) { m_Center = value; }
	void setExtents(Vector3 value) { m_Extents = value; }

	Vector3 getSize()
	{
		return m_Extents * 2.0f;
	}

	void setSize(Vector3 value)
	{
		m_Extents = value * 0.5f;
	}

	Vector3 getMin()
	{
		return m_Center - m_Extents;
	}

	void setMin(Vector3 value)
	{
		SetMinMax(value, getMax());
	}

	Vector3 getMax()
	{
		return m_Center + m_Extents;
	}

	void setMax(Vector3 value)
	{
		SetMinMax(getMin(), value);
	}

	void SetMinMax(Vector3 min, Vector3 max)
	{
		m_Extents = (max - min) * 0.5f;
		m_Center = min + m_Extents;
	}

	Bounds(Vector3 center, Vector3 size)
	{
		m_Center = center;
		m_Extents = size * 0.5f;
	}

	void Encapsulate(Vector3 point)
	{
		SetMinMax(Vector3Min(getMin(), point), Vector3Max(getMax(), point));
	}

	void Encapsulate(Bounds bounds)
	{
		Encapsulate(bounds.m_Center - bounds.m_Extents);
		Encapsulate(bounds.m_Center + bounds.m_Extents);
	}

	void Expand(float amount)
	{
		amount *= 0.5f;
		m_Extents += Vector3(amount, amount, amount);
	}

	void Expand(Vector3 amount)
	{
		m_Extents += amount * 0.5f;
	}
};

class DialogRagdollEditor
{
public:
	DialogRagdollEditor();
	~DialogRagdollEditor();

	void show();
	void update();

private:
	bool visible = false;

	float massValue = 70.0f;
	float strength = 0.0F;

	Vector3 right = Vector3(1, 0, 0);
	Vector3 up = Vector3(0, 1, 0);
	Vector3 forward = Vector3(0, 0, 1);

	Vector3 worldRight = Vector3(1, 0, 0);
	Vector3 worldUp = Vector3(0, 1, 0);
	Vector3 worldForward = Vector3(0, 0, 1);
	bool flipForward = false;

	std::vector<BoneInfo*> bones;
	BoneInfo * rootBone;

	Ogre::SceneNode* pelvisObject = nullptr;
	Ogre::SceneNode* lhipObject = nullptr;
	Ogre::SceneNode* lkneeObject = nullptr;
	Ogre::SceneNode* rhipObject = nullptr;
	Ogre::SceneNode* rkneeObject = nullptr;
	Ogre::SceneNode* larmObject = nullptr;
	Ogre::SceneNode* lelbowObject = nullptr;
	Ogre::SceneNode* rarmObject = nullptr;
	Ogre::SceneNode* relbowObject = nullptr;
	Ogre::SceneNode* mspineObject = nullptr;
	Ogre::SceneNode* headObject = nullptr;

	void acceptDragDrop(Ogre::SceneNode *& sceneNode);
	void createRagdoll();

	string CheckConsistency();
	void DecomposeVector(Vector3& normalCompo, Vector3& tangentCompo, Vector3 outwardDir, Vector3 outwardNormal);
	void CalculateAxes();
	void PrepareBones();
	BoneInfo * FindBone(string name);
	void AddMirroredJoint(string name, Ogre::SceneNode * leftAnchor, Ogre::SceneNode* rightAnchor, string parent, Vector3 worldTwistAxis, Vector3 worldSwingAxis, Vector3 minLimit, Vector3 maxLimit, float swingLimit, std::string colliderType, float radiusScale, float density);
	void AddJoint(string name, Ogre::SceneNode* anchor, string parent, Vector3 worldTwistAxis, Vector3 worldSwingAxis, Vector3 minLimit, Vector3 maxLimit, float swingLimit, std::string colliderType, float radiusScale, float density);
	void BuildCapsules();
	void Cleanup(SceneNode * root);
	void BuildBodies();
	void BuildJoints();
	void CalculateMassRecurse(BoneInfo * bone);
	void CalculateMass();
	static void CalculateDirection(Vector3 point, int & direction, float & distance);
	static Vector3 CalculateDirectionAxis(Vector3 point);
	static int SmallestComponent(Vector3 point);
	static int LargestComponent(Vector3 point);
	static int SecondLargestComponent(Vector3 point);
	Bounds Clip(Bounds bounds, Ogre::SceneNode* relativeTo, Ogre::SceneNode* clipTransform, bool below);
	Bounds GetBreastBounds(Ogre::SceneNode* relativeTo);
	void AddBreastColliders();
	void AddHeadCollider();
};

