#include "core.h"
#include "core\Node.h"
#include "core\ASsert.h"
#include <algorithm>
#include "core\NodeObserver.h"
#include "core\BoundingVolume.h"
#include "core\BoundingSphere.h"


///////////////////////////////////////////////////////////////////////////////

Node::Node(const std::string& name)
   : m_name(name)
   , m_parent(NULL)
   , m_volume( new BoundingSphere( Vector( Quad_0 ), 0) )
   , m_globalVolume( new BoundingSphere( Vector( Quad_0 ), 0 ) )
{
   m_localMtx.setIdentity();
   m_globalMtx.setIdentity();

   // initialize our cache with some stupid data to force the initial update
   // of the global matrix for all the objects
   m_localMtxCache.setIdentity();
   m_localMtxCache( 0, 0 ) = -1;
   m_localMtxCache( 1, 1 ) = -1;
   m_localMtxCache( 2, 2 ) = -1;
   m_localMtxCache( 3, 3 ) = -1;

   m_parentGlobalMtxCache.setIdentity();
   m_parentGlobalMtxCache( 0, 0 ) = -1;
   m_parentGlobalMtxCache( 1, 1 ) = -1;
   m_parentGlobalMtxCache( 2, 2 ) = -1;
   m_parentGlobalMtxCache( 3, 3 ) = -1;
}

///////////////////////////////////////////////////////////////////////////////

Node::Node( const Node& rhs )
   : m_name( rhs.m_name )
   , m_parent( NULL )
   , m_volume( rhs.m_volume->clone() )
   , m_globalVolume( rhs.m_globalVolume->clone() )
{
   m_localMtx = rhs.m_localMtx;
   m_globalMtx = rhs.m_globalMtx;
   m_localMtxCache = rhs.m_localMtxCache;
   m_parentGlobalMtxCache = rhs.m_parentGlobalMtxCache;
}

///////////////////////////////////////////////////////////////////////////////

Node::~Node()
{
   delete m_volume;
   m_volume = NULL;

   delete m_globalVolume;
   m_globalVolume = NULL;

   m_parent = NULL;

   for (std::list<Node*>::iterator it = m_childrenNodes.begin();
      it != m_childrenNodes.end(); ++it)
   {
      delete *it;
   }
   m_childrenNodes.clear();
}

///////////////////////////////////////////////////////////////////////////////

void Node::setLocalMtx( const Matrix& localMtx ) 
{
   m_localMtx = localMtx;
}

///////////////////////////////////////////////////////////////////////////////

