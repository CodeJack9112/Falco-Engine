#include "stdafx.h"
#include "ParticleSystemEditor2.h"

#include <OgreSceneNode.h>
#include <OgreParticleSystem.h>
#include <OgreParticleSystemManager.h>
#include <OgreParticleEmitter.h>
#include <OgreParticleAffector.h>
#include <OgreParticleSystemRenderer.h>
#include <OgreParticleEmitterFactory.h>

#include "../imgui/imgui.h"
#include "../imgui/imgui_internal.h"

#include "../Engine/IO.h"

#include "MainWindow.h"

#include "PropVector3.h"
#include "PropVector2.h"
#include "PropComboBox.h"
#include "PropFloat.h"
#include "PropInt.h"
#include "PropBool.h"
#include "PropString.h"
#include "PropColorPicker.h"

ParticleSystemEditor2::ParticleSystemEditor2()
{
	setEditorName("ParticleSystemEditor");
}

ParticleSystemEditor2::~ParticleSystemEditor2()
{
}

void ParticleSystemEditor2::init(std::vector<SceneNode*> nodes)
{
	__super::init(nodes);

	auto sceneNodes = getSceneNodes();

	SceneNode* sceneNode = sceneNodes[0];

	ParticleSystem* particleSystem = (ParticleSystem*)sceneNode->getAttachedObject(0);

	Property* mainGroup = new Property(this, "Particle system");
	mainGroup->icon = TextureManager::getSingleton().load("Icons/Hierarchy/particles.png", "Editor");
	mainGroup->setHasButton(true);
	mainGroup->setButtonText("Play");
	mainGroup->setOnButtonClickCallback([=](TreeNode* prop) { onReplay(prop); });

	PropBool* emit = new PropBool(this, "Emit", particleSystem->getEmitting());
	emit->setOnChangeCallback([=](Property* prop, bool val) { onChangeEnabled(prop, val); });

	PropString* material = new PropString(this, "Material", particleSystem->getMaterialName());
	material->setOnDropCallback([=](TreeNode * prop, TreeNode * from) { onDropMaterial(prop, from); });
	material->setSupportedFormats({ "material" });
	material->setReadonly(true);

	PropBool* transformSpace = new PropBool(this, "Local", particleSystem->getKeepParticlesInLocalSpace());
	transformSpace->setOnChangeCallback([=](Property* prop, bool val) { onChangeTransformSpace(prop, val); });

	PropBool* castShadows = new PropBool(this, "Cast shadows", particleSystem->getCastShadows());
	castShadows->setOnChangeCallback([=](Property* prop, bool val) { onChangeCastShadows(prop, val); });

	PropComboBox* billboardType = new PropComboBox(this, "Billboard type", { "point", "oriented_common", "oriented_self", "perpendicular_common", "perpendicular_self"});
	billboardType->setCurrentItem(particleSystem->getRenderer()->getParameter("billboard_type"));
	billboardType->setOnChangeCallback([=](Property* prop, string val) { onChangeBillboardType(prop, val); });

	PropInt* maxParticles = new PropInt(this, "Max particles", (int)particleSystem->getParticleQuota());
	maxParticles->setOnChangeCallback([=](Property* prop, float val) { onChangeMaxParticles(prop, val); });

	PropFloat* speedFactor = new PropFloat(this, "Speed", particleSystem->getSpeedFactor());
	speedFactor->setOnChangeCallback([=](Property* prop, float val) { onChangeSpeed(prop, val); });

	PropVector2* size = new PropVector2(this, "Size", Vector2(particleSystem->getDefaultWidth(), particleSystem->getDefaultHeight()));
	size->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeSize(prop, val); });

	mainGroup->addChild(material);
	mainGroup->addChild(emit);
	mainGroup->addChild(transformSpace);
	mainGroup->addChild(billboardType);
	mainGroup->addChild(castShadows);
	//mainGroup->AddSubItem(maxParticles);
	mainGroup->addChild(speedFactor);
	mainGroup->addChild(size);

	Property * emittersGroup = new Property(this, "Emitters");
	emittersGroup->setHasButton(true);
	emittersGroup->setButtonText("+");
	emittersGroup->setPopupMenu({ "Point", "Box", "Cylinder", "Ellipsoid", "Hollow Ellipsoid", "Ring" }, [=](TreeNode* node, int val) { onAddEmitterButtonClick(node, val); });

	Property* affectorsGroup = new Property(this, "Affectors");
	affectorsGroup->setHasButton(true);
	affectorsGroup->setButtonText("+");
	affectorsGroup->setPopupMenu({ "Linear Force", "Color Fade", "Color Fade 2", "Scale", "Rotation", "Color Interpolate", "Direction Randomize" }, [=](TreeNode* node, int val) { onAddAffectorButtonClick(node, val); });

	if (isEmittersIdentical())
	{
		for (int i = 0; i < particleSystem->getNumEmitters(); ++i)
		{
			ParticleEmitter* emitter = particleSystem->getEmitter(i);

			Property* emitterProp = new Property(this, emitter->getType());
			emitterProp->setUserData(static_cast<void*>(new int(i)));
			emitterProp->setPopupMenu({ "Remove" }, [=](TreeNode* node, int val) { onRemoveEmitter(node, val); });

			emittersGroup->addChild(emitterProp);

			//Properties
			PropBool* enabled = new PropBool(this, "Enabled", emitter->getEnabled());
			enabled->setOnChangeCallback([=](Property* prop, bool val) { onChangeEmitterEnabled(prop, val); });

			PropFloat* angle = new PropFloat(this, "Max angle", emitter->getAngle().valueDegrees());
			angle->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterAngle(prop, val); });

			PropColorPicker* startColor = new PropColorPicker(this, "Start color", emitter->getColour());
			startColor->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeEmitterStartColor(prop, val); });

			PropVector2* size = new PropVector2(this, "Size", Vector2(emitter->getOwnWidth(), emitter->getOwnHeight()));
			size->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeEmitterSize(prop, val); });

			PropVector3* direction = new PropVector3(this, "Direction", emitter->getDirection());
			direction->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeEmitterDirection(prop, val); });

			PropFloat* emissionRate = new PropFloat(this, "Emission rate", emitter->getEmissionRate());
			emissionRate->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterEmissionRate(prop, val); });

			PropVector2* duration = new PropVector2(this, "Duration", Vector2(emitter->getMinDuration(), emitter->getMaxDuration()));
			duration->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeEmitterDuration(prop, val); });

			PropVector2* velocity = new PropVector2(this, "Velocity", Vector2(emitter->getMinParticleVelocity(), emitter->getMaxParticleVelocity()));
			velocity->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeEmitterVelocity(prop, val); });

			PropVector2* repeatDelay = new PropVector2(this, "Repeat delay", Vector2(emitter->getMinRepeatDelay(), emitter->getMaxRepeatDelay()));
			repeatDelay->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeEmitterRepeatDelay(prop, val); });

			PropVector2* lifeTime = new PropVector2(this, "Life time", Vector2(emitter->getMinTimeToLive(), emitter->getMaxTimeToLive()));
			lifeTime->setOnChangeCallback([=](Property* prop, Vector2 val) { onChangeEmitterLifeTime(prop, val); });

			//emitterProp->AddSubItem(enabled);
			emitterProp->addChild(angle);
			emitterProp->addChild(emissionRate);
			emitterProp->addChild(startColor);
			emitterProp->addChild(direction);
			emitterProp->addChild(duration);
			emitterProp->addChild(velocity);
			emitterProp->addChild(repeatDelay);
			emitterProp->addChild(lifeTime);
			//emitterProp->AddSubItem(sizeGroup);

			//Specific parameters
			if (emitter->getType() == "Box" || emitter->getType() == "Cylinder" || emitter->getType() == "Ellipsoid")
			{
				Property * _emitter = new Property(this, emitter->getType());
				PropFloat* _width = new PropFloat(this, "Width", StringConverter::parseReal(emitter->getParameter("width")));
				PropFloat* _height = new PropFloat(this, "Height", StringConverter::parseReal(emitter->getParameter("height")));
				PropFloat* _depth = new PropFloat(this, "Depth", StringConverter::parseReal(emitter->getParameter("depth")));
				_width->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterBoxWidth(prop, val); });
				_height->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterBoxHeight(prop, val); });
				_depth->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterBoxDepth(prop, val); });

				_emitter->addChild(_width);
				_emitter->addChild(_height);
				_emitter->addChild(_depth);
				emitterProp->addChild(_emitter);
			}

			if (emitter->getType() == "HollowEllipsoid")
			{
				Property* _emitter = new Property(this, emitter->getType());
				PropFloat* innerWidth = new PropFloat(this, "Inner width", StringConverter::parseReal(emitter->getParameter("inner_width")));
				PropFloat* innerHeight = new PropFloat(this, "Inner height", StringConverter::parseReal(emitter->getParameter("inner_height")));
				PropFloat* innerDepth = new PropFloat(this, "Inner depth", StringConverter::parseReal(emitter->getParameter("inner_depth")));
				innerWidth->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterInnerWidth(prop, val); });
				innerHeight->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterInnerHeight(prop, val); });
				innerDepth->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterInnerDepth(prop, val); });

				_emitter->addChild(innerWidth);
				_emitter->addChild(innerHeight);
				_emitter->addChild(innerDepth);
				emitterProp->addChild(_emitter);
			}

			if (emitter->getType() == "Ring")
			{
				Property* _emitter = new Property(this, emitter->getType());
				PropFloat* innerWidth = new PropFloat(this, "Inner width", StringConverter::parseReal(emitter->getParameter("inner_width")));
				PropFloat* innerHeight = new PropFloat(this, "Inner height", StringConverter::parseReal(emitter->getParameter("inner_height")));
				innerWidth->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterInnerWidthRing(prop, val); });
				innerHeight->setOnChangeCallback([=](Property* prop, float val) { onChangeEmitterInnerHeightRing(prop, val); });

				_emitter->addChild(innerWidth);
				_emitter->addChild(innerHeight);
				emitterProp->addChild(_emitter);
			}
		}

		mainGroup->addChild(emittersGroup);
	}

	if (isAffectorsIdentical())
	{
		for (int i = 0; i < particleSystem->getNumAffectors(); ++i)
		{
			ParticleAffector* affector = particleSystem->getAffector(i);

			Property* affectorProp = new Property(this, affector->getType());
			affectorProp->setUserData(static_cast<void*>(new int(i)));
			affectorProp->setPopupMenu({ "Remove" }, [=](TreeNode* node, int val) { onRemoveAffector(node, val); });

			affectorsGroup->addChild(affectorProp);

			//Properties
			if (affector->getType() == "LinearForce")
			{
				PropVector3* forceVector = new PropVector3(this, "Force vector", StringConverter::parseVector3(affector->getParameter("force_vector")));
				forceVector->setOnChangeCallback([=](Property* prop, Vector3 val) { onChangeAffectorForce(prop, val); });

				PropComboBox* forceApp = new PropComboBox(this, "Force application", { "add", "average" });
				forceApp->setCurrentItem(affector->getParameter("force_application"));
				forceApp->setOnChangeCallback([=](Property* prop, string val) { onChangeAffectorApplication(prop, val); });

				affectorProp->addChild(forceApp);
				affectorProp->addChild(forceVector);
			}

			if (affector->getType() == "ColourFader")
			{
				PropFloat* red = new PropFloat(this, "Red", StringConverter::parseReal(affector->getParameter("red")));
				PropFloat* green = new PropFloat(this, "Green", StringConverter::parseReal(affector->getParameter("green")));
				PropFloat* blue = new PropFloat(this, "Blue", StringConverter::parseReal(affector->getParameter("blue")));
				PropFloat* alpha = new PropFloat(this, "Alpha", StringConverter::parseReal(affector->getParameter("alpha")));
				red->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFaderRed(prop, val); });
				green->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFaderGreen(prop, val); });
				blue->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFaderBlue(prop, val); });
				alpha->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFaderAlpha(prop, val); });

				affectorProp->addChild(red);
				affectorProp->addChild(green);
				affectorProp->addChild(blue);
				affectorProp->addChild(alpha);
			}

			if (affector->getType() == "ColourFader2")
			{
				Property* state1 = new Property(this, "State 1");
				PropFloat* red1 = new PropFloat(this, "Red", StringConverter::parseReal(affector->getParameter("red1")));
				PropFloat* green1 = new PropFloat(this, "Green", StringConverter::parseReal(affector->getParameter("green1")));
				PropFloat* blue1 = new PropFloat(this, "Blue", StringConverter::parseReal(affector->getParameter("blue1")));
				PropFloat* alpha1 = new PropFloat(this, "Alpha", StringConverter::parseReal(affector->getParameter("alpha1")));

				Property* state2 = new Property(this, "State 2");
				PropFloat* red2 = new PropFloat(this, "Red", StringConverter::parseReal(affector->getParameter("red2")));
				PropFloat* green2 = new PropFloat(this, "Green", StringConverter::parseReal(affector->getParameter("green2")));
				PropFloat* blue2 = new PropFloat(this, "Blue", StringConverter::parseReal(affector->getParameter("blue2")));
				PropFloat* alpha2 = new PropFloat(this, "Alpha", StringConverter::parseReal(affector->getParameter("alpha2")));

				PropFloat* changeTime = new PropFloat(this, "Change time", StringConverter::parseReal(affector->getParameter("state_change")));

				red1->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Red1(prop, val); });
				green1->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Green1(prop, val); });
				blue1->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Blue1(prop, val); });
				alpha1->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Alpha1(prop, val); });

				red2->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Red2(prop, val); });
				green2->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Green2(prop, val); });
				blue2->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Blue2(prop, val); });
				alpha2->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2Alpha2(prop, val); });

				changeTime->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorFader2ChangeTime(prop, val); });

				state1->addChild(red1);
				state1->addChild(green1);
				state1->addChild(blue1);
				state1->addChild(alpha1);

				state2->addChild(red2);
				state2->addChild(green2);
				state2->addChild(blue2);
				state2->addChild(alpha2);

				affectorProp->addChild(state1);
				affectorProp->addChild(state2);
				affectorProp->addChild(changeTime);
			}

			if (affector->getType() == "Scaler")
			{
				PropFloat* rate = new PropFloat(this, "Scale rate", StringConverter::parseReal(affector->getParameter("rate")));
				rate->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorScalerRate(prop, val); });

				affectorProp->addChild(rate);
			}

			if (affector->getType() == "Rotator")
			{
				PropFloat* speedStart = new PropFloat(this, "Speed range start", StringConverter::parseReal(affector->getParameter("rotation_speed_range_start")));
				PropFloat* speedEnd = new PropFloat(this, "Speed range end", StringConverter::parseReal(affector->getParameter("rotation_speed_range_end")));
				PropFloat* rotStart = new PropFloat(this, "Rotation range start", StringConverter::parseReal(affector->getParameter("rotation_range_start")));
				PropFloat* rotEnd = new PropFloat(this, "Rotation range end", StringConverter::parseReal(affector->getParameter("rotation_range_end")));

				speedStart->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorRotatorSpeedStart(prop, val); });
				speedEnd->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorRotatorSpeedEnd(prop, val); });
				rotStart->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorRotatorRotStart(prop, val); });
				rotEnd->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorRotatorRotEnd(prop, val); });

				affectorProp->addChild(speedStart);
				affectorProp->addChild(speedEnd);
				affectorProp->addChild(rotStart);
				affectorProp->addChild(rotEnd);
			}

			if (affector->getType() == "ColourInterpolator")
			{
				Property* state1 = new Property(this, "State 1");
				PropFloat* time1 = new PropFloat(this, "Time 1", StringConverter::parseReal(affector->getParameter("time0")));
				PropColorPicker* color1 = new PropColorPicker(this, "Color 1", StringConverter::parseColourValue(affector->getParameter("colour0")));
				state1->addChild(time1);
				state1->addChild(color1);
				time1->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorInterpolateTime1(prop, val); });
				color1->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeAffectorColorInterpolateColor1(prop, val); });

				Property* state2 = new Property(this, "State 2");
				PropFloat* time2 = new PropFloat(this, "Time 2", StringConverter::parseReal(affector->getParameter("time1")));
				PropColorPicker* color2 = new PropColorPicker(this, "Color 2", StringConverter::parseColourValue(affector->getParameter("colour1")));
				state2->addChild(time2);
				state2->addChild(color2);
				time2->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorInterpolateTime2(prop, val); });
				color2->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeAffectorColorInterpolateColor2(prop, val); });

				Property* state3 = new Property(this, "State 3");
				PropFloat* time3 = new PropFloat(this, "Time 3", StringConverter::parseReal(affector->getParameter("time2")));
				PropColorPicker* color3 = new PropColorPicker(this, "Color 3", StringConverter::parseColourValue(affector->getParameter("colour2")));
				state3->addChild(time3);
				state3->addChild(color3);
				time3->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorInterpolateTime3(prop, val); });
				color3->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeAffectorColorInterpolateColor3(prop, val); });

				Property* state4 = new Property(this, "State 4");
				PropFloat* time4 = new PropFloat(this, "Time 4", StringConverter::parseReal(affector->getParameter("time3")));
				PropColorPicker* color4 = new PropColorPicker(this, "Color 4", StringConverter::parseColourValue(affector->getParameter("colour3")));
				state4->addChild(time4);
				state4->addChild(color4);
				time4->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorInterpolateTime4(prop, val); });
				color4->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeAffectorColorInterpolateColor4(prop, val); });

				Property* state5 = new Property(this, "State 5");
				PropFloat* time5 = new PropFloat(this, "Time 5", StringConverter::parseReal(affector->getParameter("time4")));
				PropColorPicker* color5 = new PropColorPicker(this, "Color 5", StringConverter::parseColourValue(affector->getParameter("colour4")));
				state5->addChild(time5);
				state5->addChild(color5);
				time5->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorInterpolateTime5(prop, val); });
				color5->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeAffectorColorInterpolateColor5(prop, val); });

				Property* state6 = new Property(this, "State 6");
				PropFloat* time6 = new PropFloat(this, "Time 6", StringConverter::parseReal(affector->getParameter("time5")));
				PropColorPicker* color6 = new PropColorPicker(this, "Color 6", StringConverter::parseColourValue(affector->getParameter("colour5")));
				state6->addChild(time6);
				state6->addChild(color6);
				time6->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorColorInterpolateTime6(prop, val); });
				color6->setOnChangeCallback([=](Property* prop, ColourValue val) { onChangeAffectorColorInterpolateColor6(prop, val); });

				affectorProp->addChild(state1);
				affectorProp->addChild(state2);
				affectorProp->addChild(state3);
				affectorProp->addChild(state4);
				affectorProp->addChild(state5);
				affectorProp->addChild(state6);
			}

			if (affector->getType() == "DirectionRandomiser")
			{
				PropFloat* randomness = new PropFloat(this, "Randomness", StringConverter::parseReal(affector->getParameter("randomness")));
				PropFloat* scope = new PropFloat(this, "Scope", StringConverter::parseReal(affector->getParameter("scope")));
				PropBool* keepVelocity = new PropBool(this, "Keep velocity", StringConverter::parseBool(affector->getParameter("keep_velocity")));
				randomness->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorDirectionRandomness(prop, val); });
				scope->setOnChangeCallback([=](Property* prop, float val) { onChangeAffectorDirectionScope(prop, val); });
				keepVelocity->setOnChangeCallback([=](Property* prop, bool val) { onChangeAffectorDirectionKeepVelocity(prop, val); });

				affectorProp->addChild(randomness);
				affectorProp->addChild(scope);
				affectorProp->addChild(keepVelocity);
			}
		}

		mainGroup->addChild(affectorsGroup);
	}

	addProperty(mainGroup);
}

