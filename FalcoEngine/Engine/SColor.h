#pragma once

struct SColor
{
public:
	SColor() {}
	SColor(float _r, float _g, float _b, float _a)
	{
		r = _r;
		g = _g;
		b = _b;
		a = _a;
	}
	~SColor() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & r;
		ar & g;
		ar & b;
		ar & a;
	}

public:
	float r = 0;
	float g = 0;
	float b = 0;
	float a = 0;
};

BOOST_CLASS_VERSION(SColor, 1)