/**
 * GeometryMaker.cpp
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 Licence
 * Callum Hay, 2011
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
		glTexCoord2f(0.0f, 0.0f); glVertex2f(-0.5f, -0.5f);
		glTexCoord2f(1.0f, 0.0f); glVertex2f( 0.5f, -0.5f);
		glTexCoord2f(1.0f, 1.0f); glVertex2f( 0.5f,  0.5f);
		glTexCoord2f(0.0f, 1.0f); glVertex2f(-0.5f,  0.5f);
	glEnd();
	glEndList();

	return this->quadDL != 0;
}

void GeometryMaker::DrawRawCube(const Point3D& center, const Vector3D& size) const {
	Vector3D halfSize = 0.5f * size;

    glBegin(GL_QUADS);
    glNormal3i(0, 1, 0);
    glTexCoord2i(1, 1);
    glVertex3fv((center + Vector3D(halfSize[0], halfSize[1], -halfSize[2])).begin());	// Top Right Of The Quad (Top)
    glTexCoord2i(0, 1);
    glVertex3fv((center + Vector3D(-halfSize[0], halfSize[1], -halfSize[2])).begin());	// Top Left Of The Quad (Top)
    glTexCoord2i(0, 0);
    glVertex3fv((center + Vector3D(-halfSize[0], halfSize[1], halfSize[2])).begin());	// Bottom Left Of The Quad (Top)
    glTexCoord2i(1, 0);
    glVertex3fv((center + Vector3D(halfSize[0], halfSize[1], halfSize[2])).begin());	// Bottom Right Of The Quad (Top)

    glNormal3i(0, -1, 0);
    glTexCoord2i(1, 1);
    glVertex3fv((center + Vector3D(halfSize[0], -halfSize[1], halfSize[2])).begin());	// Top Right Of The Quad (Bottom)
    glTexCoord2i(0, 1);
    glVertex3fv((center + Vector3D(-halfSize[0], -halfSize[1], halfSize[2])).begin());	// Top Left Of The Quad (Bottom)
    glTexCoord2i(0, 0);
    glVertex3fv((center + Vector3D(-halfSize[0], -halfSize[1], -halfSize[2])).begin());	// Bottom Left Of The Quad (Bottom)
    glTexCoord2i(1, 0);
    glVertex3fv((center + Vector3D(halfSize[0], -halfSize[1], -halfSize[2])).begin());	// Bottom Right Of The Quad (Bottom)

    glNormal3i(0, 0, 1);
    glTexCoord2i(1, 1);
    glVertex3fv((center + Vector3D(halfSize[0], halfSize[1], halfSize[2])).begin());	// Top Right Of The Quad (Front)
    glTexCoord2i(0, 1);
    glVertex3fv((center + Vector3D(-halfSize[0], halfSize[1], halfSize[2])).begin());	// Top Left Of The Quad (Front)
    glTexCoord2i(0, 0);
    glVertex3fv((center + Vector3D(-halfSize[0], -halfSize[1], halfSize[2])).begin());	// Bottom Left Of The Quad (Front)
    glTexCoord2i(1, 0);
    glVertex3fv((center + Vector3D(halfSize[0], -halfSize[1], halfSize[2])).begin());	// Bottom Right Of The Quad (Front)

    glNormal3i(0, 0, -1);
    glTexCoord2i(1, 1);
    glVertex3fv((center + Vector3D(halfSize[0], -halfSize[1], -halfSize[2])).begin());	// Top Right Of The Quad (Back)
    glTexCoord2i(0, 1);
    glVertex3fv((center + Vector3D(-halfSize[0], -halfSize[1], -halfSize[2])).begin());	// Top Left Of The Quad (Back)
    glTexCoord2i(0, 0);
    glVertex3fv((center + Vector3D(-halfSize[0], halfSize[1], -halfSize[2])).begin());	// Bottom Left Of The Quad (Back)
    glTexCoord2i(1, 0);
    glVertex3fv((center + Vector3D(halfSize[0], halfSize[1], -halfSize[2])).begin());	// Bottom Right Of The Quad (Back)

    glNormal3i(-1, 0, 0);
    glTexCoord2i(1, 1);
    glVertex3fv((center + Vector3D(-halfSize[0], halfSize[1], halfSize[2])).begin());	// Top Right Of The Quad (Left)
    glTexCoord2i(0, 1);
    glVertex3fv((center + Vector3D(-halfSize[0], halfSize[1], -halfSize[2])).begin());	// Top Left Of The Quad (Left)
    glTexCoord2i(0, 0);
    glVertex3fv((center + Vector3D(-halfSize[0], -halfSize[1], -halfSize[2])).begin());	// Bottom Left Of The Quad (Left)
    glTexCoord2i(1, 0);
    glVertex3fv((center + Vector3D(-halfSize[0], -halfSize[1], halfSize[2])).begin());	// Bottom Right Of The Quad (Left)

    glNormal3i(1, 0, 0);
    glTexCoord2i(1, 1);
    glVertex3fv((center + Vector3D(halfSize[0], halfSize[1], -halfSize[2])).begin());	// Top Right Of The Quad (Right)
    glTexCoord2i(0, 1);
    glVertex3fv((center + Vector3D(halfSize[0], halfSize[1], halfSize[2])).begin());	// Top Left Of The Quad (Right)
    glTexCoord2i(0, 0);
    glVertex3fv((center + Vector3D(halfSize[0], -halfSize[1], halfSize[2])).begin());	// Bottom Left Of The Quad (Right)
    glTexCoord2i(1, 0);
    glVertex3fv((center + Vector3D(halfSize[0], -halfSize[1], -halfSize[2])).begin());	// Bottom Right Of The Quad (Right)

	glEnd();
}

/**
 * Initializes the display list for a simple, unit cube -
 * has no texture coordinates, but has normals set.
 * Returns: true on success, false otherwise.
 */
