#include "stdafx.h"
#include "NavMeshCache.h"

#include "../boost/serialization/export.hpp"
#include "../boost/iostreams/stream.hpp"
#include "../Engine/IO.h"

void NavMeshCache::Save(string path)
{
	//Prepare serialization
	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	//Write file
	stream << *this;
	ofs.close();
}

void NavMeshCache::Load(string path)
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
	}
}

void NavMeshCache::LoadFromBuffer(char* buffer, int bufSize)
{
	boost::iostreams::stream<boost::iostreams::array_source> is(buffer, bufSize);
	boost::archive::binary_iarchive stream(is);

	stream >> *this;

	is.close();
	delete[] buffer;
}
