#pragma once

#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/vector.hpp"
#include <string>

using namespace std;

struct SProject
{
public:
	SProject() {}
	~SProject() {}

	friend bool operator==(const SProject& l, const SProject& r)
	{
		return l.projectPath == r.projectPath;
	}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & projectName;
		ar & projectPath;
		ar & engineVersion;
	}

public:
	string projectName;
	string projectPath;
	string engineVersion;
};

struct SProjectInfo
{
public:
	SProjectInfo() {}
	~SProjectInfo() {}

	friend class boost::serialization::access;
	template<class Archive>
	void serialize(Archive & ar, const unsigned int version)
	{
		ar & BOOST_SERIALIZATION_NVP(projects);
	}

public:
	std::vector<SProject> projects;
};

BOOST_CLASS_VERSION(SProject, 1)
BOOST_CLASS_VERSION(SProjectInfo, 1)