#pragma once

#include "stdafx.h"

#include <random>
#include <complex>

using namespace std;
using namespace glm;

class Wave
{
public:
	// N, M:      Resolution
	// L_x, L_z:  Actual lengths of the grid (as in meters)
	// omega_hat: Direction of wind
	// V:         Speed of wind	
	Wave(int N, int M, float L_x, float L_z, vec2 omega, float V, float A, float lambda);
	~Wave();
	void buildField(float time);

	vec3* heightField;
	vec3* normalField;

private:
	complex<float> *value_h_twiddle_0 = NULL;
	complex<float> *value_h_twiddle_0_conj = NULL;
	complex<float> *value_h_twiddle = NULL;

	default_random_engine generator;
	normal_distribution<float> normal_dist;

	const float pi = float(M_PI);
	const float g = 9.8f; // Gravitational constant
	const float l = 0.1;
	float A;
	float V;
	int N, M;
	int kNum;
	vec2 omega_hat;
	float lambda;
	float L_x, L_z;	

	inline float func_omega(float k) const;
	inline float func_P_h(vec2 vec_k) const;
	inline complex<float> func_h_twiddle_0(vec2 vec_k);
	inline complex<float> func_h_twiddle(int kn, int km, float t) const;
};
