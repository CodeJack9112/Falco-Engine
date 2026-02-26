#pragma once

//Boost headers
#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"
#include "../boost/serialization/vector.hpp"

#include <string>

#include "SVector.h"
#include "SQuaternion.h"

class PropertyState
{
public:
	PropertyState() = default;
	~PropertyState() = default;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& expanded;
		ar& path;
	}

	bool expanded = true;
	std::string path = "";
};

class PropertyEditorData
{
public:
	PropertyEditorData() = default;
	~PropertyEditorData() = default;

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& BOOST_SERIALIZATION_NVP(properties);
		ar& editorName;
	}

	std::string editorName = "PropertyEditor";
	std::vector<PropertyState> properties;
};

class EditorSettings
{
public:
	EditorSettings();
	~EditorSettings();

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & LastOpenedScene;
		ar & BOOST_SERIALIZATION_NVP(propertyEditorsData);

		ar& showGrid;
		ar& showNavMesh;
		ar& snapToGrid;
		ar& snapToGridSize;

		ar& sceneWindowVisible;
		ar& consoleWindowVisible;
		ar& inspectorWindowVisible;
		ar& hierarchyWindowVisible;
		ar& assetsWindowVisible;
		ar& lightingWindowVisible;
		ar& navigationWindowVisible;
		ar& terrainWindowVisible;
		ar& previewWindowVisible;

		ar& cameraPosition;
		ar& cameraRotation;

		if (version > 1)
			ar& cameraSpeed;

		if (version > 2)
		{
			ar& cameraSpeed2;
			ar& cameraSpeed3;
			ar& cameraSpeedPreset;
		}

		if (version > 3)
		{
			ar& shadowsEnabled;
		}

		if (version > 4)
		{
			ar& cameraFarClipPlane;
		}

		if (version > 5)
		{
			ar& gizmoCenterBase;
			ar& runScreenWidth;
			ar& runScreenHeight;
			ar& runFullscreen;
		}
	}

	std::string LastOpenedScene = "";
	std::vector<PropertyEditorData> propertyEditorsData;

	bool showGrid = true;
	bool showNavMesh = true;
	bool snapToGrid = false;
	bool shadowsEnabled = true;
	float snapToGridSize = 1.0f;
	int gizmoCenterBase = 0;

	float cameraSpeed = 1.0f;
	float cameraSpeed2 = 2.0f;
	float cameraSpeed3 = 4.0f;
	float cameraFarClipPlane = 5500.0f;
	int cameraSpeedPreset = 0;

	bool sceneWindowVisible = true;
	bool consoleWindowVisible = true;
	bool inspectorWindowVisible = true;
	bool hierarchyWindowVisible = true;
	bool assetsWindowVisible = true;
	bool lightingWindowVisible = true;
	bool navigationWindowVisible = true;
	bool terrainWindowVisible = false;
	bool previewWindowVisible = true;

	int runScreenWidth = 1280;
	int runScreenHeight = 800;
	bool runFullscreen = false;

	SVector3 cameraPosition = SVector3(0, 10, 20);
	SQuaternion cameraRotation = SQuaternion(0, 0, 0, 1);

	void Load(std::string path);
	void Save(std::string path);
};

BOOST_CLASS_VERSION(PropertyState, 1)
BOOST_CLASS_VERSION(PropertyEditorData, 1)
BOOST_CLASS_VERSION(EditorSettings, 6)