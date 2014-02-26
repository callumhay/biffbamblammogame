/**
 * Skybox.cpp
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "Skybox.h"
#include "GameViewConstants.h"

#include "../BlammoEngine/Camera.h"
#include "../BlammoEngine/GeometryMaker.h"

#include "../ResourceManager.h"

Skybox::Skybox() :
skyboxEffect(ResourceManager::GetInstance()->GetImgTextureResource(
             GameViewConstants::GetInstance()->TEXTURE_STARFIELD_FG, Texture::Trilinear),
             ResourceManager::GetInstance()->GetImgTextureResource(
             GameViewConstants::GetInstance()->TEXTURE_STARFIELD_BG, Texture::Trilinear)),
skyboxDispList(0) {

	// Create a display list for the skybox geometry
	this->skyboxDispList = glGenLists(1);
	assert(this->skyboxDispList != 0);

	glNewList(this->skyboxDispList, GL_COMPILE);
	Skybox::DrawSkyboxGeometry(5, 0.5f); //(Camera::FAR_PLANE_DIST - 1.0f) / SQRT_2);
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

	glPushAttrib(GL_ENABLE_BIT | GL_TEXTURE_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);

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