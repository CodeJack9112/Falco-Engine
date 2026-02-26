#ifndef _NULLSCHEMEHANDLER_H
#define _NULLSCHEMEHANDLER_H

#include "OgreMaterialManager.h"
#include "OgreTechnique.h"

/** Class for skipping materials which do not have the scheme defined
 */
class MaterialSchemeHandler : public Ogre::MaterialManager::Listener
{
public:
    /** @copydoc MaterialManager::Listener::handleSchemeNotFound */
    virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, 
        const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, 
        const Ogre::Renderable* rend)
    {
        Technique* _tech = originalMaterial->getTechnique(schemeName);

        if (_tech != nullptr)
            return _tech;

        //Creating a technique so the handler only gets called once per material
        _tech = originalMaterial->createTechnique();
        _tech->setName(schemeName);

        _tech->removeAllPasses();
        _tech->setSchemeName(schemeName);

        return _tech;
    }
};

#endif
