/**
 * Noise.h
 * 
 * Copyright (c) 2014, Callum Hay
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 * 3. The names of its contributors may not be used to endorse or promote products
 * derived from this software without specific prior written permission.
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

#ifndef __NOISE_H__
#define __NOISE_H__

#include "BasicIncludes.h"
#include "Algebra.h"

class Texture3D;

class Noise {

private:
	static Noise* instance;

	// Noise Texture
	Texture3D* noise3DTexture;
	static const int Noise3DTexSize = 64;

	static const int MAXB	= 0x100;
	static const int N		= 0x1000;
	static const int NP		= 12;
	static const int NM		= 0xfff;

	static int     p[MAXB + MAXB + 2];
	static double g3[MAXB + MAXB + 2][3];
	static double g2[MAXB + MAXB + 2][2];
	static double g1[MAXB + MAXB + 2];

	bool start;
	int B, BM;

	Noise();
	~Noise();

	void InitNoise();
	void Normalize2(double v[2]);
	void Normalize3(double v[3]);	
	void Noise::SetNoiseFrequency(int frequency) {
		this->start = true;
		this->B = frequency;
		this->BM = B-1;
	}

	double Noise2(double vec[2]);
	double Noise3(double vec[3]);

	static double SCurve(double t) {
		return t * t * (3.0 - 2.0 * t);
	}

	static double Lerp(double t, double a, double b) {
		return a + t * (b - a);
	}

public:
	static Noise* GetInstance() {
		if (Noise::instance == NULL) {
			Noise::instance = new Noise();
		}
		return Noise::instance;
	}

	static void DeleteInstance() {
		if (Noise::instance != NULL) {
			delete Noise::instance;
			Noise::instance = NULL;
		}
	}

	double PerlinNoise2D(double x, double y, double alpha, double beta, int n);
	double PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n);

	GLubyte* Noise::Make3DNoiseTexture(int size);
	Texture3D* GetNoise3DTexture();

};
#endif