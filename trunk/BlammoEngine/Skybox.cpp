/**
 * Skybox.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "Skybox.h"
#include "Texture.h"
#include "Mesh.h"
#include "ObjReader.h"

#include "../ResourceManager.h"

// Default, private constructor for the Skybox object
Skybox::Skybox(Texture* tex) : skyboxTex(tex) {
	assert(tex != NULL);
}

Skybox::~Skybox() {
	// Release the skybox texture
	bool success = ResourceManager::GetInstance()->ReleaseTextureResource(this->skyboxTex);
	assert(success);
}

/**
 * Draw the skybox... in all its skyboxey glory!
 */
void Skybox::Draw(const Camera& camera) {
	// Draw the skybox:  The skybox consists of a cube with inward facing quads, textured with the skybox texture
	glPushMatrix();

	// Load just the rotation/orientation of the view matrix
	Vector3D camView = camera.GetNormalizedViewVector();
	Matrix4x4 camTransform = camera.GetViewTransform();
	camTransform.setTranslation(Point3D(0,0,0));
	glLoadMatrixf(camTransform.begin());

  glPushAttrib(GL_ENABLE_BIT);
  glEnable(GL_TEXTURE_2D);

  this->skyboxTex->BindTexture();
	Skybox::DrawSkyboxGeometry(1, (Camera::FAR_PLANE_DIST - 1.0f) / SQRT_2);
	this->skyboxTex->UnbindTexture();

  glPopAttrib();
  glPopMatrix();
}


/**
 * Static creator method for making a Skybox object using a cubemap. 
 */
/*
Skybox* Skybox::CreateSkybox(const std::string cubeTexFilepaths[6]) {

	// Start by making the cubemap texture
	TextureCube* skyboxTex =  ... (cubeTexFilepaths, Texture::Nearest);
	if (skyboxTex == NULL) {
		return NULL;
	}

	// The skybox consists of a cube with inward facing quads, textured with the given cube textures


	Skybox* newSkybox = new Skybox(skyboxGeom, skyboxTex);
	return newSkybox;
}
*/

/**
 * Static creator method for making a Skybox object using a single texture. 
 */
Skybox* Skybox::CreateSkybox(const std::string& texFilepath) {
	// Load the texture from the resource manager
	Texture* skyboxTex = ResourceManager::GetInstance()->GetImgTextureResource(texFilepath, Texture::Trilinear, GL_TEXTURE_2D);
	if (skyboxTex == NULL) {
		assert(false);
		return NULL;
	}

	Skybox* newSkybox = new Skybox(skyboxTex);
	return newSkybox;
}

/**
 * Static private helper function for drawing the skybox geometry with
 * the given texture tiling for pure, clamped tiling use the default value of 1.
 */
void Skybox::DrawSkyboxGeometry(int texTiling, float width) {
	glColor4f(1,1,1,1);
  glBegin(GL_QUADS); 
		// Render the front quad
    glTexCoord2i(0, texTiling);					glVertex3f(  width,  width, -width );
		glTexCoord2i(texTiling, texTiling); glVertex3f( -width,  width, -width );
		glTexCoord2i(texTiling, 0);					glVertex3f( -width, -width, -width );
		glTexCoord2i(0, 0);									glVertex3f(  width, -width, -width );

		// Render the left quad
    glTexCoord2i(0, texTiling);					glVertex3f(  width,  width,  width );
		glTexCoord2i(texTiling, texTiling); glVertex3f(  width,  width, -width );
		glTexCoord2i(texTiling, 0);					glVertex3f(  width, -width, -width );
		glTexCoord2i(0, 0);									glVertex3f(  width, -width,  width );

		// Render the back quad
    glTexCoord2i(0, texTiling);					glVertex3f( -width,  width,  width );
		glTexCoord2i(texTiling, texTiling); glVertex3f(  width,  width,  width );
		glTexCoord2i(texTiling, 0);					glVertex3f(  width, -width,  width );
		glTexCoord2i(0, 0);									glVertex3f( -width, -width,  width );

		// Render the right quad 
    glTexCoord2i(0, texTiling);					glVertex3f( -width,  width, -width );
		glTexCoord2i(texTiling, texTiling); glVertex3f( -width,  width,  width );
		glTexCoord2i(texTiling, 0);					glVertex3f( -width, -width,  width );
		glTexCoord2i(0, 0);									glVertex3f( -width, -width, -width );
		
		// Render the top quad 
    glTexCoord2i(texTiling, texTiling); glVertex3f(  width,  width, -width );
		glTexCoord2i(texTiling, 0);					glVertex3f(  width,  width,  width );
		glTexCoord2i(0, 0);									glVertex3f( -width,  width,  width );
		glTexCoord2i(0, texTiling);					glVertex3f( -width,  width, -width );

		 // Render the bottom quad
		glTexCoord2i(0, 0);									glVertex3f( -width, -width, -width );
		glTexCoord2i(0, texTiling);					glVertex3f( -width, -width,  width );
		glTexCoord2i(texTiling, texTiling); glVertex3f(  width, -width,  width );
    glTexCoord2i(texTiling, 0);					glVertex3f(  width, -width, -width );
  glEnd();
}
