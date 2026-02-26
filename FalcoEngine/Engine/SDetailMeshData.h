#pragma once

struct SDetailMeshData
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& meshPath;
		ar& position;
		ar& yaw;
		ar& scale;
		if (version > 1) ar& meshGuid;
	}

	SDetailMeshData() {}
	~SDetailMeshData() {}

	std::string meshPath;
	std::string meshGuid;
	SVector3 position;
	float yaw = 0;
	float scale = 1.0f;
};

BOOST_CLASS_VERSION(SDetailMeshData, 2)

struct SDetailMeshDataBrush
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive& ar, const unsigned int version)
	{
		ar& meshPath;
		ar& scaleMin;
		ar& scaleMax;
		if (version > 1) ar& meshGuid;
	}

	SDetailMeshDataBrush() {}
	~SDetailMeshDataBrush() {}

	std::string meshPath;
	std::string meshGuid;
	float scaleMin = 1.0f;
	float scaleMax = 1.0f;
};

BOOST_CLASS_VERSION(SDetailMeshDataBrush, 2)