// cross-compatibility issue
#include <windows.h>
// -------------------------

// Library includes
#include <float.h>
#include <time.h>

// OpenGL includes
#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"

// Cg includes
#include "Cg/cg.h"
#include "Cg/cgGL.h"

// DevIL includes ... uses a dll (cross-compatibility issue)
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

// Freetype library for loading free type fonts
#include "ft2build.h"
#include FT_FREETYPE_H

// Custom Debug functionality
#include "Debug.h"