void ParticleSystemEditor2::update()
{
	__super::update();
}

bool ParticleSystemEditor2::isEmittersIdentical()
{
	bool result = true;

	auto sceneNodes = getSceneNodes();

	SceneNode* node = sceneNodes[0];
	ParticleSystem* ps = (ParticleSystem*)node->getAttachedObject(0);

	for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if (!result)
			break;

		SceneNode* currNode = *it;
		ParticleSystem* currPs = (ParticleSystem*)currNode->getAttachedObject(0);

		if (ps->getNumEmitters() != currPs->getNumEmitters())
		{
			result = false;
			break;
		}

		for (int i = 0; i < ps->getNumEmitters(); ++i)
		{
			ParticleEmitter* emitter = ps->getEmitter(i);
			ParticleEmitter* currEmitter = currPs->getEmitter(i);

			if (currEmitter->getType() != emitter->getType())
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

bool ParticleSystemEditor2::isAffectorsIdentical()
{
	bool result = true;

	auto sceneNodes = getSceneNodes();

	SceneNode* node = sceneNodes[0];
	ParticleSystem* ps = (ParticleSystem*)node->getAttachedObject(0);

	for (std::vector<SceneNode*>::iterator it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		if (!result)
			break;

		SceneNode* currNode = *it;
		ParticleSystem* currPs = (ParticleSystem*)currNode->getAttachedObject(0);

		if (ps->getNumAffectors() != currPs->getNumAffectors())
		{
			result = false;
			break;
		}

		for (int i = 0; i < ps->getNumAffectors(); ++i)
		{
			ParticleAffector* affector = ps->getAffector(i);
			ParticleAffector* currAffector = currPs->getAffector(i);

			if (currAffector->getType() != affector->getType())
			{
				result = false;
				break;
			}
		}
	}

	return result;
}

void ParticleSystemEditor2::onAddEmitterButtonClick(TreeNode* node, int val)
{
	//"Point", "Box", "Cylinder", "Ellipsoid", "Hollow Ellipsoid", "Ring"

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);

		if (val == 0) // Point
		{
			particleSystem->addEmitter("Point");
		}

		if (val == 1) // Box
		{
			particleSystem->addEmitter("Box");
		}

		if (val == 2) // Cylinder
		{
			particleSystem->addEmitter("Cylinder");
		}

		if (val == 3) // Ellipsoid
		{
			particleSystem->addEmitter("Ellipsoid");
		}

		if (val == 4) // Hollow Ellipsoid
		{
			particleSystem->addEmitter("HollowEllipsoid");
		}

		if (val == 5) // Ring
		{
			particleSystem->addEmitter("Ring");
		}
	}

	updateEditor();
}

