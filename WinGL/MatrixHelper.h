#ifndef _MATRIX_HELPER_H_
#define _MATRIX_HELPER_H_

// crt includes
#include <math.h>

// local includes
#include "Matrix.h"
#include "Vector.h"

namespace MatrixHelper
{

template < typename T >
void DecomposeYawPitchRoll( const Matrix< T > & mat,
                            T * y, T * p, T * r )
{
   // only decomposes matrices that are
   // orthongonal and have been applied
   // in the yaw, pitch, and roll order
   if (y) *y = atan2(-mat.mT[2], mat.mT[10]);
   if (p) *p = asin(mat.mT[6]);
   if (r) *r = atan2(-mat.mT[4], mat.mT[5]);
}

template < typename T >
void DecomposeYawPitchRollDeg( const Matrix< T > & mat,
                               T * y, T * p, T * r )
{
   // only decomposes matrices that are
   // orthongonal and have been applied
   // in the yaw, pitch, and roll order
   DecomposeYawPitchRoll(mat, y, p, r);

   if (y) *y *= (180.0 / M_PI);
   if (p) *p *= (180.0 / M_PI);
   if (r) *r *= (180.0 / M_PI);
}

template < typename T >
Vector< T > GetStrafeVector( const Matrix< T > & mat )
{
   Vector< T > v;

   v.mT[0] = mat.mT[0];
   v.mT[1] = mat.mT[4];
   v.mT[2] = mat.mT[8];

   return v;
}

template < typename T >
Vector< T > GetViewVector( const Matrix< T > & mat )
{
   Vector< T > v;

   v.mT[0] = mat.mT[2];
   v.mT[1] = mat.mT[6];
   v.mT[2] = mat.mT[10];

   v *= -1;

   return v;
}

} // namespace MatrixHelper

#endif // _MATRIX_HELPER_H_