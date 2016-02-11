/*
 * vec.h
 *      Vector maths library. All the meat is 2D, but 3D and 4D types are
 *      defined for convenience.
 */
#pragma once
#include <math.h>

#define PI 3.14159265358979323846

/* For use as indices into vec2_t */
#define X 0
#define Y 1

typedef float vec_t;
typedef vec_t vec2_t[2];
typedef vec_t vec3_t[3];
typedef vec_t vec4_t[4];

#define VDot(v1, v2)            (v1[X]*v2[X]+v1[Y]*v2[Y])
#define VAdd(vout, v1, v2)      (vout[X]=v1[X]+v2[X],vout[Y]=v1[Y]+v2[Y])
#define VSub(vout, v1, v2)      (vout[X]=v1[X]-v2[X],vout[Y]=v1[Y]-v2[Y])
#define VScale(vout, v, s)      (vout[X]=v[X]*s,vout[Y]=v[Y]*s)
#define VCopy(vout, v)          (vout[X]=v[X],vout[Y]=v[Y])
#define VNeg(vout, v)           (vout[X]=-v[X],vout[Y]=-v[Y])
#define VSet(v, x, y)           (v[X]=(x),v[Y]=(y))
#define VLen(v)                 sqrt(v[X]*v[X]+v[Y]*v[Y])
#define VLenSq(v)               (v[X]*v[X]+v[Y]*v[Y])

static inline void VNorm(vec2_t v)
{
        vec_t rlen = 1/VLen(v);

        v[X] *= rlen;
        v[Y] *= rlen;
}

/*
 * VParseStr
 *      Parse a vector out of the given string. Expects it to be in the
 *      format (X Y) and returns EFAIL if it isn't.
 */
ecode_t VParseStr(const char *str, vec2_t out);

void VecTests();
