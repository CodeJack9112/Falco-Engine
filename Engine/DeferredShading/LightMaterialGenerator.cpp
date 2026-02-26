#include "../stdafx.h"
#include "LightMaterialGenerator.h"

#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreMaterialManager.h"

#include "OgrePass.h"
#include "OgreTechnique.h"

#include "OgreGpuProgramManager.h"
#include "OgreHighLevelGpuProgram.h"
#include "OgreHighLevelGpuProgramManager.h"

#include "DLight.h"
#include "DeferredShading.h"
#include "GpuProgramSources.h"

using namespace Ogre;

LightMaterialGenerator::ProgramMap LightMaterialGenerator::programsV;
LightMaterialGenerator::ProgramMap LightMaterialGenerator::programsF;
LightMaterialGenerator::MaterialMap LightMaterialGenerator::materials;

//GLSL
class LightMaterialGeneratorGLSL : public MaterialGenerator::Impl
{
public:
    typedef MaterialGenerator::Perm Perm;
    LightMaterialGeneratorGLSL(const String &baseName):
    mBaseName(baseName)
    {

    }

    virtual ~LightMaterialGeneratorGLSL()
    {

    }

    virtual GpuProgramPtr generateVertexShader(Perm permutation)
    {
        LightMaterialGenerator::ProgramMap::iterator i = LightMaterialGenerator::programsV.find(permutation);
		if (i != LightMaterialGenerator::programsV.end())
		{
			GpuProgramPtr ptr = i->second;
			return ptr;
		}

        String programName = "DeferredShading/post/";

        if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        {
            programName += "Ambient_vs";
        }
        else
        {
            programName += "Standard_vs";
        }

        GpuProgramPtr ptr = HighLevelGpuProgramManager::getSingleton().getByName(programName, "Assets");
        OgreAssert(ptr, "vertex shader is NULL");

        LightMaterialGenerator::programsV[permutation] = ptr;

        return ptr;
    }

    virtual GpuProgramPtr generateFragmentShader(Perm permutation)
    {
        LightMaterialGenerator::ProgramMap::iterator i = LightMaterialGenerator::programsF.find(permutation);
		if (i != LightMaterialGenerator::programsF.end())
		{
			GpuProgramPtr ptr = i->second;
			return ptr;
		}

		// Create name
		String name = mBaseName + StringConverter::toString(permutation) + "_ps";

        /// Create shader
        if (mMasterSource.empty())
        {
            //DataStreamPtr ptrMasterSource = ResourceGroupManager::getSingleton().openResource("DeferredShading/Shaders/Standard_ps.glsl", "Assets");

            //OgreAssert(ptrMasterSource, "could not find 'LightMaterial_ps'");
            mMasterSource = GpuProgramSources::lightFs;//ptrMasterSource->getAsString();
        }

        OgreAssert(!mMasterSource.empty(), "no source code");

        // Create shader object
        HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(name, "Assets", "glsl", GPT_FRAGMENT_PROGRAM);
        ptrProgram->setParameter("profiles", "glsl120");

        ptrProgram->setSource(mMasterSource);
        // set up the preprocessor defines
        // Important to do this before any call to get parameters, i.e. before the program gets loaded
        ptrProgram->setParameter("preprocessor_defines", getPPDefines(permutation));

        setUpBaseParameters(ptrProgram->getDefaultParameters());

        // Bind samplers
        GpuProgramParametersSharedPtr params = ptrProgram->getDefaultParameters();
        int numSamplers = 0;
        params->setNamedConstant("Tex0", (int)numSamplers++);
        params->setNamedConstant("Tex1", (int)numSamplers++);
        params->setNamedConstant("u_MetallicRoughnessSampler", (int)numSamplers++);
        params->setNamedConstant("u_OcclusionSampler", (int)numSamplers++);
        params->setNamedConstant("u_DiffuseEnvSampler", (int)numSamplers++);
        params->setNamedConstant("u_EmissiveSampler", (int)numSamplers++);
        params->setNamedConstant("u_ViewPos", (int)numSamplers++);
        params->setNamedConstant("u_LightMap", (int)numSamplers++);

		if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
		{
			if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
			{
				params->setNamedConstant("ShadowTex0", (int)numSamplers++);
				params->setNamedConstant("ShadowTex1", (int)numSamplers++);
				params->setNamedConstant("ShadowTex2", (int)numSamplers++);
				params->setNamedConstant("ShadowTex3", (int)numSamplers++);
			}
			else
			{
				params->setNamedConstant("ShadowTex", (int)numSamplers++);
			}
		}

        LightMaterialGenerator::programsF[permutation] = ptrProgram;

		return ptrProgram;
    }

