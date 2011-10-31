#include "core\Filesystem.h"
#include "core\File.h"
#include "core\StringUtils.h"
#include <stdexcept>
#include <list>


///////////////////////////////////////////////////////////////////////////////

namespace // anonymous
{
   class HierarchicalFilesDeleter : public FilesystemScanner
   {
   private:
      struct DeletionDesc
      {
         std::string path;
         bool        isDir;

         DeletionDesc( const std::string& _path, bool _isDir )
            : path( _path )
            , isDir( _isDir )
         {}
      };

   private:
      std::string                m_rootDir;
      std::list< DeletionDesc >  m_deletionList;
      
   public:
      HierarchicalFilesDeleter( const std::string& rootDir )
         : m_rootDir( rootDir )
      {}

      void perform()
      {
         for ( std::list< DeletionDesc >::const_iterator it = m_deletionList.begin();
            it != m_deletionList.end(); ++it )
         {
            const DeletionDesc& desc = *it;
            if ( desc.isDir )
            {
               RemoveDirectory( desc.path.c_str() );
            }
            else
            {
               DeleteFile( desc.path.c_str() );
            }
         }
      }

      // ----------------------------------------------------------------------
      // FilesystemScanner
      // ----------------------------------------------------------------------
      void onDirectory( const std::string& name ) 
      { 
         m_deletionList.push_front( DeletionDesc( m_rootDir + name, true ) ); 
      }

      void onFile( const std::string& name )
      {
         m_deletionList.push_front( DeletionDesc( m_rootDir + name, false ) ); 
      }
   };
};

///////////////////////////////////////////////////////////////////////////////

