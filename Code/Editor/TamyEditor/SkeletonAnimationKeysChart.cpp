#include "SkeletonAnimationKeysChart.h"
#include "core-AI/SkeletonAnimation.h"
#include "core-AI/BoneSRTAnimation.h"
#include "ChartLine.h"
#include <QPainter>
#include <math.h>
#include "core/Algorithms.h"
#include "core/EulerAngles.h"
#include "AnimChartScales.h"


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class PtsProvider : public ChartLinePointsProvider
   {
   private:
      BoneSRTAnimation&                   m_boneAnimation;
      unsigned int                        m_keyIdx;

   public:
      PtsProvider( BoneSRTAnimation& boneAnimation, unsigned int keyIdx )
         : m_boneAnimation( boneAnimation )
         , m_keyIdx( keyIdx )
      {}

      void getPoints( QPointF*& outPoints, unsigned int& outCount ) const
      {
         if ( m_keyIdx == BAKEY_POS_X || m_keyIdx == BAKEY_POS_Y || m_keyIdx == BAKEY_POS_Z )
         {
            // translation keys
            outCount = m_boneAnimation.getTranslationKeysCount();
            if ( outCount == 0 )
            {
               return;
            }

            outPoints = new QPointF[ outCount ];
            Vector pos;
            float time;
            for ( unsigned int i = 0; i < outCount; ++i )
            {
               m_boneAnimation.getTranslationKey( i, pos, time );
               outPoints[i].setX( time * ANIMATION_TIME_SCALE );

               switch( m_keyIdx )
               {
               case BAKEY_POS_X: { outPoints[i].setY( pos[0] * ANIMATION_VALUE_SCALE ); break; }
               case BAKEY_POS_Y: { outPoints[i].setY( pos[1] * ANIMATION_VALUE_SCALE ); break; }
               case BAKEY_POS_Z: { outPoints[i].setY( pos[2] * ANIMATION_VALUE_SCALE ); break; }
               }
            }
         }
         else
         {
            // orientation keys
            outCount = m_boneAnimation.getOrientationKeysCount();
            if ( outCount == 0 )
            {
               return;
            }

            outPoints = new QPointF[ outCount ];
            Quaternion orient;
            float time;
            for ( unsigned int i = 0; i < outCount; ++i )
            {
               m_boneAnimation.getOrientationKey( i, orient, time );
               outPoints[i].setX( time * ANIMATION_TIME_SCALE );

               EulerAngles angle;
               angle.setFromQuaternion( orient );
               switch( m_keyIdx )
               {
               case BAKEY_YAW: { outPoints[i].setY( angle.getYaw() ); break; }
               case BAKEY_PITCH: { outPoints[i].setY( angle.getPitch() ); break; }
               case BAKEY_ROLL: { outPoints[i].setY( angle.getRoll() ); break; }
               }
            }
         }
      }

      QPointF getPoint( unsigned int pointIdx ) const
      {
         QPointF result;

         if ( m_keyIdx == BAKEY_POS_X || m_keyIdx == BAKEY_POS_Y || m_keyIdx == BAKEY_POS_Z )
         {
            // translation keys
            unsigned int count = m_boneAnimation.getTranslationKeysCount();
            if ( pointIdx < count )
            {
               Vector pos;
               float time;
               m_boneAnimation.getTranslationKey( pointIdx, pos, time );
               result.setX( time * ANIMATION_TIME_SCALE );

               switch( m_keyIdx )
               {
               case BAKEY_POS_X: { result.setY( pos[0] * ANIMATION_VALUE_SCALE ); break; }
               case BAKEY_POS_Y: { result.setY( pos[1] * ANIMATION_VALUE_SCALE ); break; }
               case BAKEY_POS_Z: { result.setY( pos[2] * ANIMATION_VALUE_SCALE ); break; }
               }
            }
         }
         else
         {
            // orientation keys
            unsigned int count = m_boneAnimation.getOrientationKeysCount();
            if ( pointIdx < count )
            {
               Quaternion orient;
               float time;
               m_boneAnimation.getOrientationKey( pointIdx, orient, time );
               result.setX( time * ANIMATION_TIME_SCALE );

               EulerAngles angle;
               angle.setFromQuaternion( orient );
               switch( m_keyIdx )
               {
               case BAKEY_YAW: { result.setY( angle.getYaw() ); break; }
               case BAKEY_PITCH: { result.setY( angle.getPitch() ); break; }
               case BAKEY_ROLL: { result.setY( angle.getRoll() ); break; }
               }
            }
         }

         return result;
      }

      void setPointPos( unsigned int pointIdx, const QPointF& pos )
      {
         if ( m_keyIdx == BAKEY_POS_X || m_keyIdx == BAKEY_POS_Y || m_keyIdx == BAKEY_POS_Z )
         {
            // translation keys
            unsigned int keysCount = m_boneAnimation.getTranslationKeysCount();
            if ( pointIdx >= keysCount )
            {
               return;
            }

            Vector posKey;
            float time;
            m_boneAnimation.getTranslationKey( pointIdx, posKey, time );

            switch( m_keyIdx )
            {
            case BAKEY_POS_X: { posKey[0] = pos.y() / ANIMATION_VALUE_SCALE; break; }
            case BAKEY_POS_Y: { posKey[1] = pos.y() / ANIMATION_VALUE_SCALE; break; }
            case BAKEY_POS_Z: { posKey[2] = pos.y() / ANIMATION_VALUE_SCALE; break; }
            }

            m_boneAnimation.setTranslationKey( pointIdx, posKey );
         }
         else
         {
            // orientation keys
            unsigned int keysCount = m_boneAnimation.getOrientationKeysCount();
            if ( pointIdx >= keysCount )
            {
               return;
            }

            Quaternion orientKey;
            float time;
            m_boneAnimation.getOrientationKey( pointIdx, orientKey, time );

            // extract Euler angle values
            EulerAngles angle;
            angle.setFromQuaternion( orientKey );
            
            // update the edited key feature
            switch( m_keyIdx )
            {
            case BAKEY_YAW: { angle[EulerAngles::Yaw] = pos.y(); break; }
            case BAKEY_PITCH: { angle[EulerAngles::Pitch] = pos.y(); break; }
            case BAKEY_ROLL: { angle[EulerAngles::Roll] = pos.y(); break; }
            }
            angle.normalize();

            // put the Euler angle back together into a quaternion value
            orientKey.setFromEulerAngles( angle );
            m_boneAnimation.setOrientationKey( pointIdx, orientKey );

         }
      }
   };
} // anonymous

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationKeysChart::SkeletonAnimationKeysChart()
   : m_animation( NULL )
{
   for ( unsigned int i = BAKEY_MIN; i < BAKEY_MAX; ++i )
   {
      m_keysVisibility[i] = false;
   }

   m_colors[BAKEY_POS_X] = QColor( 166, 78, 229 );
   m_colors[BAKEY_POS_Y] = QColor( 255, 71, 138 );
   m_colors[BAKEY_POS_Z] = QColor( 255, 125, 61 );
   m_colors[BAKEY_YAW] = QColor( 163, 206, 76 );
   m_colors[BAKEY_PITCH] = QColor( 65, 193, 136 );
   m_colors[BAKEY_ROLL] = QColor( 84, 206, 255 );
}

