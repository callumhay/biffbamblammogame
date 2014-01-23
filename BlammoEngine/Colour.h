/**
 * Colour.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
 */

#ifndef __COLOUR_H___
#define __COLOUR_H___

#include "BasicIncludes.h"
#include "Algebra.h"
#include "Vector.h"

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
    explicit Colour(int hexColour) {
        colours[0] = static_cast<float>((hexColour & 0xFF0000) >> 16) / 255.0f;
        colours[1] = static_cast<float>((hexColour & 0x00FF00) >>  8) / 255.0f;
        colours[2] = static_cast<float>((hexColour & 0x0000FF)) / 255.0f;
    }
    explicit Colour(const Vector3D& vec) {
        colours[0] = std::min<float>(1.0f, std::max<float>(0.0f, vec[0]));
        colours[1] = std::min<float>(1.0f, std::max<float>(0.0f, vec[1]));
        colours[2] = std::min<float>(1.0f, std::max<float>(0.0f, vec[2]));
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

    const float* begin() const {
        return colours;
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

    float GetLuminance() const {
        return this->R() * 0.3f + this->G() * 0.59f + this->B() * 0.11f;
    }

    float GetGreyscaleAmt() const {
        return (this->R() + this->G() + this->B()) / 3.0f;
    }

    Colour GetComplementaryColour() const {
        return Colour(1.0f - this->colours[0], 1.0f - this->colours[1], 1.0f - this->colours[2]);
    }

    Vector3D GetAsVector3D() const {
        return Vector3D(this->colours[0], this->colours[1], this->colours[2]);
    }

    const float* GetColourValues() const {
        return colours;
    }
};

class ColourRGBA : public Colour {
private:
	float alpha;

public:
	ColourRGBA() : Colour(), alpha(1.0f) {}
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

	float& operator[](size_t idx) {
		assert(idx < 4);
		if (idx == 3) {
			return this->alpha;
		}

    return this->colours[ idx ];
  }

  float operator[](size_t idx) const {
		assert(idx < 4);
		if (idx == 3) {
			return this->alpha;
		}

    return this->colours[ idx ];
  }

	float A() const {
		return this->alpha;
	}

	const Colour& GetColour() const {
		return (*this);
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

inline bool operator ==(const Colour& a, const Colour& b) {
	return (abs(a.R() - b.R()) < EPSILON) && (abs(a.G() - b.G()) < EPSILON) && (abs(a.B() - b.B()) < EPSILON);
};
inline bool operator !=(const Colour& a, const Colour& b) {
	return !(a == b);
};

inline std::ostream& operator <<(std::ostream& os, const Colour& c) {
  return os << "c<" << c.R() << "," << c.G() << "," << c.B() << ">";
};

// Multiplicative colour based off a scalar
inline ColourRGBA operator *(float s, const ColourRGBA& a) {
	assert(s >= 0);
	return ColourRGBA(s*a.R(), s*a.G(), s*a.B(), s*a.A());
};
inline ColourRGBA operator /(const ColourRGBA& a, float b) {
	return ColourRGBA(a.R()/b, a.G()/b, a.B()/b, a.A()/b);
}

// Multiplicative colouring
inline ColourRGBA operator *(const ColourRGBA& a, const ColourRGBA& b) {
	return ColourRGBA(a.R()*b.R(), a.G()*b.G(), a.B()*b.B(), a.A()*b.A());
}
inline ColourRGBA operator *(const Colour& a, const ColourRGBA& b) {
	return ColourRGBA(a.R()*b.R(), a.G()*b.G(), a.B()*b.B(), b.A());
}

// Additive colouring
inline ColourRGBA operator +(const ColourRGBA& a, const ColourRGBA& b) {
	return ColourRGBA(a.R()+b.R(), a.G()+b.G(), a.B()+b.B(), a.A()+b.A());
};

// Subtractive colouring
inline ColourRGBA operator -(const ColourRGBA& a, const ColourRGBA& b) {
	return ColourRGBA(a.R()-b.R(), a.G()-b.G(), a.B()-b.B(), a.A()-b.A());
};

inline std::ostream& operator <<(std::ostream& os, const ColourRGBA& c) {
  return os << "c<" << c.R() << "," << c.G() << "," << c.B() << "," << c.A() << ">";
};

struct Colour4D {
	float rgba[4];
};
#endif