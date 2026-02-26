#pragma once

#include "OgreCompositorInstance.h"

#include "../Mono/include/mono/metadata/object.h"

using namespace Ogre;

class PostEffectListener : public CompositorInstance::Listener
{
private:
	MonoObject* postEffectMono = nullptr;

public:
	void setPostEffectMono(MonoObject* obj) { postEffectMono = obj; }
	MonoObject* getPostEffectMono() { return postEffectMono; }

	virtual void notifyMaterialRender(uint32 pass_id, MaterialPtr& mat);
};

