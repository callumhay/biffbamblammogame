#ifndef __CGFXEFFECT_H__
#define __CGFXEFFECT_H__

#include "../Utils/Includes.h"
#include <string>

/**
 * Base class for loading and dealing with CgFx shaders.
 */
class CgFxEffect {
protected:
	CGtechnique currTechnique;

	virtual void SetupBeforePasses() = 0;

	/**
	 * Draw the given display list in the given pass using
	 * the Cg runtime.
	 */
	void DrawPass(CGpass pass, GLint displayListID) {
		cgSetPassState(pass);
		glCallList(displayListID);
		cgResetPassState(pass);
	}

public:
	CgFxEffect(CGtechnique currTech) : currTechnique(currTech) {};
	virtual ~CgFxEffect() {};

	/**
	 * Draw the given display list with this effect
	 * applied to it.
	 */
	void Draw(GLint displayListID) {
		this->SetupBeforePasses();
		
		// Draw each pass of this effect
		CGpass currPass = cgGetFirstPass(this->currTechnique);
		while (currPass) {
			this->DrawPass(currPass, displayListID);
			currPass = cgGetNextPass(currPass);
		}
	}
	

};

#endif