#include "ResourceMap.h"
#include <OgreResourceGroupManager.h>
#include <OgreResourceManager.h>
#include "IO.h"

#include "../boost/serialization/serialization.hpp"
#include "../boost/serialization/binary_object.hpp"
#include "../boost/archive/binary_oarchive.hpp"
#include "../boost/archive/binary_iarchive.hpp"
#include "../boost/serialization/export.hpp"

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include "../boost/algorithm/string.hpp"

#include "FBXScene.h"
#include "Engine.h"

using namespace Ogre;

std::vector<ResourceID> ResourceMap::resourceMap;
std::map<Resource*, string> ResourceMap::guidMap;
std::vector<string> ResourceMap::imgs = { "jpg", "png", "jpeg", "bmp", "psd", "tga", "gif", "pic", "ppm", "pgm", "hdr" };

void ResourceMap::load()
{
	std::string path = GetEngine->GetLibraryPath() + "ResourceMap.bin";

	if (IO::FileExists(path))
	{
		std::ifstream ofs(path, std::ios::binary);
		boost::archive::binary_iarchive stream(ofs);

		ResourceMap map;

		//Read file
		stream >> map;
		ofs.close();

		auto it = std::find_if(map.resourceMap.begin(), map.resourceMap.end(), [](ResourceID& res) -> bool { return res.resourceGuid.empty(); });
		while (it != map.resourceMap.end())
		{
			map.resourceMap.erase(it);
			it = std::find_if(map.resourceMap.begin(), map.resourceMap.end(), [](ResourceID& res) -> bool { return res.resourceGuid.empty(); });
		}
	}
}

