#ifndef __COLOUR_H___
#define __COLOUR_H___

#include <iostream>
#include <assert.h>

class Colour {
private:
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
/*
// Multiplicative colour based off a scalar
inline Colour operator *(float s, const Colour& a) {
	assert(s >= 0);
  return Colour(std::min(1.0f, s*a.R()), std::min(1.0f, s*a.G()), std::min(1.0f, s*a.B()));
};

// Multiplicative colouring
inline Colour operator *(const Colour& a, const Colour& b) {
  return Colour(std::max(0.0f, std::min(1.0f, a.R()*b.R())), 
  							std::max(0.0f, std::min(1.0f, a.G()*b.G())), 
  							std::max(0.0f, std::min(1.0f, a.B()*b.B())));
}

// Additive colouring
inline Colour operator +(const Colour& a, const Colour& b) {
  return Colour(std::max(0.0f, std::min(1.0f, a.R()+b.R())), 
  							std::max(0.0f, std::min(1.0f, a.G()+b.G())), 
  							std::max(0.0f, std::min(1.0f, a.B()+b.B())));
};

inline std::ostream& operator <<(std::ostream& os, const Colour& c) {
  return os << "c<" << c.R() << "," << c.G() << "," << c.B() << ">";
};
*/
#endif