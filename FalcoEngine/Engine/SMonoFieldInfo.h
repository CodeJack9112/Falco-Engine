#pragma once

class SMonoFieldInfo
{
public:
	SMonoFieldInfo() = default;
	~SMonoFieldInfo() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & fieldName;
		ar & fieldType;
		ar & stringVal;
		ar & boolVal;
		ar & intVal;
		ar & floatVal;
		ar & objectVal;

		if (version > 1)
		{
			ar& vec2Val;
			ar& vec3Val;
			ar& vec4Val;
		}
	}

public:
	std::string fieldName = "";
	std::string fieldType = "";

	std::string stringVal = "";
	bool boolVal = false;
	int intVal = 0;
	float floatVal = 0;
	std::string objectVal = "Game Object [None]";
	SVector2 vec2Val = SVector2(0, 0);
	SVector3 vec3Val = SVector3(0, 0, 0);
	SVector4 vec4Val = SVector4(0, 0, 0, 0);
};

BOOST_CLASS_VERSION(SMonoFieldInfo, 2)