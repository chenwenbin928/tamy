#pragma once

#include <map>
#include <string>
#include "core\Assert.h"


///////////////////////////////////////////////////////////////////////////////

template<typename ResourceType>
class ResourceStorage
{
private:
   typedef std::map<std::string, ResourceType*> ResourceMap;
   ResourceMap m_resources;

public:
   virtual ~ResourceStorage()
   {
      for (ResourceMap::iterator it = m_resources.begin();
         it != m_resources.end(); ++it)
      {
         delete it->second;
      }
   }

   ResourceType& get(const std::string& name)
   {
      ResourceMap::iterator it = m_resources.find(name);
      if (it != m_resources.end()) {return *(it->second);}

      throw std::runtime_error(std::string("Resource ") + std::string(name) + " doesn't exist");
   }

   const ResourceType& get(const std::string& name) const
   {
      ResourceMap::const_iterator it = m_resources.find(name);
      if (it != m_resources.end()) {return *(it->second);}

      throw std::runtime_error(std::string("Resource ") + std::string(name) + " doesn't exist");
   }

   bool is(const std::string& name) const
   {
      ResourceMap::const_iterator it = m_resources.find(name);
      return (it != m_resources.end());
   }

   bool add(ResourceType* resource)
   {
      ASSERT (resource != NULL, "NULL pointer instead a valid resource");

      ResourceMap::iterator it = m_resources.find(resource->getName());
      if (it != m_resources.end()) 
      {
         // resource like this already exists
         delete resource;
         return false;
      } 

      m_resources.insert(std::make_pair(resource->getName(), resource));

      return true;
   }
};

///////////////////////////////////////////////////////////////////////////////