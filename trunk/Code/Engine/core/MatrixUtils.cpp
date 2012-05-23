#include "core\MatrixUtils.h"
#include "core\Vector.h"
#include "core\Matrix.h"
#include "core\Quaternion.h"
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::lerp( const Matrix& start, const Matrix& end, float lerpDist, Matrix& outInterpolatedMtx )
{
   ASSERT_MSG( lerpDist <= 1, "The value for advancement should be <= 1");
   ASSERT_MSG( lerpDist >= 0, "The value for advancement should be >= 0");

   // translation

   Vector startPos( start.m[3][0], start.m[3][1], start.m[3][2] );
   Vector endPos( end.m[3][0], end.m[3][1], end.m[3][2] );
   Vector resultPos;
   resultPos.setSub( endPos, startPos ).mul( lerpDist ).add( startPos );

   Matrix translationMtx;
   translationMtx.setTranslation( resultPos );

   // rotation
   Quaternion startQuat, endQuat;
   start.getRotation( startQuat );
   end.getRotation( endQuat );

   Quaternion resultQuat;
   resultQuat.setSlerp( startQuat, endQuat, lerpDist );

   Matrix rotationMtx;
   rotationMtx.setRotation( resultQuat );

   // calculate the final result
   outInterpolatedMtx.setMul( rotationMtx, translationMtx );
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generatePrespectiveProjection( float fov, float aspectRatio, float nearZPlane, float farZPlane, Matrix& outProjMtx )
{
   float yScale = 1.0f / tan( fov / 2.0f );
   float xScale = yScale / aspectRatio;

   memset( (float*)(outProjMtx.m), 0, sizeof( float ) * 16 );
   outProjMtx.m11 = xScale;
   outProjMtx.m22 = yScale;
   outProjMtx.m33 = farZPlane / ( farZPlane - nearZPlane );
   outProjMtx.m34 = 1.0f;
   outProjMtx.m43 = -( nearZPlane * farZPlane ) / ( farZPlane - nearZPlane );
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generateOrthogonalProjection( float viewportWidth, float viewportHeight, float nearZPlane, float farZPlane, Matrix& outProjMtx )
{
   memset( (float*)(outProjMtx.m), 0, sizeof( float ) * 16 );
   outProjMtx.m11 = 2.0f / viewportWidth;
   outProjMtx.m22 = 2.0f / viewportHeight;
   outProjMtx.m33 = 1.0f / ( farZPlane - nearZPlane );
   outProjMtx.m43 = -nearZPlane / ( farZPlane - nearZPlane );
   outProjMtx.m44 = 1.0f;
}

///////////////////////////////////////////////////////////////////////////////

void MatrixUtils::generateLookAtLH( const Vector& cameraOriginPos, const Vector& lookAtPos, const Vector& upAxis, Matrix& outLookAtMtx )
{
   Vector localForwardAxis, localSideAxis, localUpAxis;
   localForwardAxis.setSub( lookAtPos, cameraOriginPos ).normalize();
   localSideAxis.setCross( upAxis, localForwardAxis ).normalize();
   localUpAxis.setCross( localForwardAxis, localSideAxis );

   outLookAtMtx.m11 = localSideAxis.x; outLookAtMtx.m12 = localUpAxis.x; outLookAtMtx.m13 = localForwardAxis.x; outLookAtMtx.m14 = 0.0;
   outLookAtMtx.m21 = localSideAxis.y; outLookAtMtx.m22 = localUpAxis.y; outLookAtMtx.m23 = localForwardAxis.y; outLookAtMtx.m24 = 0.0;
   outLookAtMtx.m31 = localSideAxis.z; outLookAtMtx.m32 = localUpAxis.z; outLookAtMtx.m33 = localForwardAxis.z; outLookAtMtx.m34 = 0.0;

   outLookAtMtx.m41 = -localSideAxis.dot( cameraOriginPos );
   outLookAtMtx.m42 = -localUpAxis.dot( cameraOriginPos );
   outLookAtMtx.m43 = -localForwardAxis.dot( cameraOriginPos );
   outLookAtMtx.m44 = 1.0;
}

///////////////////////////////////////////////////////////////////////////////
