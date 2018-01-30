#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "Math.h"

union Matrix
{
	struct
	{
		float _11, _12, _13, _14;
		float _21, _22, _23, _24;
		float _31, _32, _33, _34;
		float _41, _42, _43, _44;
	};
	float d[4][4];
};

void matrix_identity(Matrix* m)
{
	m->_11 = m->_22 = m->_33 = m->_44 = 1.0f;
	m->_12 = m->_13 = m->_14 = m->_21 = m->_23 = m->_24 = m->_31 = m->_32 = m->_34 = m->_41 = m->_42 = m->_43 = 0.0f;
}

Vector3 matrix_transform_position(Matrix* m, Vector3 v)
{
	Vector3 out;
	//*
	float w = m->_14 * v.x + m->_24 * v.y + m->_34 * v.z + m->_44;
	out.x = (m->_11 * v.x + m->_21 * v.y + m->_31 * v.z + m->_41) / w;
	out.y = (m->_12 * v.x + m->_22 * v.y + m->_32 * v.z + m->_42) / w;
	out.z = (m->_13 * v.x + m->_23 * v.y + m->_33 * v.z + m->_43) / w;/*/
	float w = m->_41 * v.x + m->_42 * v.y + m->_43 * v.z + m->_44;
	out.x = (m->_11 * v.x + m->_12 * v.y + m->_13 * v.z + m->_14) / w;
	out.y = (m->_21 * v.x + m->_22 * v.y + m->_23 * v.z + m->_24) / w;
	out.z = (m->_31 * v.x + m->_32 * v.y + m->_33 * v.z + m->_34) / w;//*/
	return out;
}

void matrix_perspective(Matrix* m, float fovy, float aspect, float zn, float zf)
{
	//*
	m->_12 = m->_13 = m->_14 = m->_21 = m->_23 = m->_24 = m->_31 = m->_32 = m->_41 = m->_42 = m->_44 = 0.0f;
	m->_34 = 1.0f;

	m->_22 = 1.0f / tanf(fovy / 2);
	m->_11 = m->_22 / aspect;
	m->_33 = zf / (zf - zn);
	m->_43 = -zn * m->_33;/*/

	m->_12 = m->_13 = m->_14 = m->_21 = m->_23 = m->_24 = m->_31 = m->_32 = m->_41 = m->_42 = m->_44 = 0.0f;
	m->_43 = 1.0f;

	m->_22 = 1.0f / tan(fovy / 2);
	m->_11 = m->_22 / aspect;
	m->_33 = zf / (zf - zn);
	m->_34 = zn * m->_33;//*/
}

#endif
