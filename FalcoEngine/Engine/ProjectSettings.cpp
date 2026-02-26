#include "stdafx.h"
#include "ProjectSettings.h"

#include "../boost/serialization/export.hpp"
#include "Engine/IO.h"
#include "ResourceMap.h"

ProjectSettings::ProjectSettings()
{
}

ProjectSettings::~ProjectSettings()
{
}

void ProjectSettings::Save(string path)
{
	//Prepare serialization
	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	//Write file
	stream << *this;
	ofs.close();
}


void ProjectSettings::Load(string path)
{
	if (IO::FileExists(path))
	{
		//Deserialize
		std::ifstream ofs(path, std::ios::binary);
		boost::archive::binary_iarchive stream(ofs);

		//Read file
		stream >> *this;
		ofs.close();
		stream.delete_created_pointers();

		StartupScene = ResourceMap::getResourceNameFromGuid(StartupSceneGuid);
	}
}