Filesystem::Filesystem(const std::string& rootDir)
{
   changeRootDir(rootDir);
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::changeRootDir(const std::string& rootDir)
{
   m_rootDir = rootDir;

   std::size_t endingSlashPos = m_rootDir.find_last_of("/\\");
   if (endingSlashPos != (m_rootDir.length() - 1))
   {
      m_rootDir += "/";
   }
}

///////////////////////////////////////////////////////////////////////////////

const std::string& Filesystem::getCurrRoot() const
{
   return m_rootDir;
}

///////////////////////////////////////////////////////////////////////////////

bool Filesystem::doesExist(const std::string& fileName) const
{
   File* file = NULL;
   try
   {
      file = open(fileName);
   }
   catch (std::runtime_error&)
   {
      file = NULL;
   }

   bool exists = (file != NULL);
   delete file;

   return exists;
}

///////////////////////////////////////////////////////////////////////////////

File* Filesystem::open(const std::string& fileName, 
                       const std::ios_base::openmode mode) const
{
   std::string fullFileName = m_rootDir + fileName;
   File* file = new File(fullFileName, mode);

   // we're writing to a file - that changes the fs contents, therefore
   // send out the notifications
   if ( ( mode & std::ios_base::out ) == std::ios_base::out )
   {
      std::string dir = extractDir( fileName );
      notifyDirChange( dir );
   }

   return file;
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::mkdir( const std::string dirName ) const
{
   std::string absPath = toAbsolutePath( dirName );
   CreateDirectory( absPath.c_str(), NULL );

   std::string parentDir = extractDir( dirName );
   notifyDirChange( parentDir );
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::remove( const std::string path ) const
{
   // create a double-null terminated string with the absolute path
   // to the element we want to remove
   std::string absPathStr = toAbsolutePath( path );
   std::size_t strLen = absPathStr.length() + 2;
   char* absPath = new char[ strLen ];
   memset( absPath, 0, strLen );
   strcpy_s( absPath, strLen, absPathStr.c_str() );
   absPath[ strLen - 2 ] = 0;
   absPath[ strLen - 1 ] = 0;

   // setup a shell operation that will remove the element
   SHFILEOPSTRUCT fileOp;
   ZeroMemory( &fileOp, sizeof( SHFILEOPSTRUCT ) );
   fileOp.pFrom = absPath;
   fileOp.wFunc = FO_DELETE;
   fileOp.fFlags = FOF_SILENT | FOF_ALLOWUNDO;
   int result = SHFileOperation( &fileOp );
   
   // cleanup
   delete [] absPath;

   // send notifications
   std::string parentDir = extractDir( path );
   notifyDirChange( parentDir );
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::extractExtension( const std::string& fileName )
{
   std::size_t pos = fileName.find_last_of( "." );
   if ( pos != std::string::npos )
   {
      return fileName.substr( pos + 1 );
   }
   else
   {
      return "";
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::extractDir( const std::string& fileName )
{
   std::size_t lastSlash = fileName.find_last_of( "/" );
   if ( lastSlash == std::string::npos )
   {
      // the name doesn't contain any slash - therefore there's no dir
      // definition in the path
      return "";
   }
   else if ( lastSlash < fileName.length() - 1 )
   {
      // this is a file name - we can just cut the name
      // at the slash position and take the right splice
      return fileName.substr( 0, lastSlash + 1 );
   }
   else
   {
      // this is a directory ( ends with a slash ) - we need to find
      // a previous slash and cut the name up till there
      std::size_t prevSlash = fileName.find_last_of( "/", lastSlash - 1 );

      if ( lastSlash == 0 || prevSlash == std::string::npos )
      {
         // no directory is specified
         return "";
      }
      else 
      {
         return fileName.substr( 0, prevSlash + 1 );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::normalize( const std::string& fileName, std::string& outFileName )
{
   outFileName = "";

   int lastSlash = 0;
   int nextSlash = fileName.find_first_of( "\\" );

   while( (std::size_t)nextSlash != std::string::npos )
   {
      outFileName += fileName.substr( lastSlash, nextSlash - lastSlash );
      outFileName += "/";

      lastSlash = nextSlash + 1;
      nextSlash = fileName.find_first_of( "\\", lastSlash );
   }

   outFileName += fileName.substr( lastSlash, fileName.length() );
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::leaveDir( const std::string& directory, unsigned int levels, std::string& outDirectory )
{
   // first - remove the last slash, if any's appended
   std::size_t lastSlash = directory.find_last_of( "/" );
   if ( lastSlash == directory.length() - 1 )
   {
      outDirectory = directory.substr( 0, directory.length() - 1 );
   }
   else
   {
      outDirectory = directory;
   }

   // now remove the proper amount of directories in the path
   while( levels > 0 )
   {
      std::size_t lastSlash = outDirectory.find_last_of( "/" );
      if ( lastSlash == std::string::npos )
      {
         // the name doesn't contain any slash - therefore there's no dir
         // definition in the path
         return;
      }

      outDirectory = outDirectory.substr( 0, lastSlash );
      --levels;
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::extractNodeName( const std::string& fileName )
{
   std::size_t lastSlash = fileName.find_last_of( "/" );
   if ( lastSlash == std::string::npos )
   {
      // the name doesn't contain any slash - therefore there's no dir
      // definition in the path
      return fileName;
   }
   else if ( lastSlash < fileName.length() - 1 )
   {
      // this is a file name - we can just cut the name
      // at the slash position and take the right splice
      return fileName.substr( lastSlash + 1 );
   }
   else
   {
      // this is a directory ( ends with a slash ) - we need to find
      // a previous slash and cut the name up till there
      std::size_t prevSlash = fileName.find_last_of( "/", lastSlash - 1 );

      if ( lastSlash == 0 || prevSlash == std::string::npos )
      {
         // no directory is specified
         return fileName;
      }
      else 
      {
         return fileName.substr( prevSlash + 1 );
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::changeFileExtension( const std::string& fileName, 
                                             const std::string& newExtension )
{
   std::size_t pos = fileName.find_last_of( "." );
   if ( pos != std::string::npos )
   {
      return fileName.substr( 0, pos + 1 ) + newExtension;
   }
   else
   {
      return fileName + "." + newExtension;
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::toRelativePath(const std::string& absoluteFilePath) const
{
   // tokenize both the filename and the root dir
   std::vector<std::string> fileNameTokens;
   std::vector<std::string> rootDirTokens;

   StringUtils::tokenize( absoluteFilePath, "/", fileNameTokens );
   StringUtils::tokenize( m_rootDir, "/", rootDirTokens );

   // if the root dir path contains more or equal amount of nodes
   // as the filename, file can't possibly be located inside that directory
   if (rootDirTokens.size() >= fileNameTokens.size())
   {
      return "";
   }

   // compare all the nodes from the root directory to those on the file
   // to make sure the file is indeed located inside the root director
   unsigned int rootDirTokensCount = rootDirTokens.size();
   for (unsigned int i = 0; i < rootDirTokensCount; ++i)
   {
      if (rootDirTokens[i] != fileNameTokens[i])
      {
         return "";
      }
   }

   // assemble a file name relative to the root directory
   std::string relativeFileName = "";
   unsigned int fileNameTokensCount = fileNameTokens.size();
   for (unsigned int i = rootDirTokensCount; i < fileNameTokensCount; ++i)
   {
      relativeFileName += "/" + fileNameTokens[i];
   }

   // if the path ended with a backslash (meaning that it was a path to a directory),
   // finish it with one as well
   if ( absoluteFilePath[ absoluteFilePath.length() - 1 ] == '/' )
   {
      relativeFileName += "/";
   }
   return relativeFileName;
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::toAbsolutePath( const std::string& relativeFilePath ) const
{
   return m_rootDir + relativeFilePath;
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::setShortcut( const std::string& shortcut, const std::string& relativePath )
{
   Shortcuts::iterator it = m_shortcuts.find( shortcut );
   if ( it != m_shortcuts.end() )
   {
      it->second = relativePath;
   }
   else
   {
      m_shortcuts.insert( std::make_pair( shortcut, relativePath ) );
   }
}

///////////////////////////////////////////////////////////////////////////////

std::string Filesystem::getShortcut( const std::string& shortcut ) const
{
   Shortcuts::const_iterator it = m_shortcuts.find( shortcut );
   if ( it != m_shortcuts.end() )
   {
      return m_rootDir + it->second;
   }
   else
   {
      return "";
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::attach( FilesystemListener& listener )
{
   m_listeners.insert( &listener );
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::detach( FilesystemListener& listener )
{
   Listeners::iterator it = m_listeners.find( &listener );
   if ( it != m_listeners.end() )
   {
      m_listeners.erase( it );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::notifyDirChange( const std::string& dir ) const
{
   for ( Listeners::const_iterator it = m_listeners.begin();
         it != m_listeners.end(); ++it )
   {
      ( *it )->onDirChanged( dir );
   }
}

///////////////////////////////////////////////////////////////////////////////

void Filesystem::scan( const std::string& rootDir, FilesystemScanner& scanner, bool recursive ) const
{
   std::vector< std::string > pathsStack;
   pathsStack.push_back( m_rootDir + rootDir + "/" );

   while( !pathsStack.empty() )
   {
      std::string currPath = pathsStack.back();
      pathsStack.pop_back();

      WIN32_FIND_DATA findFileData;
      HANDLE hFind;
      bool result = true;
      for ( hFind = FindFirstFile( ( currPath + "*" ).c_str(), &findFileData );
            hFind != INVALID_HANDLE_VALUE && result; 
            result = FindNextFile( hFind, &findFileData ) )
      {
         std::string name = findFileData.cFileName;
         if ( ( findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
         {
            // we found a directory
            if ( name != "." && name != ".." )
            {
               std::string dirName = currPath + name + "/";
               scanner.onDirectory( toRelativePath( dirName ) );

               if ( recursive )
               {
                  // this is a recursive search, so add the found directory to the search tree
                  pathsStack.push_back( dirName );
               }
            }
         }
         else
         {
            // we found a file
            scanner.onFile( toRelativePath( currPath + name ) );
         }
      }

      FindClose( hFind );
   }
}

///////////////////////////////////////////////////////////////////////////////
