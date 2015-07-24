#include "Giraffe.h"

void GiraffeElement::setup(ofVec2f StartPos, eANIM_MOVE_STATE eState, float fSize)
{
	_DyanmicGiraffe.setup(fSize);
	_eMoveState = eState;

	_AnimMove.setPosition(StartPos);


}

//--------------------------------------------------------------
void GiraffeElement::update(float fDelta)
{

}

//--------------------------------------------------------------
void GiraffeElement::draw()
{

}