bool GeometryMaker::InitializeCubeDL() {
	
	this->cubeDL = glGenLists(1);
	glNewList(this->cubeDL, GL_COMPILE);
	this->DrawRawCube(Point3D(0,0,0), Vector3D(1,1,1));
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

const int GeometryMaker::TUNNEL_TESSELATION					= 10;
const float GeometryMaker::TUNNEL_WALL_SIZE					= 10.0f;
const float GeometryMaker::HALF_TUNNEL_WALL_SIZE		= GeometryMaker::TUNNEL_WALL_SIZE / 2.0f;

Vector3D GeometryMaker::TunnelOrientToVector(GeometryMaker::TunnelOrientation orientation) {
	switch (orientation) {
		case PosY:
			return Vector3D(0, 1, 0);
		case NegY:
			return Vector3D(0, -1, 0);
		case PosZ:
			return Vector3D(0, 0, 1);
		case NegZ:
			return Vector3D(0, 0, -1);
		case PosX:
			return Vector3D(1, 0, 0);
		case NegX:
			return Vector3D(-1, 0, 0);
		default:
			assert(false);
			break;
	}
	return Vector3D(0,0,0);
}

GeometryMaker::TunnelOrientation GeometryMaker::VectorToTunnelOrient(const Vector3D& vec) {
	if (vec == Vector3D(0, 1, 0)) {
		return PosY;
	}
	else if (vec == Vector3D(0, -1, 0)) {
		return NegY;
	}
	else if (vec == Vector3D(0, 0, 1)) {
		return PosZ;
	}
	else if (vec == Vector3D(0, 0, -1)) {
		return NegZ;
	}
	else if (vec == Vector3D(1, 0, 0)) {
		return PosX;
	}
	else if (vec == Vector3D(-1, 0, 0)) {
		return NegX;
	}
	else {
		assert(false);
	}
	return NegX;
}

/**
 * Draw a tesselated quad representing a square section of tunnel wall.
 */
void GeometryMaker::DrawTunnelWallSection(const Point3D& center, GeometryMaker::TunnelOrientation orientation) {
	assert(GeometryMaker::TUNNEL_TESSELATION > 0);
	static const float QUAD_SIZE = GeometryMaker::TUNNEL_WALL_SIZE / static_cast<float>(GeometryMaker::TUNNEL_TESSELATION);

	glPushMatrix();

	// Move the grid to its proper, centered location
	glTranslatef(center[0], center[1], center[2]);
	
	// Rotate into the proper orientation
	switch (orientation) {
		case PosY:
			glRotatef(-90, 1, 0, 0);
			break;
		case NegY:
			glRotatef(90, 1, 0, 0);
			break;
		case PosZ:
			break;
		case NegZ:
			glRotatef(180, 1, 0, 0);
			break;
		case PosX:
			glRotatef(90, 0, 1, 0);
			break;
		case NegX:
			glRotatef(-90, 0, 1, 0);
			break;
		default:
			assert(false);
			break;
	}	

	// Move the grid so it gets centered at the origin
	glTranslatef(GeometryMaker::HALF_TUNNEL_WALL_SIZE, GeometryMaker::HALF_TUNNEL_WALL_SIZE, 0.0f);
	glBegin(GL_QUADS);

	// Create the grid of quads from (0,0) to (GeometryMaker::TUNNEL_WALL_SIZE, GeometryMaker::TUNNEL_WALL_SIZE)
	glNormal3i(0, 0, 1);
	for (int row = 0; row < GeometryMaker::TUNNEL_TESSELATION; row++) {
		for (int col = 0; col < GeometryMaker::TUNNEL_TESSELATION; col++) {
			glVertex2f(row*QUAD_SIZE, col*QUAD_SIZE);
			glVertex2f(row*QUAD_SIZE, (col+1)*QUAD_SIZE);
			glVertex2f((row+1)*QUAD_SIZE, (col+1)*QUAD_SIZE);
			glVertex2f((row+1)*QUAD_SIZE, col*QUAD_SIZE);
		}
	}

	glEnd();
	glPopMatrix();
}

void GeometryMaker::DrawFullTunnel(const Point3D& startCenterPt, int units, GeometryMaker::TunnelOrientation dir) {
	assert(units > 0);

	Vector3D dirUnitVec = TunnelOrientToVector(dir);
	Vector3D upUnitVec(0,0,0);
	Vector3D sideUnitVec(0,0,0);

	switch (dir) {
		case PosY:
			dirUnitVec  = Vector3D(0, 1, 0);
			upUnitVec   = Vector3D(1, 0, 0);
			sideUnitVec = Vector3D(0, 0, 1);
			break;
		case NegY:
			dirUnitVec  = Vector3D(0, -1, 0);
			upUnitVec   = Vector3D(1, 0, 0);
			sideUnitVec = Vector3D(0, 0, -1);
			break;
		case PosZ:
			dirUnitVec  = Vector3D(0, 0, 1);
			upUnitVec   = Vector3D(0, 1, 0);
			sideUnitVec = Vector3D(1, 0, 0);
			break;
		case NegZ:
			dirUnitVec  = Vector3D(0, 0, -1);
			upUnitVec   = Vector3D(0, 1, 0);
			sideUnitVec = Vector3D(-1, 0, 0);
			break;
		case PosX:
			dirUnitVec  = Vector3D(1, 0, 0);
			upUnitVec   = Vector3D(0, 0, 1);
			sideUnitVec = Vector3D(0, 1, 0);
			break;
		case NegX:
			dirUnitVec = Vector3D(-1, 0, 0);
			upUnitVec   = Vector3D(0, 0, 1);
			sideUnitVec = Vector3D(0, -1, 0);
			break;
		default:
			assert(false);
			break;
	}

	// Build a straight, full section of tunnel using the tunnel walls for each piece...
	// Four sides...

	const Vector3D HALF_INCREMENT_AMT = GeometryMaker::HALF_TUNNEL_WALL_SIZE * dirUnitVec;
	const Vector3D INCREMENT_AMT			= GeometryMaker::TUNNEL_WALL_SIZE * dirUnitVec;

	// Top
	Point3D currentTopPt = startCenterPt + upUnitVec + HALF_INCREMENT_AMT;
	TunnelOrientation inwardsFromTopOrient = this->VectorToTunnelOrient(-upUnitVec);
	// Bottom
	Point3D currentBottomPt = startCenterPt - upUnitVec + HALF_INCREMENT_AMT;
	TunnelOrientation inwardsFromBottomOrient = this->VectorToTunnelOrient(upUnitVec);
	// Side1
	Point3D currentSide1Pt = startCenterPt + sideUnitVec + HALF_INCREMENT_AMT;
	TunnelOrientation inwardsFromSide1Orient = this->VectorToTunnelOrient(-sideUnitVec);
	// Side2
	Point3D currentSide2Pt = startCenterPt - sideUnitVec + HALF_INCREMENT_AMT;
	TunnelOrientation inwardsFromSide2Orient = this->VectorToTunnelOrient(sideUnitVec);

	int count = 1;
	for (;;) {
		this->DrawTunnelWallSection(currentTopPt,    inwardsFromTopOrient);
		this->DrawTunnelWallSection(currentBottomPt, inwardsFromBottomOrient);
		this->DrawTunnelWallSection(currentSide1Pt,  inwardsFromSide1Orient);
		this->DrawTunnelWallSection(currentSide2Pt,  inwardsFromSide2Orient);

		if (count >= units) {
			break;
		}

		currentTopPt    = currentTopPt    + INCREMENT_AMT;
		currentBottomPt = currentBottomPt + INCREMENT_AMT;
		currentSide1Pt  = currentSide1Pt  + INCREMENT_AMT;
		currentSide2Pt  = currentSide2Pt  + INCREMENT_AMT;
		count++;
	}

}