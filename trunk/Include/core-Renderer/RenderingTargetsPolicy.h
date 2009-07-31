#pragma once


///////////////////////////////////////////////////////////////////////////////

/**
 * This policy defines what targets should be used for particular 
 * material rendering passes
 */
class RenderingTargetsPolicy
{
public:
   virtual ~RenderingTargetsPolicy() {}

   /**
    * The method sets the rendering targets for a pass
    * and returns the number of targets that were set
    */
   virtual unsigned int setTargets(unsigned int passIdx) = 0;

   /**
    * This method should return the number of rendering passes
    * it has the targets defined for
    */
   virtual unsigned int getDefinedPassesCount() const = 0;
};

///////////////////////////////////////////////////////////////////////////////