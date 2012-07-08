#include "ProjectTree.h"
#include "core.h"

// widgets
#include <QVBoxLayout>
#include <QToolbar>
#include <QInputDialog>

// tree widget
#include "FSNodeMimeData.h"
#include "TypeDescFactory.h"
#include <QTreeWidgetItem>

// nodes
#include "ProjectTreeNode.h"
#include "ProjectRootNode.h"
#include "ProjectDirNode.h"
#include "ProjectLeafNode.h"

// project related stuff
#include "Project.h"

// tools
#include "ResourceManagementUtil.h"
#include <QSettings>
#include "AdditionalTreeItemFactories.h"


///////////////////////////////////////////////////////////////////////////////

ProjectTree::ProjectTree( QWidget* parentWidget )
   : QFrame( parentWidget )
   , m_fsTree( NULL )
   , m_rootDir( NULL )
   , m_toggleFileTypesViewBtn( NULL )
   , m_viewResourcesOnly( false )
   , m_itemsFactory( NULL )
   , m_section( NULL )
{
   // give the frame a meaningful name so that it's taken into consideration during UI serialization
   QString objectName = "ResourcesBrowser/projectTreeFrame";
   setObjectName( objectName );

   // setup the frame layout
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);
   setLayout( layout );

   Filesystem& fs = ResourcesManager::getInstance().getFilesystem();
   m_iconsDir = fs.getShortcut( "editorIcons" ).c_str();

   {
      // we want to be able to create all sorts of resources
      TypeDescFactory< Resource >* resourcesFactory = new TypeDescFactory< Resource >( m_iconsDir, fs, "unknownResourceIcon.png" );

      // except for new projects
      resourcesFactory->removeType< Project >();

      // and we want to be able to create new directories
      m_itemsFactory = new DirectoriesFactoryWrapper( resourcesFactory, m_iconsDir );
   }

   // setup the widgets
   initialize( objectName );
   onToggleFilesFiltering();
}

///////////////////////////////////////////////////////////////////////////////

