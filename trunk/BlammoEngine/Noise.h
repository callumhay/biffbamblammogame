/**
 * Noise.h
 *
 * (cc) Creative Commons Attribution-Noncommercial 3.0 License
 * Callum Hay, 2011
 *
 * You may not use this work for commercial purposes.
 * If you alter, transform, or build upon this work, you may distribute the 
 * resulting work only under the same or similar license to this one.
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