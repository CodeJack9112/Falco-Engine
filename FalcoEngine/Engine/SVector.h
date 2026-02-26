#pragma once

struct SVector2
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;
		ar & y;
	}

	SVector2() {}
	SVector2(float _x, float _y) {
		x = _x;
		y = _y;
	}
	~SVector2() {}

	float x = 0;
	float y = 0;
};

BOOST_CLASS_VERSION(SVector2, 1)

struct SVector3
{
public:

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & x;
		ar & y;
		ar & z;
	}

	SVector3() {}
	SVector3(float _x, float _y, float _z) {
		x = _x;
		y = _y;
		z = _z;
	}
	~SVector3() {}

	float x = 0;
	float y = 0;
	float z = 0;
};

BOOST_CLASS_VERSION(SVector3, 1)

struct SVector4
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

	SVector4() {}
	SVector4(float _x, float _y, float _z, float _w) {
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}
	~SVector4() {}

	float x = 0;
	float y = 0;
	float z = 0;
	float w = 0;
};

BOOST_CLASS_VERSION(SVector4, 1)