#ifndef _SINGLETONS_MANAGER_H
#define _SINGLETONS_MANAGER_H

/// @file   core\SingletonsManager.h
/// @brief  tool for managing unique instances of classes

#include <vector>
#include "core\Manageable.h"


///////////////////////////////////////////////////////////////////////////////

/**
 * This class can manage singleton instances of various classes.
 */
class SingletonsManager
{
private:
   std::vector<Manageable*> m_shared;

public:
   ~SingletonsManager();

   /**
    * This method allows to set a shared instance of a particular class
    * that can later on be queried using the @see shared method.
    *
    * @param shared     instance we want to manage
    * @return           'true' if the instance was correctly added, 
    *                   'false' otherwise
    */
   template<typename Type>
   bool setShared(Type* shared);

   /**
    * This method allows to set a shared instance of a particular class
    * that can later on be queried using the @see shared method
    *
    * When an instance is set through this method, it won't get deleted when
    * the ResourceManager instance is deleted
    */
   template<typename Type>
   bool setShared(Type& shared);

   /**
    * This method returns a shared instance of a certain class.
    *
    * @throw out_of_range if the instance of this class wasn't registered
    *
    * @return     reference to the requested instance
    */
   template<typename Type>
   Type& shared();

private:
   bool add(Manageable* newManageable);
};

///////////////////////////////////////////////////////////////////////////////

#include "core\SingletonsManager.inl"

///////////////////////////////////////////////////////////////////////////////

#endif // _SINGLETONS_MANAGER_H
