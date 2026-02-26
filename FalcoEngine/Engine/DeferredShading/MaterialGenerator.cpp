/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2014 Torus Knot Software Ltd
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
same license as the rest of the engine.
-----------------------------------------------------------------------------
*/

#include "../stdafx.h"
#include "MaterialGenerator.h"

#include "OgreStringConverter.h"
#include "OgreException.h"

#include "OgrePass.h"
#include "OgreTechnique.h"

#include "OgreHighLevelGpuProgram.h"
#include "OgreHighLevelGpuProgramManager.h"
#include "OgreMaterialManager.h"

#include "OgreMaterialManager.h"
#include <OgreGpuProgramManager.h>

#include <iostream>

using namespace Ogre;

MaterialGenerator::MaterialGenerator():
    vsMask(0), fsMask(0), matMask(0), mImpl(0)
{
}
MaterialGenerator::~MaterialGenerator()
{
    // we have generated fragment shaders and materials
    // so delete them
    for(ProgramMap::iterator it = mFs.begin(); it != mFs.end(); ++it) {
        HighLevelGpuProgramManager::getSingleton().remove(it->second);
    }
    for(MaterialMap::iterator it = mMaterials.begin(); it != mMaterials.end(); ++it) {
        MaterialManager::getSingleton().remove(it->second);
    }

    delete mImpl;
}

const MaterialPtr &MaterialGenerator::getMaterial(Perm permutation, std::string name)
{
    /// Create it
    MaterialPtr templ = getTemplateMaterial(permutation & matMask, name);

	if (templ != nullptr)
	{
		GpuProgramPtr vs = getVertexShader(permutation & vsMask);
		GpuProgramPtr fs = getFragmentShader(permutation & fsMask);

		/// Create material name
		String name = materialBaseName + StringConverter::toString(permutation);

		Technique* tech = templ->getTechnique(0);
		Pass* pass = tech->getPass(0);

		if (!pass->hasVertexProgram())
		{
			if (vs != nullptr && GpuProgramManager::getSingleton().getByName(vs->getName(), ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME) != nullptr)
				pass->setVertexProgram(vs->getName());
		}

		if (!pass->hasFragmentProgram())
		{
			if (fs != nullptr && GpuProgramManager::getSingleton().getByName(fs->getName(), ResourceGroupManager::AUTODETECT_RESOURCE_GROUP_NAME) != nullptr)
				pass->setFragmentProgram(fs->getName());
		}
	}

	return templ;
}

const GpuProgramPtr &MaterialGenerator::getVertexShader(Perm permutation)
{
    return mImpl->generateVertexShader(permutation);
}

const GpuProgramPtr &MaterialGenerator::getFragmentShader(Perm permutation)
{
	return mImpl->generateFragmentShader(permutation);
}

const MaterialPtr &MaterialGenerator::getTemplateMaterial(Perm permutation, std::string name)
{
    return mImpl->generateTemplateMaterial(permutation, name);
}

MaterialGenerator::Impl::~Impl()
{
}