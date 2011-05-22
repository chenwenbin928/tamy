/// @file   core-AI/AnimationTimeline.h
/// @brief  a utility structure for keeping a specific type of animation keys
#ifndef _ANIMATION_TIMELINE_H
#define _ANIMATION_TIMELINE_H


///////////////////////////////////////////////////////////////////////////////

/**
 * A utility structure for keeping a specific type of animation keys.
 *
 * The user needs to specify a LEPR functor with a signature:
 * class LERP
 * {
 * public:
 *    void operator()( T& outValue, const T& start, const T& end, float percentage ) const;
 * };
 */
template< typename T, typename LERP >
class AnimationTimeline
{
public:
   std::vector< float >    m_time;
   std::vector< T >        m_keys;

private:
   LERP                    m_lerp;

public:
   /**
    * Adds the specified key at the specified time.
    *
    * @param time
    * @param key
    */
   void addKey( float time, const T& key );

   /**
    * Returns the key value at the specified time.
    *
    * @param lastCheckedKeyIdx
    * @param time
    * @param outKey
    *
    * @return  'true' if a value was found, 'false' otherwise
    */
   bool getKey( unsigned int& lastCheckedKeyIdx, float time, T& outKey ) const;
};

///////////////////////////////////////////////////////////////////////////////

#include "core-AI/AnimationTimeline.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _ANIMATION_TIMELINE_H