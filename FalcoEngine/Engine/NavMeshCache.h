#pragma once

//Boost headers
#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"
#include "../boost/serialization/vector.hpp"

#include <string>
#include "SVector.h"

using namespace std;

class NavMeshCache
{
public:
	NavMeshCache() {}
	~NavMeshCache() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & cacheSize;
		ar & BOOST_SERIALIZATION_NVP(dataCache);
		ar & BOOST_SERIALIZATION_NVP(vertsCache);
	}

	int cacheSize = 0;
	std::vector<unsigned char> dataCache;
	std::vector<SVector3> vertsCache;

	void Load(string path);
	void LoadFromBuffer(char* buffer, int bufSize);
	void Save(string path);
};

BOOST_CLASS_VERSION(NavMeshCache, 1)