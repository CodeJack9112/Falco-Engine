#pragma once

#include "Engine.h"

class UIManager : public RenderQueueListener
{
public:
	//enum RenderType { COLOR, TEXTURE };

	class UIRenderQueue
	{
	public:
		RenderOperation renderOp;
		TexturePtr texture;
		RealRect rect;
		//RenderType renderType;
		int index = 0;
	};

private:
	std::vector<UIRenderQueue*> renderQueues;
	void prepareForRender();

public:
	UIManager();
	~UIManager();

	virtual void renderQueueStarted(uint8 queueGroupId, const String& invocation, bool& skipThisInvocation);
	virtual void renderQueueEnded(uint8 queueGroupId, const String &invocation, bool &repeatThisInvocation);

	void AddRenderQueue(UIRenderQueue * queue) { renderQueues.push_back(queue); }
	void RemoveRenderQueue(UIRenderQueue * queue) 
	{ 
		//renderQueues.erase(std::remove(renderQueues.begin(), renderQueues.end(), queue), renderQueues.end()); 
		auto it = std::find(renderQueues.begin(), renderQueues.end(), queue);

		if (it != renderQueues.end())
			renderQueues.erase(it);
	}
	void Cleanup();
};