void ParticleSystemEditor2::onAddAffectorButtonClick(TreeNode* node, int val)
{
	//"Linear Force", "Color Fade", "Color Fade 2", "Scale", "Rotation", "Color Interpolate", "Direction Randomize"

	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);

		if (val == 0) // Linear Force
		{
			particleSystem->addAffector("LinearForce");
		}

		if (val == 1) // Color Fade
		{
			particleSystem->addAffector("ColourFader");
		}

		if (val == 2) // Color Fade 2
		{
			particleSystem->addAffector("ColourFader2");
		}

		if (val == 3) // Scale
		{
			particleSystem->addAffector("Scaler");
		}

		if (val == 4) // Rotation
		{
			particleSystem->addAffector("Rotator");
		}

		if (val == 5) // Color Interpolate
		{
			particleSystem->addAffector("ColourInterpolator");
		}

		if (val == 6) // Direction Randomize
		{
			particleSystem->addAffector("DirectionRandomiser");
		}
	}

	updateEditor();
}

void ParticleSystemEditor2::onRemoveEmitter(TreeNode* node, int val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(node->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->removeEmitter(index);
	}

	updateEditor();
}

void ParticleSystemEditor2::onRemoveAffector(TreeNode* node, int val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(node->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->removeAffector(index);
	}

	updateEditor();
}

