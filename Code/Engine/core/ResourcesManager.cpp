#include "core\ResourcesManager.h"
#include "core\ResourceLoader.h"
#include "core\Serializer.h"
#include "core\FileSerializer.h"
#include "core\IProgressObserver.h"


///////////////////////////////////////////////////////////////////////////////

ResourcesManager ResourcesManager::s_theInstance;

///////////////////////////////////////////////////////////////////////////////

ResourcesManager::ResourcesManager()
: m_filesystem( new Filesystem() )
, m_progressObserverCreator( NULL )
{
   m_filesystem->attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

ResourcesManager::~ResourcesManager()
{
   // delete the loaders
   for ( ResourceLoadersMap::iterator it = m_loaders.begin(); it != m_loaders.end(); ++it )
   {
      delete it->second;
   }
   m_loaders.clear();

   delete m_progressObserverCreator; m_progressObserverCreator = NULL;

   m_filesystem->detach( *this );
   delete m_filesystem; m_filesystem = NULL;

   reset();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::reset()
{
   unsigned int componentsCount = getComponentsCount();

   for ( ResourcesMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      Resource* resource = it->second;
      if ( resource )
      {
         // inform the resource about the components being removed
         for ( unsigned int compIdx = 0; compIdx < componentsCount; ++compIdx )
         {
            Component< ResourcesManager >* comp = getComponent( compIdx );
            resource->onComponentRemoved( *comp );
         }
      }

      resource->resetResourcesManager();
      delete resource;
   }
   m_resources.clear();
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::setFilesystem( Filesystem* filesystem )
{
   if (filesystem == NULL)
   {
      throw std::invalid_argument("NULL pointer instead a Filesystem instance");
   }

   // we need to delete all resources along with changing the filesystem
   reset();

   if ( m_filesystem )
   {
      m_filesystem->detach( *this );
   }
   delete m_filesystem;
   m_filesystem = filesystem;
   m_filesystem->attach( *this );
}

///////////////////////////////////////////////////////////////////////////////

bool ResourcesManager::addResource( Resource* resource )
{
   if ( resource == NULL )
   {
      return false;
   }

   if ( resource->isManaged() )
   {
      return false;
   }

   // correct the resource's name
   std::string correctResourcePath = Filesystem::changeFileExtension( resource->getFilePath(), resource->getVirtualExtension() );

   ResourcesMap::iterator it = m_resources.find( correctResourcePath );
   if ( it != m_resources.end() )
   {
      // a resource by this name already exists - and we can't replace it, since
      // pointers to it may exist all over the running application
      delete resource;
      return false;
   }

   resource->setFilePath( correctResourcePath );
   m_resources.insert( std::make_pair( correctResourcePath, resource ) );
   resource->setResourcesManager( *this );

   // inform the resource about the registered components
   unsigned int count = getComponentsCount();
   for ( unsigned int i = 0; i < count; ++i )
   {
      resource->onComponentAdded( *getComponent( i ) );
   }

   return true;
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

   // correct the resource's name
   std::string correctResourcePath = Filesystem::changeFileExtension( newPath, resource->getVirtualExtension() );

   ResourcesMap::iterator it = m_resources.find( correctResourcePath );
   if ( it == m_resources.end() )
   {
      return;
   }

   resource->setFilePath( correctResourcePath );
   m_resources.erase( it );
   m_resources.insert( std::make_pair( correctResourcePath, resource ) );
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

void ResourcesManager::scan( const std::string& rootDir, FilesystemScanner& scanner, bool recursive ) const
{
   // scan the contents of the filesystem
   m_filesystem->scan( rootDir, scanner, recursive );

   // scan the registered resources
   const std::size_t rootDirNameLen = rootDir.length();
   for ( ResourcesMap::const_iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      std::size_t rootDirPos = it->first.find( rootDir );
      if ( rootDirPos != 0 )
      {
         // we only want to consider files located in the root directory
         continue;
      }

      if ( recursive )
      {
         // it's a recursive search - add all files
         scanner.onFile( it->first );
      }
      else
      {
         // it's not a recursive search - add only the files in this directory
         std::size_t subDirCharPos = it->first.find_first_of( "/\\", rootDirNameLen );
         if ( subDirCharPos == std::string::npos )
         {
            scanner.onFile( it->first );
         }
      }

   }
}

///////////////////////////////////////////////////////////////////////////////

Resource* ResourcesManager::loadResource( ResourceLoader& loader )
{
   IProgressObserver* observer = createObserver();

   Resource* res = NULL;
   try
   {
      res = loader.load( *this, *observer );
   }
   catch( std::exception& ex )
   {
      ASSERT_MSG( false, ex.what() );
   }
   
   delete observer;

   return res;
}

///////////////////////////////////////////////////////////////////////////////

Resource& ResourcesManager::create( const std::string& name )
{
   ResourceLoader* loader = createResourceLoader( name );
   std::string correctResourcePath = Filesystem::changeFileExtension( name, loader->getOutputResourceExtension() );

   Resource* res = findResource( correctResourcePath );
   if ( res == NULL )
   {
      res = loadResource( *loader );
      if ( res )
      {
         addResource( res );
      }
   }

   delete loader;
   
   if ( res == NULL )
   {
      throw std::runtime_error( "Resource not found" );
   }

   return *res;
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::free( Resource* resource )
{
   if ( resource == NULL )
   {
      return;
   }

   ResourcesMap::iterator it = m_resources.find( resource->getResourceName() );
   if ( it != m_resources.end() && it->second != resource )
   {
      char errMsg[512];
      sprintf_s( errMsg, "Resources discrepancy - two resources share the same name '%s'", resource->getResourceName().c_str() );
      ASSERT_MSG( false, errMsg );

      // find a matching resource the hard way
      for ( it = m_resources.begin(); it != m_resources.end(); ++it )
      {
         if ( it->second == resource )
         {
            break;
         }
      }
   }

   if ( it != m_resources.end() )
   {
      m_resources.erase( it );
   }
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

ResourceLoader* ResourcesManager::createResourceLoader( const std::string& name ) const
{
   std::string extension = Filesystem::extractExtension( name );

   ResourceLoadersMap::const_iterator it = m_loaders.find( extension );
   ResourceLoader* loader = NULL;
   if ( it != m_loaders.end() )
   {
      loader = it->second->create();
   }
   else
   {
      loader = new DefaultResourceLoader();
   }

   loader->setLoadedFileName( name );
   return loader;
}

///////////////////////////////////////////////////////////////////////////////

IProgressObserver* ResourcesManager::createObserver() const
{
   if ( m_progressObserverCreator )
   {
      return m_progressObserverCreator->create();
   }
   else
   {
      return new NullProgressObserver();
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
   for ( ResourcesMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      it->second->onComponentRemoved( component );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onDirChanged( const std::string& dir )
{
   std::vector< std::string > entriesToRemove;

   // go through the registered resources and see if any of those 
   // still exist - if they don't remove them from the system
   for ( ResourcesMap::iterator it = m_resources.begin(); it != m_resources.end(); ++it )
   {
      const std::string& path = it->first;
      if ( path.find( dir ) != std::string::npos )
      {
         bool doesResourceExist = m_filesystem->doesExist( path );
         if ( !doesResourceExist )
         {
            entriesToRemove.push_back( path );
         }
      }
   }

   // remove the removed resources entries from the map
   unsigned int count = entriesToRemove.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ResourcesMap::iterator it = m_resources.find( entriesToRemove[i] );
      Resource* res = it->second;
      m_resources.erase( it );

      res->resetResourcesManager();
      delete res;
   }
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFileEdited( const std::string& path )
{
   // nothing to do here
}

///////////////////////////////////////////////////////////////////////////////

void ResourcesManager::onFileRemoved( const std::string& path )
{
   // check if the removed file was a resource, and if it was - remove it
   ResourcesMap::iterator it = m_resources.find( path );
   if ( it != m_resources.end() )
   {
      Resource* res = it->second;
      m_resources.erase( it );

      res->resetResourcesManager();
      delete res;
   }
}

///////////////////////////////////////////////////////////////////////////////
