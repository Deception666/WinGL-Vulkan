#ifndef _CAMERA_POLICY_ROAM_NO_ROLL_RESTRICT_PITCH_H_
#define _CAMERA_POLICY_ROAM_NO_ROLL_RESTRICT_PITCH_H_

// local includes
#include "../Matrix.h"
#include "../Vector3.h"

// std includes
#include <ratio>
#include <cstdint>

namespace camera_policy
{

// todo: think about moving the up vector templates to their own header
// todo: need to add the other basis vectors too, as they will describe the origin
template < typename T >
struct up_vector_y_axis
{
   static Vector3< T > up( ) { return Vector3< T >(0, 1, 0); }
};

template < typename T, typename U = up_vector_y_axis< T > >
struct up_vector
{
   static const Vector3< T > up;
};

template < typename T, typename U >
const Vector3< T > up_vector< T, U >::up = U::up();

template < typename T, typename MIN_PITCH_RATIO, typename MAX_PITCH_RATIO, typename UP_VECTOR = up_vector< T > >
class RoamNoRollRestrictPitch
{
public:
   // public typedefs
   typedef T               type;
   typedef Matrix< T >     mat_type;
   typedef Vector3< T >    vec_type;

   // constructor / destructor
    RoamNoRollRestrictPitch( );
    RoamNoRollRestrictPitch( const RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR > & policy );
   ~RoamNoRollRestrictPitch( );

   // copy operator
   RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR > &
   operator = ( const RoamNoRollRestrictPitch< T, MIN_PITCH_RATIO, MAX_PITCH_RATIO, UP_VECTOR > & policy );

   // initializes the camera at a specific location
   void LookAt( const vec_type & eye, const vec_type & center );

   // moves the camera by a set amount along the camera axes
   void TranslateRight( const type amount );
   void TranslateForward( const type amount );
   void TranslateUp( const type amount );

   // moves the camera by a set amount along the camera axes defined by direction
   void Translate( const type amount, const vec_type & direction );

   // rotates the camera
   void SetYaw( const type yaw_deg );
   void SetPitch( const type pitch_deg );
   void SetRoll( const type roll_deg );

   void RotateYaw( const type amount_deg );
   void RotatePitch( const type amount_deg );
   void RotateRoll( const type amount_deg );

   // obtains the current euler angles
   type GetYaw( ) const;
   type GetPitch( ) const;
   type GetRoll( ) const;

   // obtains the current euler angles and the pointers can be null
   void GetYPR( type * const yaw_deg, type * const pitch_deg, type * const roll_deg ) const;

   // returns the camera
   mat_type GetMatrix( ) const;

private:
   // helper function to do translations along the basis vectors
   template < uint32_t AXIS >
   void Translate( const type amount );

   // helper function to do the actual rotation
   // there is no roll, as roll does not influence this camera type
   void Rotate( const type yaw_deg, const type pitch_deg );

   // private constants
   const T     MIN_PITCH;
   const T     MAX_PITCH;

   // private members
   mat_type    mCamMatrix;

};

} // namespace camera_policy

// include the inline code
#include "RoamNoRollRestrictPitch.inl"

#endif // _CAMERA_POLICY_ROAM_NO_ROLL_RESTRICT_PITCH_H_
