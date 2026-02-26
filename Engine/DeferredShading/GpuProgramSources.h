#pragma once

#include <string>

class GpuProgramSources
{
public:
	static std::string ambientVs;
	static std::string ambientPs;
	static std::string shadowCasterVs;
	static std::string shadowCasterFs;
	static std::string lightVs;
	static std::string lightFs;
};