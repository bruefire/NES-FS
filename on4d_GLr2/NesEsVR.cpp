#include "NesEsVR.h"
#include "engine3dWinOVR.h"
using WorldGeo = engine3d::WorldGeo;



void NesEsVR::initGlScnene()
{
	owner->GL_InitScene();
}


void NesEsVR::updateGlScene()
{
	owner->update();
}


void NesEsVR::disposeGlScene()
{
	owner->GL_DisposeScene();
}
