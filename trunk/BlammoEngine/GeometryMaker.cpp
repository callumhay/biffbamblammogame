/**
 * GeometryMaker.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial-Share Alike 2.5 Licence
 * Callum Hay, 2009
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar licence to this one.
 */

#include "GeometryMaker.h"
#include "Mesh.h"

GeometryMaker* GeometryMaker::instance = NULL;

GeometryMaker::GeometryMaker() : quadDL(0), cubeDL(0), sphereDL(0) {
	
	// Initialize all the geometry display lists
	bool result = this->InitializeQuadDL();
	assert(result);
	result = this->InitializeCubeDL();
	assert(result);
	result = this->InitializeSphereDL();
	assert(result);
}

GeometryMaker::~GeometryMaker() {
	// Delete all the display lists
	glDeleteLists(this->quadDL, 1);
	this->quadDL = 0;
	glDeleteLists(this->cubeDL, 1);
	this->cubeDL = 0;
	glDeleteLists(this->sphereDL, 1);
	this->sphereDL = 0;
}

/**
 * Initializes the display list for a simple quad -
 * has basic texture coordinates and a normal pointing out at (0,0,1).
 * Returns: true on success, false otherwise.
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

/**
 * Initializes the display list for a simple, unit cube -
 * has no texture coordinates, but has normals set.
 * Returns: true on success, false otherwise.
 */
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

/**
 * Initializes the display list for a simple, unit sphere -
 * has no texture coordinates, but has normals set.
 * Returns: true on success, false otherwise.
 */
bool GeometryMaker::InitializeSphereDL() {
	this->sphereDL = GeometryMaker::CreateSphereDL(0.5f, 0.5f, GeometryMaker::NUM_SPHERE_STACKS, GeometryMaker::NUM_SPHERE_SLICES);
	return this->sphereDL != 0;
}

/**
 * Creates a display list for a simple, unit sphere -
 * has no texture coordinates, but has normals set. Also will have the tesselation specified.
 * Returns: The drawlist generated for the sphere.
 */
GLuint GeometryMaker::CreateSphereDL(float horizRadius, float vertRadius, unsigned int stacks, unsigned int slices) {
	// Number of vertices used for the sphere
	const unsigned int vertexCount = 2 * stacks * (slices + 1);

	// Create arrays to hold temporary geometry data
	std::vector<Point3D> vertices;
	vertices.reserve(vertexCount);
	std::vector<Vector3D> normals;
	normals.reserve(vertexCount);
	std::vector<Point2D> texCoords;
	texCoords.reserve(vertexCount);

	// Constants used to generate the sphere
	const float deltaRingAngle = (M_PI / stacks);
  const float deltaSegAngle  = (2.0f * M_PI / static_cast<float>(slices));

	// Generate the group of rings for the sphere
	for (unsigned int ring = 0; ring < stacks; ring++) {
		float r0 = horizRadius * sin(ring * deltaRingAngle);
		float r1 = horizRadius * sin((ring + 1) * deltaRingAngle);
		float y0 = vertRadius * cos(ring * deltaRingAngle);
		float y1 = vertRadius * cos((ring + 1) * deltaRingAngle);

		// Generate the group of segments for the current ring
		for (unsigned int seg = 0; seg < (slices + 1); seg++) { 
			float x0 =  r0 * sin(seg * deltaSegAngle);
			float z0 =  r0 * cos(seg * deltaSegAngle);
			float x1 =  r1 * sin(seg * deltaSegAngle);
			float z1 =  r1 * cos(seg * deltaSegAngle);

			vertices.push_back(Point3D(x0/2.0f, y0/2.0f, z0/2.0f));
			normals.push_back(Vector3D(x0/2.0f, y0/2.0f, z0/2.0f));
			texCoords.push_back(Point2D((static_cast<float>(slices) - seg) / static_cast<float>(slices),
																	 1.0f - (static_cast<float>(ring) / static_cast<float>(stacks)))); 

			vertices.push_back(Point3D(x1/2.0f, y1/2.0f, z1/2.0f));
			normals.push_back(Vector3D(x1/2.0f, y1/2.0f, z1/2.0f));
			texCoords.push_back(Point2D((static_cast<float>(slices) - seg) / static_cast<float>(slices),
																	 1.0f - (static_cast<float>(ring + 1) / static_cast<float>(stacks)))); 
		}
	}	

	// Draw as a triangle strip
	GLuint sphereDrawList = glGenLists(1);
	assert(sphereDrawList != 0);
	glNewList(sphereDrawList, GL_COMPILE);

	glBegin(GL_TRIANGLE_STRIP);

	for (unsigned int i = 0; i < vertexCount; i++) {
		glTexCoord2f(texCoords[i][0], texCoords[i][1]);
		glNormal3f(normals[i][0], normals[i][1], normals[i][2]);
		glVertex3f(vertices[i][0], vertices[i][1], vertices[i][2]);
	}

	glEnd();
	glEndList();
	return sphereDrawList;
}