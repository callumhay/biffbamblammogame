#include "GeometryMaker.h"

GeometryMaker* GeometryMaker::instance = NULL;

GeometryMaker::GeometryMaker() : quadDL(0), cubeDL(0) {
	
	// Initialize all the geometry display lists
	bool result = this->InitializeQuadDL();
	assert(result);
	result = this->InitializeCubeDL();
	assert(result);

}

GeometryMaker::~GeometryMaker() {
	// Delete all the display lists
	glDeleteLists(this->quadDL, 1);
	this->quadDL = 0;
	glDeleteLists(this->cubeDL, 1);
	this->cubeDL = 0;
}

/**
 * Initializes the display list for a simple quad -
 * has basic texture coordinates and a normal pointing out at (0,0,1).
 */
bool GeometryMaker::InitializeQuadDL() {
	this->quadDL = glGenLists(1);
	glNewList(this->quadDL, GL_COMPILE);
	
	glBegin(GL_QUADS);
		glNormal3i(0, 0, 1);
		glTexCoord2i(0, 0); glVertex2f(-0.5f, -0.5f);
		glTexCoord2i(1, 0); glVertex2f( 0.5f, -0.5f);
		glTexCoord2i(1, 1); glVertex2f( 0.5f,  0.5f);
		glTexCoord2i(0, 1); glVertex2f(-0.5f,  0.5f);
	glEnd();
	glEndList();

	return this->quadDL != 0;
}

bool GeometryMaker::InitializeCubeDL() {
	
	this->cubeDL = glGenLists(1);
	glNewList(this->cubeDL, GL_COMPILE);
	
	glBegin(GL_QUADS);
		glNormal3i(0, 1, 0);
    glVertex3f( 0.5f, 0.5f,-0.5f);	// Top Right Of The Quad (Top)
    glVertex3f(-0.5f, 0.5f,-0.5f);	// Top Left Of The Quad (Top)
    glVertex3f(-0.5f, 0.5f, 0.5f);	// Bottom Left Of The Quad (Top)
    glVertex3f( 0.5f, 0.5f, 0.5f);	// Bottom Right Of The Quad (Top)

		glNormal3i(0, -1, 0);
    glVertex3f( 0.5f,-0.5f, 0.5f);	// Top Right Of The Quad (Bottom)
    glVertex3f(-0.5f,-0.5f, 0.5f);	// Top Left Of The Quad (Bottom)
    glVertex3f(-0.5f,-0.5f,-0.5f);	// Bottom Left Of The Quad (Bottom)
    glVertex3f( 0.5f,-0.5f,-0.5f);	// Bottom Right Of The Quad (Bottom)

		glNormal3i(0, 0, 1);
    glVertex3f( 0.5f, 0.5f, 0.5f);	// Top Right Of The Quad (Front)
    glVertex3f(-0.5f, 0.5f, 0.5f);	// Top Left Of The Quad (Front)
    glVertex3f(-0.5f,-0.5f, 0.5f);	// Bottom Left Of The Quad (Front)
    glVertex3f( 0.5f,-0.5f, 0.5f);	// Bottom Right Of The Quad (Front)

		glNormal3i(0, 0, -1);
    glVertex3f( 0.5f,-0.5f,-0.5f);	// Top Right Of The Quad (Back)
    glVertex3f(-0.5f,-0.5f,-0.5f);	// Top Left Of The Quad (Back)
    glVertex3f(-0.5f, 0.5f,-0.5f);	// Bottom Left Of The Quad (Back)
    glVertex3f( 0.5f, 0.5f,-0.5f);	// Bottom Right Of The Quad (Back)

		glNormal3i(-1, 0, 0);
    glVertex3f(-0.5f, 0.5f, 0.5f);	// Top Right Of The Quad (Left)
    glVertex3f(-0.5f, 0.5f,-0.5f);	// Top Left Of The Quad (Left)
    glVertex3f(-0.5f,-0.5f,-0.5f);	// Bottom Left Of The Quad (Left)
    glVertex3f(-0.5f,-0.5f, 0.5f);	// Bottom Right Of The Quad (Left)

		glNormal3i(1, 0, 0);
    glVertex3f( 0.5f, 0.5f,-0.5f);	// Top Right Of The Quad (Right)
    glVertex3f( 0.5f, 0.5f, 0.5f);	// Top Left Of The Quad (Right)
    glVertex3f( 0.5f,-0.5f, 0.5f);	// Bottom Left Of The Quad (Right)
    glVertex3f( 0.5f,-0.5f,-0.5f);	// Bottom Right Of The Quad (Right)
		
	glEnd();
	glEndList();
	return this->cubeDL != 0;
}