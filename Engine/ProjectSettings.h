#pragma once

#include "Engine.h"

//Boost headers
#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"
#include "../boost/serialization/vector.hpp"

#include <string>

using namespace std;

class ProjectSettings
{
public:
	ProjectSettings();
	~ProjectSettings();

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & StartupScene;
		if (version > 1)
		{
			ar& directionalShadowResolution;
			ar& spotShadowResolution;
			ar& pointShadowResolution;
		}
		
		if (version > 2)
		{
			ar& shadowCascadesCount;
			ar& shadowSamplesCount;
			ar& shadowCascadesBlending;
		}

		if (version > 3)
		{
			ar& screenWidth;
			ar& screenHeight;
			ar& fullscreen;
		}

		if (version > 4)
			ar& StartupSceneGuid;

		if (version > 5)
			ar& shadowsEnabled;

		if (version > 6)
		{
			ar& BOOST_SERIALIZATION_NVP(tags);
			ar& BOOST_SERIALIZATION_NVP(layers);
		}
	}

	string StartupScene;
	string StartupSceneGuid;

	int directionalShadowResolution = 1024;
	int spotShadowResolution = 1024;
	int pointShadowResolution = 512;
	int shadowCascadesCount = 4;
	int shadowSamplesCount = 4;
	bool shadowCascadesBlending = true;
	bool shadowsEnabled = true;

	int screenWidth = 0;
	int screenHeight = 0;
	bool fullscreen = true;

	std::vector<std::string> tags;
	std::vector<std::string> layers;

	void Load(string path);
	void Save(string path);
};

BOOST_CLASS_VERSION(ProjectSettings, 7)