void ParticleSystemEditor2::onReplay(TreeNode* node)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		
		for (int i = 0; i < ps->getNumEmitters(); ++i)
		{
			ParticleEmitter* em = ps->getEmitter(i);

			em->setEnabled(false);
			em->setEnabled(true);
		}
	}
}

void ParticleSystemEditor2::onDropMaterial(TreeNode* prop, TreeNode* from)
{
	string fullPath = from->getPath();

	((PropString*)prop)->setValue(fullPath);
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setMaterialName(fullPath, "Assets");
	}
}

void ParticleSystemEditor2::onChangeEnabled(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setEmitting(val);
	}
}

void ParticleSystemEditor2::onChangeTransformSpace(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setKeepParticlesInLocalSpace(val);
	}
}

void ParticleSystemEditor2::onChangeCastShadows(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setCastShadows(val);
	}
}

void ParticleSystemEditor2::onChangeMaxParticles(Property* prop, int val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setParticleQuota((size_t)val);
	}
}

void ParticleSystemEditor2::onChangeSpeed(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setSpeedFactor(val);
	}
}

void ParticleSystemEditor2::onChangeSize(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->setDefaultWidth(val.x);
		particleSystem->setDefaultHeight(val.y);
	}
}

void ParticleSystemEditor2::onChangeBillboardType(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* particleSystem = (ParticleSystem*)(*it)->getAttachedObject(0);
		particleSystem->getRenderer()->setParameter("billboard_type", val);
	}
}

