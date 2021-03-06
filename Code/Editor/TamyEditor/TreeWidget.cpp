#include "TreeWidget.h"
#include <QMenu.h>
#include "core\Assert.h"
#include "TreeWidgetDescFactory.h"


///////////////////////////////////////////////////////////////////////////////

TreeWidget::TreeWidget( QWidget* parent, const QString& objName, const QString& iconsDir )
: QTreeWidget( parent )
, m_iconsDir( iconsDir )
, m_popupMenuItem( NULL )
{
   setObjectName( objName );
   setContextMenuPolicy( Qt::CustomContextMenu );

   connect( this, SIGNAL( customContextMenuRequested( const QPoint& ) ), this, SLOT( showPopupMenu( const QPoint& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::showPopupMenu( const QPoint& pos )
{
   m_popupMenuItem = itemAt( pos );
   if ( m_popupMenuItem )
   {
      QMenu* popupMenu = new QMenu( this );
      // add basic actions:

      // nodes collapsing and expansion - but these two work only
      // if a node has any children
      if ( m_popupMenuItem->childCount() > 0 )
      {
         // expanding the entire node's hierarchy
         QAction* expandNodeAction = new QAction( "Expand All", this );
         connect( expandNodeAction, SIGNAL( triggered() ), this, SLOT( expandNode() ) );
         popupMenu->addAction( expandNodeAction );

         // ... and collapsing it ( for symmetry of operations )
         QAction* collapseNodeAction = new QAction( "Collapse All", this );
         connect( collapseNodeAction, SIGNAL( triggered() ), this, SLOT( collapseNode() ) );
         popupMenu->addAction( collapseNodeAction );

         popupMenu->addSeparator();
      }

      // removing the node
      if ( m_popupMenuItem->parent() )
      {
         QAction* removeNodeAction = new QAction( QIcon( m_iconsDir + "quit.png" ), "Remove", this );
         connect( removeNodeAction, SIGNAL( triggered() ), this, SLOT( removeNode() ) );
         popupMenu->addAction( removeNodeAction );
      }

      // clearing the node's contents
      QAction* clearNodeAction = new QAction( QIcon( m_iconsDir + "clear.png" ), "Clear", this );
      connect( clearNodeAction, SIGNAL( triggered() ), this, SLOT( clearNode() ) );
      popupMenu->addAction( clearNodeAction );

      // inform about a menu being shown
      emit popupMenuShown( m_popupMenuItem, *popupMenu );

      // display the menu
      popupMenu->popup( mapToGlobal( pos ) );
   }
}


///////////////////////////////////////////////////////////////////////////////

void TreeWidget::expandNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );
   deepCollapse( m_popupMenuItem, true );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::collapseNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );
   deepCollapse( m_popupMenuItem, false );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::deepCollapse( QTreeWidgetItem* root, bool expand )
{
   std::list< QTreeWidgetItem* > itemsQueue;
   itemsQueue.push_back( root );

   while( !itemsQueue.empty() )
   {
      QTreeWidgetItem* currItem = itemsQueue.front();
      itemsQueue.pop_front();

      currItem->setExpanded( expand );

      unsigned int count = currItem->childCount();
      for ( unsigned int i = 0; i < count; ++i )
      {
         QTreeWidgetItem* child = currItem->child( i );
         if ( child )
         {
            itemsQueue.push_back( child );
         }
      }
   }
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::removeNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );
   ASSERT_MSG( m_popupMenuItem->parent(), "Item needsto have a parent in order to be removed from it" );

   emit removeNode( m_popupMenuItem->parent(), m_popupMenuItem );
}

///////////////////////////////////////////////////////////////////////////////

void TreeWidget::clearNode()
{
   ASSERT_MSG( m_popupMenuItem, "No tree item selected" );

   emit clearNode( m_popupMenuItem );
}

///////////////////////////////////////////////////////////////////////////////
