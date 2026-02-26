#pragma once

#include <string>
#include "../Ogre/source/OgreMain/custom/Component.h"
#include "../Mono/include/mono/metadata/object.h"

class AnimationList : public Component
{
public:
	enum AnimationPlayMode : unsigned int
	{
		APL_Prev,
		APL_Current,
		APL_Both,
		APL_None
	};

	enum AnimationPositionMode : unsigned int
	{
		APS_Start,
		APS_End,
		APS_Current
	};

	struct AnimationData
	{
	public:
		AnimationData() {};
		AnimationData(std::string _name) { name = _name; }

		std::string name = "";
		std::string fileName = "";
		std::string fileGuid = "";
		int startFrame = 0;
		int endFrame = 100;
		float speed = 1.0;
		bool loop = false;
	};

	typedef std::vector<AnimationData> AnimationDataList;

	AnimationList(SceneNode * parent);
	virtual ~AnimationList();

	static std::string COMPONENT_TYPE;
	virtual std::string GetComponentTypeName() { return COMPONENT_TYPE; }

	AnimationDataList & GetAnimationDataList() { return animationDataList; };
	void AddAnimationData(std::string name, std::string fileName, int start, int end, bool loop = false, float speed = 1.0);
	AnimationData & GetAnimationDataAt(int index);
	AnimationData & GetAnimationData(std::string name);
	AnimationData & GetAnimationDataFullName(std::string name);
	bool IsAnimationDataExists(std::string name);
	int GetAnimationDataIndex(std::string name);
	void RemoveAnimationDataAt(int index);
	void RemoveAnimationData(std::string name);
	void SetFBXFileName(std::string name);
	std::string GetFBXFileName() { return fbxFileName; }
	std::string GetFBXFileGuid() { return fbxFileGuid; }
	void SetDefaultAnimation(int def) { defaultAnimation = def; };
	int GetDefaultAnimation() { return defaultAnimation; }
	AnimationData & GetDefaultAnimationData();
	void SetPlayAtStart(bool play) { playAtStart = play; }
	bool GetPlayAtStart() { return playAtStart; }

	//Animation blending
	void SetTransitionTimeLeft(float value) { mTransitionTimeLeft = value; }
	float GetTransitionTimeLeft() { return mTransitionTimeLeft; }

	void SetTransitionDuration(float value) { mTransitionDuration = value; }
	float GetTransitionDuration() { return mTransitionDuration; }

	void SetCurrentAnimationName(std::string name) { currentAnimName = name; };
	std::string GetCurrentAnimationName() { return currentAnimName; };

	void SetPrevAnimationName(std::string name) { prevAnimName = name; };
	std::string GetPrevAnimationName() { return prevAnimName; };

	void SetPlayMode(AnimationPlayMode mode) { playMode = mode; }
	AnimationPlayMode GetPlayMode() { return playMode; }

	void SetPrevStatePositionMode(AnimationPositionMode mode) { prevStatePosition = mode; }
	AnimationPositionMode GetPrevStatePositionMode() { return prevStatePosition; }

	void SetCurrentStatePositionMode(AnimationPositionMode mode) { currentStatePosition = mode; }
	AnimationPositionMode GetCurrentStatePositionMode() { return currentStatePosition; }

	//Internals
	std::map<std::string, SceneNode*> indexedPointers;
	std::map<std::string, bool> eventsCalled;

private:
	AnimationDataList animationDataList;
	std::string fbxFileName = "";
	std::string fbxFileGuid = "";
	int defaultAnimation = 0;
	bool playAtStart = false;

	//Animation blending
	float mTransitionTimeLeft = 0;
	float mTransitionDuration = 0;
	std::string currentAnimName = "";
	std::string prevAnimName = "";
	AnimationPlayMode playMode = AnimationPlayMode::APL_Both;
	AnimationPositionMode prevStatePosition = AnimationPositionMode::APS_Current;
	AnimationPositionMode currentStatePosition = AnimationPositionMode::APS_Current;
};