void ParticleSystemEditor2::onChangeEmitterEnabled(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setEnabled(val);
	}
}

void ParticleSystemEditor2::onChangeEmitterAngle(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setAngle(Radian(Degree(val)));
	}
}

void ParticleSystemEditor2::onChangeEmitterStartColor(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setColour(val);
	}
}

void ParticleSystemEditor2::onChangeEmitterSize(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setDimensions(val.x, val.y);
	}
}

void ParticleSystemEditor2::onChangeEmitterEmissionRate(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setEmissionRate(val);
	}
}

void ParticleSystemEditor2::onChangeEmitterDirection(Property* prop, Vector3 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setDirection(val);
	}
}

void ParticleSystemEditor2::onChangeEmitterDuration(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setMinDuration(val.x);
		ps->getEmitter(index)->setMaxDuration(val.y);
	}
}

void ParticleSystemEditor2::onChangeEmitterVelocity(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setMinParticleVelocity(val.x);
		ps->getEmitter(index)->setMaxParticleVelocity(val.y);
	}
}

void ParticleSystemEditor2::onChangeEmitterRepeatDelay(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setMinRepeatDelay(val.x);
		ps->getEmitter(index)->setMaxRepeatDelay(val.y);
	}
}

void ParticleSystemEditor2::onChangeEmitterLifeTime(Property* prop, Vector2 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setMinTimeToLive(val.x);
		ps->getEmitter(index)->setMaxTimeToLive(val.y);
	}
}

