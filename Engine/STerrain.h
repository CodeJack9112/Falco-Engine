#pragma once

struct STerrain
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & fileName;
		ar & x;
		ar & y;
		if (version > 1) ar& fileGuid;
		//if (version > 2)
		//{
			//ar& position;
			//ar& size;
			//ar& worldSize;
		//}
	}

	STerrain() {}
	~STerrain() {}

	std::string fileName = "";
	std::string fileGuid = "";
	int x = 0;
	int y = 0;
	//SVector3 position = SVector3(0, 0, 0);
	//int size = 129;
	//float worldSize = 3000;
};

BOOST_CLASS_VERSION(STerrain, 2)