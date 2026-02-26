#pragma once

struct SQuaternion
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;
		ar & y;
		ar & z;
		ar & w;
	}

	SQuaternion() {}
	SQuaternion(float _x, float _y, float _z, float _w)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
};

BOOST_CLASS_VERSION(SQuaternion, 1)