void ParticleSystemEditor2::onChangeEmitterBoxWidth(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("width", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterBoxHeight(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("height", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterBoxDepth(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("depth", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterInnerWidth(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("inner_width", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterInnerHeight(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("inner_height", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterInnerDepth(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("inner_depth", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterInnerWidthRing(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("inner_width", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeEmitterInnerHeightRing(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getEmitter(index)->setParameter("inner_height", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorForce(Property* prop, Vector3 val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("force_vector", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorApplication(Property* prop, std::string val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("force_application", val);
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFaderRed(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("red", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFaderGreen(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("green", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFaderBlue(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("blue", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFaderAlpha(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("alpha", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Red1(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("red1", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Green1(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("green1", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Blue1(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("blue1", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Alpha1(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("alpha1", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Red2(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("red2", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Green2(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("green2", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Blue2(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("blue2", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2Alpha2(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("alpha2", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorFader2ChangeTime(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("state_change", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorScalerRate(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("rate", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorRotatorSpeedStart(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("rotation_speed_range_start", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorRotatorSpeedEnd(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("rotation_speed_range_end", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorRotatorRotStart(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("rotation_range_start", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorRotatorRotEnd(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("rotation_range_end", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateTime1(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("time0", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateColor1(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("colour0", StringConverter::StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateTime2(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("time1", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateColor2(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("colour1", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateTime3(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("time2", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateColor3(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("colour2", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateTime4(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("time3", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateColor4(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("colour3", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateTime5(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("time4", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateColor5(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("colour4", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateTime6(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("time5", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorColorInterpolateColor6(Property* prop, ColourValue val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("colour5", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorDirectionRandomness(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("randomness", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorDirectionScope(Property* prop, float val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("scope", StringConverter::toString(val));
	}
}

void ParticleSystemEditor2::onChangeAffectorDirectionKeepVelocity(Property* prop, bool val)
{
	auto sceneNodes = getSceneNodes();
	int index = *static_cast<int*>(prop->parent->getUserData());

	for (auto it = sceneNodes.begin(); it != sceneNodes.end(); ++it)
	{
		ParticleSystem* ps = (ParticleSystem*)(*it)->getAttachedObject(0);
		ps->getAffector(index)->setParameter("keep_velocity", StringConverter::toString(val));
	}
}
