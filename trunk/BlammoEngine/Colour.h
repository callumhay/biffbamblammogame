#ifndef __COLOUR_H___
#define __COLOUR_H___

#include "BasicIncludes.h"
#include "Algebra.h"

class Colour {
protected:
  float colours[3];

public:
	Colour() {
		colours[0] = colours[1] = colours[2] = 1;
	}
  Colour(float r, float g, float b) {
    colours[0] = r;
    colours[1] = g;
    colours[2] = b;
  }
  Colour(const Colour& other) {
    colours[0] = other.colours[0];
    colours[1] = other.colours[1];
    colours[2] = other.colours[2];
  }

  Colour& operator =(const Colour& other) {
    colours[0] = other.colours[0];
    colours[1] = other.colours[1];
    colours[2] = other.colours[2];
    return *this;
  }
	
	float& operator[](size_t idx) {
		assert(idx < 3);
    return this->colours[ idx ];
  }

  float operator[](size_t idx) const {
		assert(idx < 3);
    return this->colours[ idx ];
  }

  float R() const { 
    return colours[0];
  }
  float G() const { 
    return colours[1];
  }
  float B() const { 
    return colours[2];
  }

  const float* getColourVector() const {
    return colours;
  }
};

class ColourRGBA : public Colour {
private:
	float alpha;

public:
	ColourRGBA(float r, float g, float b, float a): Colour(r,g,b), alpha(a) {}
	ColourRGBA(const Colour &c, float a) : Colour(c), alpha(a) {}
	ColourRGBA(const ColourRGBA& other) : alpha(other.alpha) {
	  this->colours[0] = other.colours[0];
    this->colours[1] = other.colours[1];
    this->colours[2] = other.colours[2];	
	}
  
	ColourRGBA& operator =(const ColourRGBA& other) {
	  this->colours[0] = other.colours[0];
    this->colours[1] = other.colours[1];
    this->colours[2] = other.colours[2];	
		this->alpha   = other.alpha;
    return *this;
  }

	float A() const {
		return this->alpha;
	}
};

// Multiplicative colour based off a scalar
inline Colour operator *(float s, const Colour& a) {
	assert(s >= 0);
	return Colour(s*a.R(), s*a.G(), s*a.B());
};
inline Colour operator /(const Colour& a, float b) {
  return Colour(a.R()/b, a.G()/b, a.B()/b);
}

// Multiplicative colouring
inline Colour operator *(const Colour& a, const Colour& b) {
  return Colour(a.R()*b.R(), a.G()*b.G(), a.B()*b.B());
}

// Additive colouring
inline Colour operator +(const Colour& a, const Colour& b) {
  return Colour(a.R()+b.R(), a.G()+b.G(), a.B()+b.B());
};

// Subtractive colouring
inline Colour operator -(const Colour& a, const Colour& b) {
  return Colour(a.R()-b.R(), a.G()-b.G(), a.B()-b.B());
};

inline std::ostream& operator <<(std::ostream& os, const Colour& c) {
  return os << "c<" << c.R() << "," << c.G() << "," << c.B() << ">";
};

struct Colour4D {
	float rgba[4];
};
#endif