ProjectTree::~ProjectTree()
{
   delete m_itemsFactory;
   m_itemsFactory = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::initialize( const QString& topObjectName )
{
   // toolbar
   {
      QToolBar* toolbar = new QToolBar( this );
      toolbar->setObjectName( topObjectName + "/toolbar" );
      layout()->addWidget( toolbar );

      // toggle file tabs
      {
         m_toggleFileTypesViewBtn = new QAction( tr( "Toggle file types" ), toolbar );
         toolbar->addAction( m_toggleFileTypesViewBtn );
         connect( m_toggleFileTypesViewBtn, SIGNAL( triggered() ), this, SLOT( onToggleFilesFiltering() ) );
      }
   }

   // filesystem tree
   {
      m_fsTree = new ProjectTreeWidget( this, topObjectName + "/m_fsTree", m_iconsDir );
      layout()->addWidget( m_fsTree );

      QStringList columnLabels; 
      columnLabels.push_back( "Name" );
      columnLabels.push_back( "Size" );
      m_fsTree->setColumnCount( columnLabels.size() );
      m_fsTree->setHeaderLabels( columnLabels );
      m_fsTree->setDragEnabled( true ); 
      m_fsTree->setDropIndicatorShown( true ); 
      connect( m_fsTree, SIGNAL( itemDoubleClicked( QTreeWidgetItem*, int ) ), this, SLOT( onItemDoubleClicked( QTreeWidgetItem*, int ) ) );
      connect( m_fsTree, SIGNAL( getItemsFactory( QTreeWidgetItem*, TreeWidgetDescFactory*& ) ), this, SLOT( onGetItemsFactory( QTreeWidgetItem*, TreeWidgetDescFactory*& ) ) );
      connect( m_fsTree, SIGNAL( addNode( QTreeWidgetItem*, unsigned int ) ), this, SLOT( onAddNode( QTreeWidgetItem*, unsigned int ) ) );
      connect( m_fsTree, SIGNAL( removeNode( QTreeWidgetItem*, QTreeWidgetItem* ) ), this, SLOT( onRemoveNode( QTreeWidgetItem*, QTreeWidgetItem* ) ) );
      connect( m_fsTree, SIGNAL( clearNode( QTreeWidgetItem* ) ), this, SLOT( onClearNode( QTreeWidgetItem* ) ) );
      connect( m_fsTree, SIGNAL( popupMenuShown( QTreeWidgetItem*, QMenu& ) ), this, SLOT( onPopupMenuShown( QTreeWidgetItem*, QMenu& ) ) );

      ResourcesManager& rm = ResourcesManager::getInstance();

      m_rootDir = new ProjectRootNode( m_fsTree );
      m_fsTree->addTopLevelItem( m_rootDir );

      m_rootDir->setExpanded( true );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::setSource( FilesystemSection* section )
{
   if ( m_section == section )
   {
      // it's the same section, nothing changes
      return;
   }

   // detach self as a listener from the previous section
   if ( m_section )
   {
      m_section->detach( *this );
   }

   // clear tree contents
   m_rootDir->clear();

   // set the new section
   m_section = section;

   // attach self as a listener from the previous section
   if ( m_section )
   {
      m_section->attach( *this );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onToggleFilesFiltering()
{
   // change the view mode flag
   m_viewResourcesOnly = !m_viewResourcesOnly;

   // update the icon on the view type toggling button
   QString iconName;
   if ( m_viewResourcesOnly )
   {
      iconName = m_iconsDir + "resource.png";
      m_toggleFileTypesViewBtn->setToolTip( "Switch to viewing all files" );
   }
   else
   {
      iconName = m_iconsDir + "fileInDir.png";
      m_toggleFileTypesViewBtn->setToolTip( "Switch to viewing resources only" );
   }
   m_toggleFileTypesViewBtn->setIcon( QIcon( iconName ) );

   // refresh the view
   refresh();
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onDirAdded( const FilePath& dir )
{
   addDirectory( dir );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onDirRemoved( const FilePath& dir )
{
   removeDirectory( dir );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onFileEdited( const FilePath& path )
{
   FilePath dir;
   path.extractDir( dir );
   refresh( dir.getRelativePath() );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onFileRemoved( const FilePath& path )
{
   FilePath dir;
   path.extractDir( dir );
   refresh( dir.getRelativePath() );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::addDirectory( const FilePath& dir )
{
   // add the path to it - piece by piece
   std::vector< std::string > pathElements;
   StringUtils::tokenize( dir.c_str(), "/\\", pathElements );
   uint count = pathElements.size();

   ProjectTreeNode* currNode = m_rootDir;
   for ( uint i = 0; i < count; ++i )
   {
      ProjectTreeNode* childNode = currNode->find( pathElements[i] );
      if ( childNode )
      {
         // the node exists - follow it deeper
         currNode = childNode;
      }
      else
      {
         // the node doesn't exist - create a new one
         currNode = new ProjectDirNode( currNode, pathElements[i] + "/" );
      }
   }

   // once the path's been recreated, scan the final directory
   refresh( dir );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::removeDirectory( const FilePath& dir )
{
   // remove all directories way to the root that are not a part of the project
   // and don't have any other children ( 'cause if they do, it means that those
   // children might either be part of the project or may be hosts to some parts of the project )
   
   ProjectTreeNode* removedDirNode = find( dir );
   while( removedDirNode )
   {
      QTreeWidgetItem* parentItem = removedDirNode->parent();
      if ( parentItem )
      {
         int childIdx = parentItem->indexOfChild( removedDirNode );
         parentItem->takeChild( childIdx );
      }

      if ( parentItem && parentItem->childCount() == 0 )
      {
         removedDirNode = static_cast< ProjectTreeNode* >( parentItem );
      }
      else
      {
         removedDirNode = NULL;
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::refresh( const std::string& rootDir )
{
   // find the entry corresponding to the specified root directory
   ProjectTreeNode* rootDirNode = find( rootDir );
   if ( !rootDirNode )
   {
      return;
   }

   // clean it up
   rootDirNode->clear();

   if ( !m_section )
   {
      // there's no section to scan
      return;
   }

   // scan the contents of the resources manager
   struct Scanner : public FilesystemSectionScanner
   {
      ProjectTreeNode*                    m_rootDirNode;
      TreeWidgetDescFactory*              m_itemsFactory;
      bool                                m_viewResourcesOnly;

      Scanner( FilesystemSection& section, ProjectTreeNode* rootDirNode, TreeWidgetDescFactory* itemsFactory, bool viewResourcesOnly )
         : FilesystemSectionScanner( section )
         , m_rootDirNode( rootDirNode )
         , m_itemsFactory( itemsFactory )
         , m_viewResourcesOnly( viewResourcesOnly )
      {
      }

      void onSectionDirectory( const FilePath& name )
      {
         const Filesystem& fs = ResourcesManager::getInstance().getFilesystem();
         std::string parentDirName = fs.extractDir( name );
         std::string newNodeName = fs.extractNodeName( name );

         if ( m_rootDirNode->find( newNodeName ) == NULL )
         { 
            // add the new node ( but only if it's unique )
            new ProjectDirNode( m_rootDirNode, newNodeName );
         }
      }

      void onSectionFile( const FilePath& name )
      {
         if ( m_viewResourcesOnly )
         {
            std::string extension = name.extractExtension();
            if ( !Resource::isResource( extension ) )
            {
               return;
            }
         }

         FilePath parentDirName;
         name.extractDir( parentDirName );
         std::string newNodeName = name.extractNodeName();

         if ( m_rootDirNode->find( newNodeName ) == NULL )
         {
            // add the new node ( but only if it's unique )
            new ProjectLeafNode( m_rootDirNode, newNodeName, *m_itemsFactory );
         }
      }
   };

   // scan the directory
   Scanner scanner( *m_section, rootDirNode, m_itemsFactory, m_viewResourcesOnly );
   ResourcesManager& resMgr = ResourcesManager::getInstance();
   resMgr.scan( rootDir, scanner, false );

}

///////////////////////////////////////////////////////////////////////////////

ProjectTreeNode* ProjectTree::find( const std::string& dir )
{
   // split the path int particular elements
   FilePath path( dir );
   std::vector< std::string > pathParts;
   path.getElements( pathParts );

   // if there were no elements, simply return the root node
   if ( pathParts.empty() )
   {
      return m_rootDir;
   }

   // find the node
   ProjectTreeNode* currItem = m_rootDir;
   unsigned int count = pathParts.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      std::string currPathPart = pathParts[i];
      ProjectTreeNode* nextItem = currItem->find( currPathPart );
      if ( !nextItem )
      {
         // that's the last leaf - there's nothing else to search in - bail
         break;
      }

      if ( i + 1 == count )
      {
         // we found it
         return nextItem;
      }

      // keep on looking starting from the node we've just analyzed
      currItem = nextItem;
   }

   // the node hasn't been mapped yet
   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

ProjectTreeNode* ProjectTree::open( const FilePath& path )
{
   ResourcesManager& resMgr = ResourcesManager::getInstance();

   // split the path int particular elements
   std::vector< std::string > pathParts;
   path.getElements( pathParts );

   // if there were no elements, simply return the root node
   if ( pathParts.empty() )
   {
      return m_rootDir;
   }

   // start mapping the nodes
   std::string currRelativePath = "/";

   ProjectTreeNode* currItem = m_rootDir;
   unsigned int count = pathParts.size();

   ProjectTreeNode* foundItem = NULL;
   for ( unsigned int i = 0; i < count; ++i )
   {
      std::string currPathPart = pathParts[i];
      ProjectTreeNode* nextItem = currItem->find( currPathPart );
      if ( !nextItem )
      {
         // that's the last leaf thus far - so scan it
         refresh( currRelativePath );

         // and try finding the corresponding node again
         nextItem = currItem->find( currPathPart );
      }

      if ( !nextItem )
      {
         // still nothing - this means that the path we're looking for doesn't exist - so bail
         break;
      }

      if ( i + 1 == count )
      {
         // we found it
         foundItem = nextItem;
         break;
      }

      // keep on looking starting from the node we've just analyzed
      currRelativePath += currPathPart;
      currItem = nextItem;
   }

   if ( foundItem )
   {
      // we found it - just to be sure, rescan the contents
      refresh( foundItem->getRelativePath() );

      if ( dynamic_cast< ProjectDirNode* >( foundItem ) )
      {
         foundItem->setExpanded( true );
      }
   }

   // the path doesn't exist
   return foundItem;
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::focusOn( ProjectTreeNode* node )
{
   m_fsTree->setCurrentItem( node );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::addNode( unsigned int idx, const std::string& parentDir )
{
   const SerializableReflectionType* type = m_itemsFactory->getClass( idx );

   if ( type == NULL )
   {
      // learn the new file's name
      bool ok = false;
      QString newDirName = QInputDialog::getText( this, "New directory", "Dir name:", QLineEdit::Normal, "", &ok );
      if ( ok )
      {
         const Filesystem& fs = ResourcesManager::getInstance().getFilesystem();
         fs.mkdir( parentDir + newDirName.toStdString() );
      }
   }
   else
   {
      ResourceManagementUtil::createResource( this, *type, parentDir );
   }
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onGetItemsFactory( QTreeWidgetItem* parent, TreeWidgetDescFactory*& outFactoryPtr )
{
   if ( !m_section )
   {
      // there's no section - don't show a thing
      outFactoryPtr = NULL;
      return;
   }

   ProjectTreeNode* nodeItem = dynamic_cast< ProjectTreeNode* >( parent );
   FilePath path = nodeItem->getRelativePath();
   if ( !m_section->isMember( path ) )
   {
      // the path is not a part of the displayed section - don't show a thing
      outFactoryPtr = NULL;
      return;
   }

   ProjectTreeNode* item = dynamic_cast< ProjectTreeNode* >( parent );
   outFactoryPtr = item->getDescFactory( *this );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onAddNode( QTreeWidgetItem* parent, unsigned int typeIdx )
{  
   ProjectTreeNode* item = static_cast< ProjectTreeNode* >( parent );
   item->addNode( typeIdx, *this );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onRemoveNode( QTreeWidgetItem* parent, QTreeWidgetItem* child )
{
   ProjectTreeNode* parentItem = static_cast< ProjectTreeNode* >( parent );
   ProjectTreeNode* childItem = static_cast< ProjectTreeNode* >( child );
   parentItem->removeNode( childItem );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onClearNode( QTreeWidgetItem* node )
{
   ProjectTreeNode* item = static_cast< ProjectTreeNode* >( node );
   item->clearNodes();
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onItemDoubleClicked( QTreeWidgetItem* item, int column )
{
   ProjectTreeNode* entry = static_cast< ProjectTreeNode* >( item );
   entry->openItem( *this );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::onPopupMenuShown( QTreeWidgetItem* node, QMenu& menu )
{
   if ( !m_section )
   {
      // there's no section - don't show a thing
      return;
   }

   ProjectTreeNode* nodeItem = static_cast< ProjectTreeNode* >( node );
   FilePath path = nodeItem->getRelativePath();
   if ( !m_section->isMember( path ) )
   {
      // the path is not a part of the displayed section - don't show a thing
      return;
   }

   emit popupMenuRequest( node, menu );
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::saveLayout( QSettings& settings )
{
   // begin serialization
   settings.beginGroup( objectName() + "/layoutSettings" );

   // save all open directories
   {
      QString openDirectories;

      std::list< ProjectTreeNode* > nodesToExplore;
      nodesToExplore.push_back( m_rootDir );
      while ( !nodesToExplore.empty() )
      {
         ProjectTreeNode* currNode = nodesToExplore.front(); nodesToExplore.pop_front();
         if ( currNode->isExpanded() )
         {
            // memorize only expanded nodes
            openDirectories += ";" + QString( currNode->getRelativePath().c_str() );
         }

         uint childrenCount = currNode->childCount();
         for ( uint i = 0; i < childrenCount; ++i )
         {
            ProjectDirNode* childDirNode = dynamic_cast< ProjectDirNode* >( currNode->child( i ) );
            if ( childDirNode )
            {
               nodesToExplore.push_back( childDirNode );
            }
         }
      }

      settings.setValue( "openDirectories", openDirectories );
   }

   // finish serialization
   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////

void ProjectTree::loadLayout( QSettings& settings )
{
   // begin serialization
   settings.beginGroup( objectName() + "/layoutSettings" );

   // deserialize all open directories
   {
      std::string openDirectories = settings.value( "openDirectories" ).toString().toStdString();

      std::vector< std::string > arrOpenDirectories;
      StringUtils::tokenize( openDirectories, ";", arrOpenDirectories );
      uint count = arrOpenDirectories.size();
      for ( uint i = 0; i < count; ++i )
      {
         open( arrOpenDirectories[i] );
      }
   }

   // finish serialization
   settings.endGroup();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

ProjectTreeWidget::ProjectTreeWidget( QWidget* parent, const QString& objName, const QString& iconsDir )
   : TreeWidget( parent, objName, iconsDir )
{
}

///////////////////////////////////////////////////////////////////////////////

QMimeData* ProjectTreeWidget::mimeData( const QList<QTreeWidgetItem *> items ) const
{
   QMimeData* data = __super::mimeData( items );

   std::vector< std::string > paths;
   unsigned int count = items.size();
   for ( unsigned int i = 0; i < count; ++i )
   {
      ProjectTreeNode* node = dynamic_cast< ProjectTreeNode* >( items[ i ] );
      if ( node )
      {
         paths.push_back( node->getRelativePath() );
      }
   }

   FSNodeMimeData dataEncoder( paths );
   dataEncoder.save( *data );

   return data;
}

///////////////////////////////////////////////////////////////////////////////