void Node::setRightVec( const Vector& vec )
{
   m_localMtx.setSideVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Node::setUpVec( const Vector& vec )
{
   m_localMtx.setUpVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Node::setLookVec( const Vector& vec )
{
   m_localMtx.setForwardVec<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Node::setPosition( const Vector& vec )
{
   m_localMtx.setPosition<3>( vec );
}

///////////////////////////////////////////////////////////////////////////////

void Node::getRightVec( Vector& outRightVec ) const
{
   outRightVec = m_localMtx.sideVec();
}

///////////////////////////////////////////////////////////////////////////////

void Node::getUpVec( Vector& outUpVec ) const
{
   outUpVec = m_localMtx.upVec();
}

///////////////////////////////////////////////////////////////////////////////

void Node::getLookVec( Vector& outLookVec ) const
{
   outLookVec = m_localMtx.forwardVec();
}

///////////////////////////////////////////////////////////////////////////////

void Node::getPosition( Vector& outPos ) const
{
   outPos = m_localMtx.position();
}

///////////////////////////////////////////////////////////////////////////////

const Matrix& Node::getGlobalMtx() const
{
   if ( hasParentNode() == false ) 
   {
      return m_localMtx;
   }

   // this node's global matrix can be influenced by the change
   // in this node's local matrix, or any of it's parents' global matrices.
   // we need to check for both events.
   bool localMatrixChanged = false;

   // The case with the parent's matrices will be dealt with using recurrence,
   // so we basically need to check only the immediate parent to spot the difference
   // in the global matrix

   if ( m_localMtxCache != m_localMtx )
   {
      m_localMtxCache = m_localMtx;
      m_globalMtx = m_localMtx;
      localMatrixChanged = true;
   }

   const Matrix& parentGlobalMtx = m_parent->getGlobalMtx();
   if ( localMatrixChanged || ( m_parentGlobalMtxCache != parentGlobalMtx ) )
   {
      m_parentGlobalMtxCache = parentGlobalMtx;
      m_globalMtx.setMul( m_localMtx, m_parentGlobalMtxCache );
   }

   return m_globalMtx;
}

///////////////////////////////////////////////////////////////////////////////

void Node::getGlobalVectors( Vector& outRightVec, Vector& outUpVec, Vector& outLookVec, Vector& outPos ) const
{
   const Matrix& globalMtx = getGlobalMtx();
   globalMtx.getVectors( outRightVec, outUpVec, outLookVec, outPos );
}

///////////////////////////////////////////////////////////////////////////////

const BoundingVolume& Node::getBoundingVolume() const
{
   const Matrix& globalMtx = getGlobalMtx();
   m_volume->transform( globalMtx, *m_globalVolume );
   return *m_globalVolume;
}

///////////////////////////////////////////////////////////////////////////////

void Node::setBoundingVolume( BoundingVolume* volume )
{
   if (volume == NULL)
   {
      ASSERT_MSG( false, "NULL pointer instead a BoundingVolume instance");
   }
   delete m_volume;
   m_volume = volume;

   delete m_globalVolume;
   m_globalVolume = m_volume->clone();
}

///////////////////////////////////////////////////////////////////////////////

Node* Node::getParentNode()
{
   return m_parent;
}

///////////////////////////////////////////////////////////////////////////////


void Node::addChild(Node* childNode)
{
   if (childNode->hasParentNode())
   {
      Node* prevParent = childNode->getParentNode();
      prevParent->removeChild(*childNode);
   }

   m_childrenNodes.push_back(childNode);
   childNode->setParent(*this);

   for ( std::list<NodeObserver*>::iterator it = m_observers.begin(); it != m_observers.end(); ++it )
   {
      (*it)->childAdded(*this, *childNode);
   }
}

///////////////////////////////////////////////////////////////////////////////

void Node::removeChild(Node& childNode)
{
   for (std::list<NodeObserver*>::iterator it = m_observers.begin();
        it != m_observers.end(); ++it)
   {
      (*it)->childRemoved(*this, childNode);
   }

   std::list<Node*>::iterator it = std::find(m_childrenNodes.begin(), m_childrenNodes.end(), &childNode);
   if (it != m_childrenNodes.end())
   {
      childNode.resetParent();
      m_childrenNodes.erase(it);
   }
}

///////////////////////////////////////////////////////////////////////////////

Node* Node::findNode( const std::string& name )
{
   std::list< Node* >   nodesQueue;
   nodesQueue.push_back( this );

   while ( !nodesQueue.empty() )
   {
      Node* currNode = nodesQueue.front();
      nodesQueue.pop_front();
      if ( currNode->getName() == name )
      {
         return currNode;
      }

      for ( std::list<Node*>::iterator it = currNode->m_childrenNodes.begin(); it != currNode->m_childrenNodes.end(); ++it )
      {
         if ( *it != NULL )
         {
            nodesQueue.push_back( *it );
         }
      }
   }

   return NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Node::accept(NodeVisitor& visitor)
{
   onAccept(visitor);

   for (std::list<Node*>::iterator it = m_childrenNodes.begin(); it != m_childrenNodes.end(); ++it)
   {
      (*it)->accept(visitor);
   }
}

///////////////////////////////////////////////////////////////////////////////

void Node::attachObserver(NodeObserver& observer)
{
   m_observers.push_back(&observer);

   for (std::list<Node*>::iterator it = m_childrenNodes.begin();
      it != m_childrenNodes.end(); ++it)
   {
      (*it)->attachObserver(observer);
   }
}

///////////////////////////////////////////////////////////////////////////////

void Node::detachObserver(NodeObserver& observer)
{
   for (std::list<NodeObserver*>::iterator it = m_observers.begin();
        it != m_observers.end(); ++it)
   {
      if (&observer == *it)
      {
         m_observers.erase(it);
         break;
      }
   }

   for (std::list<Node*>::iterator it = m_childrenNodes.begin();
      it != m_childrenNodes.end(); ++it)
   {
      (*it)->detachObserver(observer);
   }
}

///////////////////////////////////////////////////////////////////////////////
