#include "core\ResourcesManager.h"
#include "core\Serializer.h"
#include "core\FileSerializer.h"


///////////////////////////////////////////////////////////////////////////////

ResourcesManager::ResourcesManager()
: m_filesystem(new Filesystem())
{
}

///////////////////////////////////////////////////////////////////////////////

ResourcesManager::~ResourcesManager()
{
   delete m_filesystem; m_filesystem = NULL;

   reset();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::reset()
{
   for (ResourcesMap::iterator it = m_resources.begin();
      it != m_resources.end(); ++it)
   {
      delete it->second;
   }
   m_resources.clear();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::setFilesystem(Filesystem* filesystem)
{
   if (filesystem == NULL)
   {
      throw std::invalid_argument("NULL pointer instead a Filesystem instance");
   }

   delete m_filesystem;
   m_filesystem = filesystem;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::addResource( Resource* resource )
{
   if ( resource == NULL )
   {
      throw std::invalid_argument("NULL pointer instead a Resource instance");
   }

   if ( resource->isManaged() )
   {
      return;
   }

   ResourcesMap::iterator it = m_resources.find( resource->getFilePath() );
   if ( it == m_resources.end() )
   {
      m_resources.insert( std::make_pair( resource->getFilePath(), resource ) );
      resource->setResourcesManager( *this );
   }

   // inform the resource about the registered components
   unsigned int count = getComponentsCount();
   for ( unsigned int i = 0; i < count; ++i )
   {
      resource->onComponentAdded( *getComponent( i ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::moveResource( Resource* resource, const std::string& newPath )
{
   if ( resource == NULL )
   {
      throw std::invalid_argument("NULL pointer instead a Resource instance");
   }

   if ( resource->isManaged() )
   {
      return;
   }

   ResourcesMap::iterator it = m_resources.find( resource->getFilePath() );
   if ( it == m_resources.end() )
   {
      return;
   }

   resource->setFilePath( newPath );
   m_resources.erase( it );
   m_resources.insert( std::make_pair( newPath, resource ) );
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResourcesManager::findResource( const std::string& name )
{
   ResourcesMap::iterator it = m_resources.find( name );
   if ( it != m_resources.end() )
   {
      return it->second;
   }
   else
   {
      return NULL;
   }
}

///////////////////////////////////////////////////////////////////////////////

Resource& ResourcesManager::create( const std::string& name )
{
   Resource* res = findResource( name );
   if ( res == NULL )
   {
      std::string extension = Filesystem::extractExtension( name );
      std::ios_base::openmode fileAccessMode = Resource::getFileAccessMode( extension );

      File* file = m_filesystem->open( name, std::ios_base::in | fileAccessMode );
      Loader loader( new FileSerializer( file ) );
      res = &loader.load( *this );
   }

   return *res;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::save( const std::string& name, ExternalDependenciesSet& outExternalDependencies )
{
   Resource* res = findResource( name );
   if ( res != NULL )
   {
      std::string extension = Filesystem::extractExtension( name );
      std::ios_base::openmode fileAccessMode = Resource::getFileAccessMode( extension );

      File* file = m_filesystem->open( name, std::ios_base::out | fileAccessMode );
      Saver saver( new FileSerializer( file ) );
      saver.save( *res, outExternalDependencies );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onComponentAdded( Component< ResourcesManager >& component )
{
   for ( ResourcesMap::iterator it = m_resources.begin(); 
      it != m_resources.end(); ++it )
   {
      it->second->onComponentAdded( component );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onComponentRemoved( Component< ResourcesManager >& component )
{
   for ( ResourcesMap::iterator it = m_resources.begin(); 
      it != m_resources.end(); ++it )
   {
      it->second->onComponentRemoved( component );
   }
}

///////////////////////////////////////////////////////////////////////////////
