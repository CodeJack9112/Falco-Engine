#include "Screen.h"
#include "Engine.h"

using namespace Ogre;
using namespace std;

float Screen::GetScreenScaleFactor()
{
	if (Ogre::Root::getSingleton().getRenderSystem()->_getViewport() == nullptr)
		return 1;

	int w = Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getActualWidth();
	int h = Ogre::Root::getSingleton().getRenderSystem()->_getViewport()->getActualHeight();
	double fw = w;
	double fh = h;
	int refW = w;
	int refH = h;

	SceneManager* mgr = GetEngine->GetSceneManager();
	if (UICanvasFactory::uiCanvas[mgr] != nullptr)
	{
		refW = UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenWidth();
		refH = UICanvasFactory::uiCanvas[mgr]->GetReferenceScreenHeight();
	}

	double offsetW = w - refW;
	double offsetH = h - refH;

	if (refW > 0)
		fw = refW;
	if (refH > 0)
		fh = refH;

	double sclW = w / fw;
	double sclH = h / fh;

	if (UICanvasFactory::uiCanvas[mgr] != nullptr)
	{
		if (UICanvasFactory::uiCanvas[mgr]->GetScaleMode() == UICanvas::ScaleMode::AdjustWithScreenSize)
		{
			sclW = 1;
			sclH = 1;
		}
	}

	return sclH;
}

Vector2 Screen::GetScreenSize(Viewport* viewport)
{
	Viewport* view = Ogre::Root::getSingleton().getRenderSystem()->_getViewport();

	if (viewport != nullptr)
		view = viewport;

	if (view == nullptr)
		return Vector2(1, 1);

	int w = view->getActualWidth();
	int h = view->getActualHeight();

	return Vector2(w, h);
}