///////////////////////////////////////////////////////////////////////////////

SkeletonAnimationKeysChart::~SkeletonAnimationKeysChart()
{
   for ( ChartsMap::iterator it = m_charts.begin(); it != m_charts.end(); ++it )
   {
      delete it->second;
   }
   m_charts.clear();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationKeysChart::setAnimation( SkeletonAnimation& animation )
{
   // clear the charts
   for ( ChartsMap::iterator it = m_charts.begin(); it != m_charts.end(); ++it )
   {
      delete it->second;
   }
   m_charts.clear();

   // set the new animation
   m_animation = &animation;

   // set the time range
   float animLength = animation.getAnimationLength();
   float maxVal = 100.0f * ANIMATION_VALUE_SCALE; // we can edit an animation up to 100 meters - should be more than enough
   setSceneRect( 0, -maxVal, animLength * ANIMATION_TIME_SCALE, 2.0f * maxVal );
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationKeysChart::toggleBone( const std::string& boneName )
{
   if ( !m_animation )
   {
      return;
   }

   ChartsMap::iterator it = m_charts.find( boneName );
   if ( it != m_charts.end() )
   {
      // the bone's chart's being displayed - hide it
      delete it->second;
      m_charts.erase( it );
   }
   else
   {
      // the bone's not displayed - show it
      BoneSRTAnimation* boneAnimation = m_animation->getBoneDef( boneName );
      if ( boneAnimation != NULL )
      {
         BoneSRTAnimationChart* chart = new BoneSRTAnimationChart( *this, *boneAnimation );
         m_charts.insert( std::make_pair( boneName, chart ) );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

bool SkeletonAnimationKeysChart::isBoneDisplayed( const std::string& boneName ) const
{
   ChartsMap::const_iterator it = m_charts.find( boneName );
   return it != m_charts.end();
}

///////////////////////////////////////////////////////////////////////////////

void SkeletonAnimationKeysChart::toggleKeyVisibility( EBoneKeyId keyIdx )
{
   m_keysVisibility[keyIdx] = !m_keysVisibility[keyIdx];

   for ( ChartsMap::iterator it = m_charts.begin(); it != m_charts.end(); ++it )
   {
      it->second->onKeysVisibilityChanged();
   }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

BoneSRTAnimationChart::BoneSRTAnimationChart( SkeletonAnimationKeysChart& scene, BoneSRTAnimation& boneAnimation )
   : m_scene( scene )
   , m_boneAnimation( boneAnimation )
{
   for ( unsigned int i = BAKEY_MIN; i < BAKEY_MAX; ++i )
   {
      m_lines[i] = NULL;
   }
   updateVisibleKeys();
}

///////////////////////////////////////////////////////////////////////////////

BoneSRTAnimationChart::~BoneSRTAnimationChart()
{
   for ( unsigned int i = BAKEY_MIN; i < BAKEY_MAX; ++i )
   {
      if ( m_lines[i] )
      {
         m_scene.removeItem( m_lines[i] );
         delete m_lines[i];
         m_lines[i] = NULL;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void BoneSRTAnimationChart::onKeysVisibilityChanged()
{
   updateVisibleKeys();
}

///////////////////////////////////////////////////////////////////////////////

void BoneSRTAnimationChart::updateVisibleKeys()
{
   for ( unsigned int keyIdx = BAKEY_MIN; keyIdx < BAKEY_MAX; ++keyIdx )
   {
      if ( m_scene.isKeyVisible( (EBoneKeyId)keyIdx ) && m_lines[keyIdx] == NULL )
      {
         // the key should be visible, but it's not at the moment - display it
         m_lines[keyIdx] = new ChartLine( m_scene, m_scene.getColor( (EBoneKeyId)keyIdx ), 2.f, new PtsProvider( m_boneAnimation, keyIdx ) );
         m_scene.addItem( m_lines[keyIdx] );
         m_lines[keyIdx]->setPos( QPointF( 0.0f, 0.0f ) );
      }
      else if ( m_scene.isKeyVisible( (EBoneKeyId)keyIdx ) == false && m_lines[keyIdx] != NULL )
      {
         // the key should not be visible, bu it is at the moment - hide it
         m_scene.removeItem( m_lines[keyIdx] );
         delete m_lines[keyIdx];
         m_lines[keyIdx] = NULL;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////
