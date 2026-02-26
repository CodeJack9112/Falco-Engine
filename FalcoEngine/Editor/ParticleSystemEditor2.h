#pragma once

#include "ObjectEditor2.h"

class ParticleSystemEditor2 : public ObjectEditor2
{
public:
	ParticleSystemEditor2();
	~ParticleSystemEditor2();

	virtual void init() {}
	virtual void init(std::vector<Ogre::SceneNode*> nodes);
	virtual void update();

private:
	bool isEmittersIdentical();
	bool isAffectorsIdentical();

	void onAddEmitterButtonClick(TreeNode* node, int val);
	void onAddAffectorButtonClick(TreeNode* node, int val);

	void onRemoveEmitter(TreeNode* node, int val);
	void onRemoveAffector(TreeNode* node, int val);

	void onReplay(TreeNode* node);

	//Particle system props
	void onDropMaterial(TreeNode* prop, TreeNode* from);
	void onChangeEnabled(Property* prop, bool val);
	void onChangeTransformSpace(Property* prop, bool val);
	void onChangeCastShadows(Property* prop, bool val);
	void onChangeMaxParticles(Property* prop, int val);
	void onChangeSpeed(Property* prop, float val);
	void onChangeSize(Property* prop, Ogre::Vector2 val);
	void onChangeBillboardType(Property* prop, std::string val);

	//Emitters props
	void onChangeEmitterEnabled(Property* prop, bool val);
	void onChangeEmitterAngle(Property* prop, float val);
	void onChangeEmitterStartColor(Property* prop, Ogre::ColourValue val);
	void onChangeEmitterSize(Property* prop, Ogre::Vector2 val);
	void onChangeEmitterEmissionRate(Property* prop, float val);
	void onChangeEmitterDirection(Property* prop, Ogre::Vector3 val);
	void onChangeEmitterDuration(Property* prop, Ogre::Vector2 val);
	void onChangeEmitterVelocity(Property* prop, Ogre::Vector2 val);
	void onChangeEmitterRepeatDelay(Property* prop, Ogre::Vector2 val);
	void onChangeEmitterLifeTime(Property* prop, Ogre::Vector2 val);
	void onChangeEmitterBoxWidth(Property* prop, float val);
	void onChangeEmitterBoxHeight(Property* prop, float val);
	void onChangeEmitterBoxDepth(Property* prop, float val);
	void onChangeEmitterInnerWidth(Property* prop, float val);
	void onChangeEmitterInnerHeight(Property* prop, float val);
	void onChangeEmitterInnerDepth(Property* prop, float val);
	void onChangeEmitterInnerWidthRing(Property* prop, float val);
	void onChangeEmitterInnerHeightRing(Property* prop, float val);

	//Affectors props
	void onChangeAffectorForce(Property* prop, Ogre::Vector3 val);
	void onChangeAffectorApplication(Property* prop, std::string val);
	void onChangeAffectorColorFaderRed(Property* prop, float val);
	void onChangeAffectorColorFaderGreen(Property* prop, float val);
	void onChangeAffectorColorFaderBlue(Property* prop, float val);
	void onChangeAffectorColorFaderAlpha(Property* prop, float val);
	void onChangeAffectorColorFader2Red1(Property* prop, float val);
	void onChangeAffectorColorFader2Green1(Property* prop, float val);
	void onChangeAffectorColorFader2Blue1(Property* prop, float val);
	void onChangeAffectorColorFader2Alpha1(Property* prop, float val);
	void onChangeAffectorColorFader2Red2(Property* prop, float val);
	void onChangeAffectorColorFader2Green2(Property* prop, float val);
	void onChangeAffectorColorFader2Blue2(Property* prop, float val);
	void onChangeAffectorColorFader2Alpha2(Property* prop, float val);
	void onChangeAffectorColorFader2ChangeTime(Property* prop, float val);
	void onChangeAffectorScalerRate(Property* prop, float val);
	void onChangeAffectorRotatorSpeedStart(Property* prop, float val);
	void onChangeAffectorRotatorSpeedEnd(Property* prop, float val);
	void onChangeAffectorRotatorRotStart(Property* prop, float val);
	void onChangeAffectorRotatorRotEnd(Property* prop, float val);
	void onChangeAffectorColorInterpolateTime1(Property* prop, float val);
	void onChangeAffectorColorInterpolateColor1(Property* prop, Ogre::ColourValue val);
	void onChangeAffectorColorInterpolateTime2(Property* prop, float val);
	void onChangeAffectorColorInterpolateColor2(Property* prop, Ogre::ColourValue val);
	void onChangeAffectorColorInterpolateTime3(Property* prop, float val);
	void onChangeAffectorColorInterpolateColor3(Property* prop, Ogre::ColourValue val);
	void onChangeAffectorColorInterpolateTime4(Property* prop, float val);
	void onChangeAffectorColorInterpolateColor4(Property* prop, Ogre::ColourValue val);
	void onChangeAffectorColorInterpolateTime5(Property* prop, float val);
	void onChangeAffectorColorInterpolateColor5(Property* prop, Ogre::ColourValue val);
	void onChangeAffectorColorInterpolateTime6(Property* prop, float val);
	void onChangeAffectorColorInterpolateColor6(Property* prop, Ogre::ColourValue val);
	void onChangeAffectorDirectionRandomness(Property* prop, float val);
	void onChangeAffectorDirectionScope(Property* prop, float val);
	void onChangeAffectorDirectionKeepVelocity(Property* prop, bool val);
};