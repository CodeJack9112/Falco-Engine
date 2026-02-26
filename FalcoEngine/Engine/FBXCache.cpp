#include "stdafx.h"
#include "FBXCache.h"

//#include "Engine.h"
#include "../boost/serialization/export.hpp"
#include "IO.h"

void FBXCache::Save(string path)
{
	//Prepare serialization
	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	//Write file
	stream << *this;
	ofs.close();
}

void FBXCache::Load(string path)
{
	if (IO::FileExists(path))
	{
		//Deserialize
		std::ifstream ofs(path, std::ios::binary);
		boost::archive::binary_iarchive stream(ofs);

		//Read file
		stream >> *this;
		ofs.close();
	}
}