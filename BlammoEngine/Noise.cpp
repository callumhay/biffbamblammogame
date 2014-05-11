/**
 * Noise.cpp
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

#include "Noise.h"
#include "Texture3D.h"

#define NOISE_SETUP(i, b0, b1, r0, r1)\
        t = vec[i] + N;\
        b0 = ((int)t) & BM;\
        b1 = (b0+1) & BM;\
        r0 = t - (int)t;\
        r1 = r0 - 1.;
#define NOISE_AT2(rx, ry) ( rx * q[0] + ry * q[1] )
#define NOISE_AT3(rx, ry, rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

Noise *Noise::instance = NULL;

int    Noise::p[MAXB + MAXB + 2];
double Noise::g3[MAXB + MAXB + 2][3];
double Noise::g2[MAXB + MAXB + 2][2];
double Noise::g1[MAXB + MAXB + 2];


Noise::Noise() : noise3DTexture(NULL) {
}

Noise::~Noise() {
	if (this->noise3DTexture != NULL) {
		delete this->noise3DTexture;
		this->noise3DTexture = NULL;
	}
}

Texture3D* Noise::GetNoise3DTexture() {
	if (this->noise3DTexture == NULL) {
		this->noise3DTexture = Texture3D::Create3DNoiseTexture(Noise3DTexSize);
	}
	return this->noise3DTexture;
}

void Noise::Normalize2(double v[2]) {
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
}

void Noise::Normalize3(double v[3]) {
	double s;

	s = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
	v[0] = v[0] / s;
	v[1] = v[1] / s;
	v[2] = v[2] / s;
}


void Noise::InitNoise() {
	
	int i, j, k;

	for (i = 0; i < this->B; i++) {
		Noise::p[i] = i;
		Noise::g1[i] = (double)((rand() % (this->B + this->B)) - this->B) / this->B;

		for (j = 0; j < 2; j++) {
			Noise::g2[i][j] = (double)((rand() % (this->B + this->B)) - this->B) / this->B;
		}
		this->Normalize2(Noise::g2[i]);

		for (j = 0; j < 3; j++) {
			Noise::g3[i][j] = (double)((rand() % (this->B + this->B)) - this->B) / this->B;
		}
		this->Normalize3(Noise::g3[i]);
	}

	while (--i) {
		k = Noise::p[i];
		Noise::p[i] = Noise::p[j = rand() % this->B];
		Noise::p[j] = k;
	}

	for (i = 0; i < this->B + 2; i++) {
		Noise::p[B + i] = Noise::p[i];
		Noise::g1[B + i] = Noise::g1[i];
		
		for (j = 0; j < 2; j++) {
			Noise::g2[B + i][j] = Noise::g2[i][j];
		}
		
		for (j = 0; j < 3; j++) {
			Noise::g3[B + i][j] = Noise::g3[i][j];
		}
	}
}

double Noise::Noise2(double vec[2]) {
	
	int bx0, bx1, by0, by1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, *q, sx, sy, a, b, t, u, v;
	int i, j;

	if (this->start) {
		this->start = false;
		this->InitNoise();
	}

	NOISE_SETUP(0, bx0, bx1, rx0, rx1);
	NOISE_SETUP(1, by0, by1, ry0, ry1);

	i = Noise::p[bx0];
	j = Noise::p[bx1];

	b00 = Noise::p[i + by0];
	b10 = Noise::p[j + by0];
	b01 = Noise::p[i + by1];
	b11 = Noise::p[j + by1];

	sx = this->SCurve(rx0);
	sy = this->SCurve(ry0);

	q = Noise::g2[b00]; u = NOISE_AT2(rx0, ry0);
	q = Noise::g2[b10]; v = NOISE_AT2(rx1, ry0);
	a = this->Lerp(sx, u, v);

	q = Noise::g2[b01]; u = NOISE_AT2(rx0, ry1);
	q = Noise::g2[b11]; v = NOISE_AT2(rx1, ry1);
	b = this->Lerp(sx, u, v);

	return this->Lerp(sy, a, b);
}

double Noise::Noise3(double vec[3]) {
	
	int bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	double rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	int i, j;

	if (this->start) {
		this->start = false;
		this->InitNoise();
	}

	NOISE_SETUP(0, bx0, bx1, rx0, rx1);
	NOISE_SETUP(1, by0, by1, ry0, ry1);
	NOISE_SETUP(2, bz0, bz1, rz0, rz1);

	i = Noise::p[bx0];
	j = Noise::p[bx1];

	b00 = Noise::p[i + by0];
	b10 = Noise::p[j + by0];
	b01 = Noise::p[i + by1];
	b11 = Noise::p[j + by1];

	t  = this->SCurve(rx0);
	sy = this->SCurve(ry0);
	sz = this->SCurve(rz0);

	q = Noise::g3[b00 + bz0]; u = NOISE_AT3(rx0, ry0, rz0);
	q = Noise::g3[b10 + bz0]; v = NOISE_AT3(rx1, ry0, rz0);
	a = this->Lerp(t, u, v);

	q = Noise::g3[b01 + bz0]; u = NOISE_AT3(rx0, ry1, rz0);
	q = Noise::g3[b11 + bz0]; v = NOISE_AT3(rx1, ry1, rz0);
	b = this->Lerp(t, u, v);

	c = this->Lerp(sy, a, b);

	q = Noise::g3[b00 + bz1]; u = NOISE_AT3(rx0, ry0, rz1);
	q = Noise::g3[b10 + bz1]; v = NOISE_AT3(rx1, ry0, rz1);
	a = this->Lerp(t, u, v);

	q = Noise::g3[b01 + bz1]; u = NOISE_AT3(rx0, ry1, rz1);
	q = Noise::g3[b11 + bz1]; v = NOISE_AT3(rx1, ry1, rz1);
	b = this->Lerp(t, u, v);

	d = this->Lerp(sy, a, b);

	return this->Lerp(sz, c, d);
}

double Noise::PerlinNoise2D(double x, double y, double alpha, double beta, int n) {
	
	int i;
	double val, sum = 0;
	double temp[2], scale = 1;

	temp[0] = x;
	temp[1] = y;
	
	for (i = 0; i < n; i++) {
		val = this->Noise2(temp);
		sum += val / scale;
		scale *= alpha;
		temp[0] *= beta;
		temp[1] *= beta;
	}

	return(sum);
}

double Noise::PerlinNoise3D(double x, double y, double z, double alpha, double beta, int n) {
	int i;
	double val,sum = 0;
	double temp[3],scale = 1;

	temp[0] = x;
	temp[1] = y;
	temp[2] = z;
	
	for (i = 0; i < n; i++) {
		val = this->Noise3(temp);
		sum += val / scale;
		scale *= alpha;
		temp[0] *= beta;
		temp[1] *= beta;
		temp[2] *= beta;
	}

	return(sum);
}

GLubyte* Noise::Make3DNoiseTexture(int size) {
	if (size <= 0) {
		return NULL;
	}

	int f, i, j, k, inc;
	int startFrequency = 4;
	int numOctaves = 4;
	double ni[3];
	double inci, incj, inck;
	int frequency = startFrequency;
	GLubyte* ptr;
	double amp = 0.5;

	GLubyte* noise3DTexPtr = new GLubyte[size * size * size * 4];
	for (f = 0, inc = 0; f < numOctaves; ++f, frequency *= 2, ++inc, amp *= 0.5)
	{
		SetNoiseFrequency(frequency);
		ptr = noise3DTexPtr;
		ni[0] = ni[1] = ni[2] = 0;

		inci = 1.0 / (size / frequency);
		for (i = 0; i < size; ++i, ni[0] += inci)
		{
			incj = 1.0 / (size / frequency);
			for (j = 0; j < size; ++j, ni[1] += incj)
			{
				inck = 1.0 / (size / frequency);
				for (k = 0; k < size; ++k, ni[2] += inck, ptr += 4)
					*(ptr + inc) = (GLubyte) (((this->Noise3(ni) + 1.0) * amp) * 128.0);
			}
		}
	}

	return noise3DTexPtr;
}

/*

void init3DNoiseTexture(int texSize, GLubyte* texPtr)
{
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, texSize, texSize, texSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, texPtr);
	free(texPtr);
}
*/