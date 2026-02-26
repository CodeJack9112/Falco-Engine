#include "stdafx.h"
#include "API_Transform.h"
#include "RigidBody.h"
#include "StringConverter.h"
#include <OgreSceneNode.h>

void API_Transform::getPosition(MonoObject * this_ptr, API::Vector3 * out_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody * body = (RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		Vector3 p = body->GetPosition();

		out_pos->x = p.x;
		out_pos->y = p.y;
		out_pos->z = p.z;
	}
	else
	{
		out_pos->x = node->_getDerivedPosition().x;
		out_pos->y = node->_getDerivedPosition().y;
		out_pos->z = node->_getDerivedPosition().z;
	}
}

void API_Transform::setPosition(MonoObject * this_ptr, API::Vector3 * ref_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody * body = (RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		node->_setDerivedPosition(Vector3(ref_pos->x, ref_pos->y, ref_pos->z));
		body->SetPosition(Vector3(ref_pos->x, ref_pos->y, ref_pos->z));
	}
	else
	{
		node->_setDerivedPosition(Vector3(ref_pos->x, ref_pos->y, ref_pos->z));
	}
}

void API_Transform::getScale(MonoObject * this_ptr, API::Vector3 * out_scale)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	out_scale->x = node->_getDerivedScale().x;
	out_scale->y = node->_getDerivedScale().y;
	out_scale->z = node->_getDerivedScale().z;
}

void API_Transform::setScale(MonoObject * this_ptr, API::Vector3 * ref_scale)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	node->setInheritScale(false);
	node->setScale(Vector3(ref_scale->x, ref_scale->y, ref_scale->z));
	node->setInheritScale(true);
}

void API_Transform::getRotation(MonoObject * this_ptr, API::Quaternion * out_rot)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody * body = (RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		Quaternion q = body->GetRotation();

		out_rot->x = q.x;
		out_rot->y = q.y;
		out_rot->z = q.z;
		out_rot->w = q.w;
	}
	else
	{
		Quaternion q = node->_getDerivedOrientation();

		out_rot->x = q.x;
		out_rot->y = q.y;
		out_rot->z = q.z;
		out_rot->w = q.w;
	}
}

void API_Transform::setRotation(MonoObject * this_ptr, API::Quaternion * ref_rot)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	RigidBody * body = (RigidBody*)node->GetComponent(RigidBody::COMPONENT_TYPE);

	if (body != nullptr)
	{
		node->_setDerivedOrientation(Quaternion(ref_rot->w, ref_rot->x, ref_rot->y, ref_rot->z));
		body->SetRotation(Quaternion(ref_rot->w, ref_rot->x, ref_rot->y, ref_rot->z));
	}
	else
	{
		node->_setDerivedOrientation(Quaternion(ref_rot->w, ref_rot->x, ref_rot->y, ref_rot->z));
	}
}

void API_Transform::getLocalPosition(MonoObject * this_ptr, API::Vector3 * out_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	out_pos->x = node->getPosition().x;
	out_pos->y = node->getPosition().y;
	out_pos->z = node->getPosition().z;
}

void API_Transform::setLocalPosition(MonoObject * this_ptr, API::Vector3 * ref_pos)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	node->setPosition(Vector3(ref_pos->x, ref_pos->y, ref_pos->z));
}

void API_Transform::getLocalRotation(MonoObject * this_ptr, API::Quaternion * out_rot)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	Quaternion q = node->getOrientation();

	out_rot->x = q.x;
	out_rot->y = q.y;
	out_rot->z = q.z;
	out_rot->w = q.w;
}

void API_Transform::setLocalRotation(MonoObject * this_ptr, API::Quaternion * ref_rot)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	node->setOrientation(Quaternion(ref_rot->w, ref_rot->x, ref_rot->y, ref_rot->z));
}

MonoObject * API_Transform::getGameObject(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node != nullptr)
		return node->gameObjectMono;

	return nullptr;
}

int API_Transform::getChildCount(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	return node->getChildren().size();
}

MonoObject * API_Transform::getParent(MonoObject * this_ptr)
{
	SceneNode * node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node->getParentSceneNode() != GetEngine->GetSceneManager()->getRootSceneNode())
		return node->getParentSceneNode()->transformMono;
	else
		return nullptr;
}

void API_Transform::setParent(MonoObject * this_ptr, MonoObject * parent)
{
	SceneNode * node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	SceneNode * parentNode;
	mono_field_get_value(parent, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&parentNode));

	Vector3 pos = node->_getDerivedPosition();
	Vector3 scl = node->getScale();
	Quaternion rot = node->_getDerivedOrientation();

	SceneNode * _parent = node->getParentSceneNode();

	scl *= _parent->_getDerivedScale();
	scl /= parentNode->_getDerivedScale();

	_parent->removeChild(node);
	parentNode->addChild(node);

	node->_setDerivedPosition(pos);
	node->setScale(scl);
	node->_setDerivedOrientation(rot);

	GetEngine->UpdateSceneNodeIndexes();
}

MonoObject* API_Transform::getChild(MonoObject* this_ptr, int index)
{
	SceneNode* node;

	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	if (node->getChildren().size() > index)
		return ((SceneNode*)node->getChild(index))->transformMono;

	return nullptr;
}

MonoObject* API_Transform::findChild(MonoObject* this_ptr, MonoString* name)
{
	SceneNode* node;
	mono_field_get_value(this_ptr, GetEngine->GetMonoRuntime()->transform_native_ptr_field, reinterpret_cast<void*>(&node));

	MonoObject* ret = nullptr;

	auto children = node->getChildren();

	for (auto it = children.begin(); it != children.end(); ++it)
	{
		SceneNode* child = (SceneNode*)*it;

		string _name = CP_SYS(mono_string_to_utf8(name));
		if (child->getAlias() == _name)
		{
			ret = child->transformMono;
			break;
		}
	}

	return ret;
}