    virtual MaterialPtr generateTemplateMaterial(Perm permutation, std::string name)
    {
        String materialName = mBaseName;

        if(permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        {
            materialName += "Quad";
        }
        else
        {
            materialName += "Geometry";
        }

        if(permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
        {
            materialName += "Shadow";
        }

        auto i = LightMaterialGenerator::materials.find(materialName + "_" + name);
        if (i != LightMaterialGenerator::materials.end())
        {
            MaterialPtr ptr = i->second;
            return ptr;
        }

        MaterialPtr ptr = MaterialManager::getSingleton().getByName(materialName, "Assets")->clone(materialName + "_" + name);

        LightMaterialGenerator::materials[materialName + "_" + name] = ptr;

		return ptr;
    }

protected:
    String mBaseName = "";
    std::string mMasterSource = "";
    
    // Utility method
    String getPPDefines(Perm permutation)
    {
        String strPPD;

        //Get the type of light
        Ogre::uint lightType = 0;
        if (permutation & LightMaterialGenerator::MI_POINT)
        {
            lightType = 1;
        }
        else if (permutation & LightMaterialGenerator::MI_SPOTLIGHT)
        {
            lightType = 2;
        }
        else if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        {
            lightType = 3;
        }
        else
        {
            assert(false && "Permutation must have a light type");
        }
        strPPD += "LIGHT_TYPE=" + StringConverter::toString(lightType);

        //Optional parameters
        if (permutation & LightMaterialGenerator::MI_SPECULAR)
        {
            strPPD += ",IS_SPECULAR=1";
        }
        if (permutation & LightMaterialGenerator::MI_ATTENUATED)
        {
            strPPD += ",IS_ATTENUATED=1";
        }
        if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
        {
            strPPD += ",IS_SHADOW_CASTER=1";
        }
		if (SceneManager::shadowCascadesBlending)
		{
			strPPD += ",CASCADE_BLENDING=1";
		}

		strPPD += ",MANUAL_SRGB,SRGB_FAST_APPROXIMATION,HAS_METALROUGHNESSMAP,HAS_OCCLUSIONMAP,HAS_EMISSIVEMAP,USE_IBL";

        return strPPD;
    }

    void setUpBaseParameters(const GpuProgramParametersSharedPtr& params)
    {
        assert(params);

        struct AutoParamPair { String name; GpuProgramParameters::AutoConstantType type; };

        //A list of auto params that might be present in the shaders generated
		static const AutoParamPair AUTO_PARAMS[] = {
			{ "vpWidth",            GpuProgramParameters::ACT_VIEWPORT_WIDTH },
			{ "vpHeight",           GpuProgramParameters::ACT_VIEWPORT_HEIGHT },
			{ "worldView",          GpuProgramParameters::ACT_WORLDVIEW_MATRIX },
			{ "world",				GpuProgramParameters::ACT_WORLD_MATRIX },
			{ "invProj",            GpuProgramParameters::ACT_INVERSE_PROJECTION_MATRIX },
			{ "invView",            GpuProgramParameters::ACT_INVERSE_VIEW_MATRIX },
			{ "flip",               GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING },
			{ "lightDiffuseColor",  GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR },
			{ "lightSpecularColor", GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR },
			{ "lightFalloff",       GpuProgramParameters::ACT_LIGHT_ATTENUATION },
			{ "lightPos",           GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE },
			{ "lightPosWorld",      GpuProgramParameters::ACT_LIGHT_POSITION },
			{ "lightDir",           GpuProgramParameters::ACT_LIGHT_DIRECTION_VIEW_SPACE },
			{ "spotParams",         GpuProgramParameters::ACT_SPOTLIGHT_PARAMS },
			{ "farClipDistance",    GpuProgramParameters::ACT_FAR_CLIP_DISTANCE },
			{ "nearClipDistance",   GpuProgramParameters::ACT_NEAR_CLIP_DISTANCE },
			{ "lightPower",			GpuProgramParameters::ACT_LIGHT_POWER_SCALE },
        };
        int numParams = sizeof(AUTO_PARAMS) / sizeof(AutoParamPair);
        
        for (int i=0; i<numParams; i++)
        {
            if (params->_findNamedConstantDefinition(AUTO_PARAMS[i].name))
            {
                params->setNamedAutoConstant(AUTO_PARAMS[i].name, AUTO_PARAMS[i].type);
            }
        }

		params->setNamedConstant("u_BaseColorFactor", Vector4(1, 1, 1, 1));
		params->setNamedConstant("u_MetallicRoughnessValues", Vector2(1, 1));
		//params->setNamedConstant("u_ScaleFGDSpec", Vector4(0, 0, 0, 0));
		//params->setNamedConstant("u_ScaleDiffBaseMR", Vector4(0, 0, 0, 0));
    }
};

LightMaterialGenerator::LightMaterialGenerator()
{
    vsMask = 0x00000004;
    fsMask = 0x0000003F;
	matMask = LightMaterialGenerator::MI_DIRECTIONAL | LightMaterialGenerator::MI_SHADOW_CASTER;
    
    materialBaseName = "DeferredShading/LightMaterial/";
    mImpl = new LightMaterialGeneratorGLSL("DeferredShading/LightMaterial/");
}

LightMaterialGenerator::~LightMaterialGenerator()
{

}
