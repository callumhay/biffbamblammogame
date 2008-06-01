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
#define FREETYPE2_STATIC
#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

// Custom Debug functionality
#include "Debug.h"