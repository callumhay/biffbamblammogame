/**
 * Colour.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use of the Biff! Bam!! Blammo!?! code or any derivative
 * works are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
 * 4. Redistributions may not be sold, nor may they be used in a commercial
 * product or activity without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL CALLUM HAY BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
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
        colours[0] = NumberFuncs::Clamp(vec[0], 0, 1);
        colours[1] = NumberFuncs::Clamp(vec[1], 0, 1);
        colours[2] = NumberFuncs::Clamp(vec[2], 0, 1);
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

    Colour& operator/=(float f) {
        assert(f > 0);
        this->colours[0] /= f;
        this->colours[1] /= f;
        this->colours[2] /= f;
        return *this;
    }
    Colour& operator+=(const Colour& c) {
        this->colours[0] = std::min<float>(1, c[0] + this->colours[0]);
        this->colours[1] = std::min<float>(1, c[1] + this->colours[1]);
        this->colours[2] = std::min<float>(1, c[2] + this->colours[2]);
        return *this;
    }
};

class ColourRGBA : public Colour {
private:
	float alpha;

public:
	ColourRGBA() : Colour(), alpha(1.0f) {}
    ColourRGBA(float r, float g, float b, float a): Colour(r,g,b), alpha(a) {}
	ColourRGBA(const Colour &c, float a) : Colour(c), alpha(a) {}
    ColourRGBA(const Vector4D& v) {
        this->colours[0] = NumberFuncs::Clamp(v[0], 0.0f, 1.0f);
        this->colours[1] = NumberFuncs::Clamp(v[1], 0.0f, 1.0f);
        this->colours[2] = NumberFuncs::Clamp(v[2], 0.0f, 1.0f);
        this->alpha      = NumberFuncs::Clamp(v[3], 0.0f, 1.0f);
    }
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
    void SetColour(const Colour& colour) {
        (*static_cast<Colour*>(this)) = colour;
    }

    Vector4D GetAsVector4D() const {
        return Vector4D(this->R(), this->G(), this->B(), this->A());
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
inline ColourRGBA operator *(const ColourRGBA& a, const Colour& b) {
    return ColourRGBA(a.R()*b.R(), a.G()*b.G(), a.B()*b.B(), a.A());
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