void ResourceMap::save()
{
	std::string path = GetEngine->GetLibraryPath() + "ResourceMap.bin";

	std::ofstream ofs(path, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	ResourceMap map;

	//Write file
	stream << map;

	ofs.close();
}

void ResourceMapTemp::load(std::string fileName)
{
	if (IO::FileExists(fileName))
	{
		std::ifstream ofs(fileName, std::ios::binary);
		boost::archive::binary_iarchive stream(ofs);

		//Read file
		stream >> *this;
		ofs.close();

		auto it = std::find_if(resourceMap.begin(), resourceMap.end(), [](ResourceID& res) -> bool { return res.resourceGuid.empty(); });
		while (it != resourceMap.end())
		{
			(*this).resourceMap.erase(it);
			it = std::find_if(resourceMap.begin(), resourceMap.end(), [](ResourceID& res) -> bool { return res.resourceGuid.empty(); });
		}
	}
}

void ResourceMapTemp::save(std::string fileName)
{
	std::ofstream ofs(fileName, std::ios::binary);
	boost::archive::binary_oarchive stream(ofs);

	//Write file
	stream << *this;

	ofs.close();
}

std::string ResourceMap::genGuid()
{
	boost::uuids::random_generator uuid_gen;
	boost::uuids::uuid u = uuid_gen();

	return boost::uuids::to_string(u);
}

void ResourceMap::identifyResources()
{
	load();

	IO::listFiles(GetEngine->GetAssetsPath(), true, nullptr, [=](string d, string f) {
		
		string ext = IO::GetFileExtension(f);
		string path = d + f;
		path = boost::replace_all_copy(path, GetEngine->GetAssetsPath(), "");

		if (ext == "ogg" || ext == "terrain" || ext == "prefab" || ext == "scene" || ext == "cs" || find(imgs.begin(), imgs.end(), ext) != imgs.end())
		{
			auto it = find_if(resourceMap.begin(), resourceMap.end(), [path](ResourceID& res) -> bool { return res.resourceName == path; });
			if (it == resourceMap.end())
			{
				ResourceID resId = ResourceID();
				resId.resourceName = path;
				resId.resourceGuid = genGuid();

				resourceMap.push_back(resId);
			}
		}
		else
		{
			//if (ResourceGroupManager::getSingleton().resourceExistsInAnyGroup(path))
			{
				MapIterator managers = ResourceGroupManager::getSingleton().getResourceManagerIterator();
				ResourcePtr resource;

				while (managers.hasMoreElements())
				{
					ResourceManager* manager = managers.getNext();

					if (manager->resourceExists(path, "Assets"))
					{
						resource = manager->getResourceByName(path, "Assets");
						break;
					}

					if (manager->resourceExists(IO::GetFileName(path), "Assets"))
					{
						path = IO::GetFileName(path);
						resource = manager->getResourceByName(path, "Assets");
						break;
					}
				}

				if (resource != nullptr)
				{
					auto it = find_if(resourceMap.begin(), resourceMap.end(), [path](ResourceID& res) -> bool { return res.resourceName == path; });
					if (it == resourceMap.end())
					{
						ResourceID resId = ResourceID();
						resId.resourceName = path;
						resId.resourceGuid = genGuid();

						guidMap[resource.getPointer()] = resId.resourceGuid;

						resourceMap.push_back(resId);
					}
					else
					{
						guidMap[resource.getPointer()] = it->resourceGuid;
					}
				}
			}
		}
	});

	save();
}

//TODO: call this function on project build/reopen
void ResourceMap::removeLostResources()
{
	load();

	std::vector<ResourceID> del;
	for (auto it = resourceMap.begin(); it != resourceMap.end(); ++it)
	{
		string ext = IO::GetFileExtension(it->resourceName);

		if (ext == "ogg" || ext == "terrain" || ext == "prefab" || ext == "scene" || ext == "cs" || find(imgs.begin(), imgs.end(), ext) != imgs.end())
		{
			if (!IO::FileExists(GetEngine->GetAssetsPath() + it->resourceName))
			{
				del.push_back(*it);
			}
		}
		else
		{
			bool exists = false;

			MapIterator managers = ResourceGroupManager::getSingleton().getResourceManagerIterator();

			while (managers.hasMoreElements())
			{
				ResourceManager* manager = managers.getNext();

				if (manager->resourceExists(it->resourceName, "Assets"))
				{
					exists = true;
					break;
				}
			}

			if (!exists)
			{
				del.push_back(*it);
			}
		}
	}

	for (auto it = del.begin(); it != del.end(); ++it)
	{
		auto _it = find_if(resourceMap.begin(), resourceMap.end(), [it](ResourceID& res) -> bool { return res.resourceGuid == it->resourceGuid; });
		resourceMap.erase(_it);
	}

	del.clear();

	save();
}

void ResourceMap::addResource(string name, string guid)
{
	if (getResourceNameFromGuid(guid) == "" && getResourceGuidFromName(name) == "")
	{
		ResourceID resId = ResourceID();
		resId.resourceName = name;
		resId.resourceGuid = guid;

		resourceMap.push_back(resId);
	}
}

string ResourceMap::getResourceNameFromGuid(string guid)
{
	auto it = find_if(resourceMap.begin(), resourceMap.end(), [guid](ResourceID& res) -> bool { return res.resourceGuid == guid; });

	if (it != resourceMap.end())
		return it->resourceName;

	return "";
}

string ResourceMap::getResourceGuidFromName(string name)
{
	auto it = find_if(resourceMap.begin(), resourceMap.end(), [name](ResourceID& res) -> bool { return res.resourceName == name; });

	if (it != resourceMap.end())
		return it->resourceGuid;

	return "";
}

void ResourceMap::setResourceGuid(string name, string guid)
{
	auto it = find_if(resourceMap.begin(), resourceMap.end(), [name](ResourceID& res) -> bool { return res.resourceName == name; });

	if (it != resourceMap.end())
		it->resourceGuid = guid;

	save();
}

void ResourceMap::setResourceNameByGuid(string guid, string name)
{
	auto it = find_if(resourceMap.begin(), resourceMap.end(), [guid](ResourceID& res) -> bool { return res.resourceGuid == guid; });

	if (it != resourceMap.end())
		it->resourceName = name;

	save();
}

void ResourceMap::setResourceName(string oldName, string newName)
{
	auto it = find_if(resourceMap.begin(), resourceMap.end(), [oldName](ResourceID& res) -> bool { return res.resourceName == oldName; });

	if (it != resourceMap.end())
		it->resourceName = newName;

	save();
}