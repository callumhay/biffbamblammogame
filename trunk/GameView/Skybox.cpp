/**
 * Skybox.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011-2013
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#include "Skybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../ResourceManager.h"

Skybox::Skybox() :
skyboxEffect(ResourceManager::GetInstance()->GetImgTextureResource(
                 GameViewConstants::GetInstance()->TEXTURE_STARFIELD, Texture::Trilinear)),
skyboxDispList(0) {

	// Create a display list for the skybox geometry
	this->skyboxDispList = glGenLists(1);
	assert(this->skyboxDispList != 0);

	glNewList(this->skyboxDispList, GL_COMPILE);
	Skybox::DrawSkyboxGeometry(4, (Camera::FAR_PLANE_DIST - 1.0f) / SQRT_2);
	glEndList();
}

Skybox::~Skybox() {
	// Clean up the display list for the skybox
	glDeleteLists(this->skyboxDispList, 1);
	this->skyboxDispList = 0;
}

void Skybox::Draw(const Camera& camera) {
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    glLoadIdentity();

	Matrix4x4 camTransform = camera.GetViewTransform();
	camTransform.setTranslation(Point3D(0,0,0));
	glMultMatrixf(camTransform.begin());

	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT);
	glDisable(GL_DEPTH_TEST);

	this->skyboxEffect.Draw(camera, this->skyboxDispList);

	glPopAttrib();
	glPopMatrix();

	debug_cg_state();
}

/**
* Static private helper function for drawing the skybox geometry with
* the given texture tiling for pure, clamped tiling use the default value of 1.
*/
void Skybox::DrawSkyboxGeometry(int texTiling, float width) {
    glColor4f(1,1,1,1);
    glBegin(GL_QUADS); 
    // Render the front quad
    glTexCoord2i(0, texTiling);			glVertex3f(  width,  width, -width );
    glTexCoord2i(texTiling, texTiling); glVertex3f( -width,  width, -width );
    glTexCoord2i(texTiling, 0);			glVertex3f( -width, -width, -width );
    glTexCoord2i(0, 0);					glVertex3f(  width, -width, -width );

    // Render the left quad
    glTexCoord2i(0, texTiling);			glVertex3f(  width,  width,  width );
    glTexCoord2i(texTiling, texTiling); glVertex3f(  width,  width, -width );
    glTexCoord2i(texTiling, 0);			glVertex3f(  width, -width, -width );
    glTexCoord2i(0, 0);					glVertex3f(  width, -width,  width );

    // Render the back quad
    glTexCoord2i(0, texTiling);			glVertex3f( -width,  width,  width );
    glTexCoord2i(texTiling, texTiling); glVertex3f(  width,  width,  width );
    glTexCoord2i(texTiling, 0);			glVertex3f(  width, -width,  width );
    glTexCoord2i(0, 0);					glVertex3f( -width, -width,  width );

    // Render the right quad 
    glTexCoord2i(0, texTiling);			glVertex3f( -width,  width, -width );
    glTexCoord2i(texTiling, texTiling); glVertex3f( -width,  width,  width );
    glTexCoord2i(texTiling, 0);			glVertex3f( -width, -width,  width );
    glTexCoord2i(0, 0);					glVertex3f( -width, -width, -width );

    // Render the top quad 
    glTexCoord2i(texTiling, texTiling); glVertex3f(  width,  width, -width );
    glTexCoord2i(texTiling, 0);		    glVertex3f(  width,  width,  width );
    glTexCoord2i(0, 0);					glVertex3f( -width,  width,  width );
    glTexCoord2i(0, texTiling);			glVertex3f( -width,  width, -width );

    // Render the bottom quad
    glTexCoord2i(0, 0);					glVertex3f( -width, -width, -width );
    glTexCoord2i(0, texTiling);			glVertex3f( -width, -width,  width );
    glTexCoord2i(texTiling, texTiling); glVertex3f(  width, -width,  width );
    glTexCoord2i(texTiling, 0);			glVertex3f(  width, -width, -width );
    glEnd();
}