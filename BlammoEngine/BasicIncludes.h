// cross-compatibility issue
#ifdef WIN32
#include <windows.h>
#endif
// -------------------------

// C Library includes
#include <cstdlib>
#include <float.h>
#include <time.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <cmath>

// STL includes
#include <algorithm>
#include <vector>
#include <list>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ostream>

// OpenGL includes
#define GLEW_STATIC
#include "GL/glew.h"
#include "GL/gl.h"
#include "GL/glu.h"
#include "GL/glut.h"

// SDL Includes
#ifdef WIN32
#pragma comment(lib, "SDL.lib")
#pragma comment(lib, "SDLmain.lib")
#endif
#include "SDL.h"

// Cg includes
#include "Cg/cg.h"
#include "Cg/cgGL.h"

// DevIL includes ... uses a dll (cross-compatibility issue)
#include "IL/il.h"
#include "IL/ilu.h"
#include "IL/ilut.h"

// Freetype library for loading free type fonts
#define FREETYPE2_STATIC

#ifdef WIN32
#define generic __identifier(generic)
#endif

#include "ft2build.h"
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_OUTLINE_H
#include FT_TRIGONOMETRY_H

// Pseudo Random Number Generator (PRNG)
#include "mtrand.h"

// Custom Debug functionality
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef  NDEBUG
#define debug_output(s) ((void)0)
#else
#define debug_output(s) std::cout << s << std::endl
#endif

#endif