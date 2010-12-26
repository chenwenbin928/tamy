/// @file      TamyEditor/GraphBlock.h
/// @brief     a graphical representation of a graph node
#pragma once

#include <QGraphicsItem>
#include <QColor>
#include <QRectF>
#include <QPen>
#include <QFont>
#include <QGraphicsLineItem>
#include "core/Object.h"


///////////////////////////////////////////////////////////////////////////////

class QPainter;
class QGraphicsScene;
class GraphBlockSocket;
class GraphBlockConnection;

///////////////////////////////////////////////////////////////////////////////

enum GraphBlockSocketPosition
{
   GBSP_LEFT,
   GBSP_RIGHT,
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A graphical representation of a graph node.
 */
class GraphBlock : public Object, public QGraphicsItem
{
   DECLARE_CLASS( GraphBlock )

public:
   enum Shape
   {
      GBS_RECTANGLE,
      GBS_CIRCLE,
      GBS_ROUNDED,
   };

private:
   std::string                         m_caption;

   QPointF                             m_position;
   QRectF                              m_bounds;
   QRectF                              m_captionBounds;
   QRectF                              m_totalBounds;

   static QPen                         s_textPen;
   static QPen                         s_borderPen;
   static QPen                         s_selectionPen;
   QFont                               m_font;

   // sockets
   std::vector< GraphBlockSocket* >    m_sockets;
   unsigned int                        m_leftSocketsCount;
   unsigned int                        m_rightSocketsCount;

public:
   /**
    * Constructor.
    */
   GraphBlock();
   virtual ~GraphBlock();

   /**
    * Sets the caption of the block.h
    *
    * @param caption
    */
   void setCaption( const char* caption );

   /**
    * Checks if the block overlaps the specified position.
    * 
    * @param pos
    */
   bool doesOverlap( const QPointF& pos ) const;

   /**
    * Returns the represented node instance.
    */
   virtual Object& getNode() { return *( reinterpret_cast< Object* >( NULL ) ); }

   /**
    * Returns a list of connections the block's involved in.
    *
    * @param outConnections
    */
   void getConnections( std::vector< GraphBlockConnection* >& outConnections ) const;

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   inline QRectF boundingRect() const { return m_totalBounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );
   void mouseMoveEvent( QGraphicsSceneMouseEvent* event );

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( Property& property );

   // -------------------------------------------------------------------------
   // Block layout operation
   // -------------------------------------------------------------------------
   /**
    * Forces the block to recalculate its layout
    */
   void calculateBounds();

   /**
    * Notifies the block that a connection has been created.
    *
    * @param connection       created connection
    */
   virtual void onConnectionCreated( GraphBlockConnection& connection ) {}

   /**
    * Notifies the block that a connection is about to be removed.
    *
    * @param connection       connection to be removed
    */
   virtual void onConnectionRemoved( GraphBlockConnection& connection ) {}

protected:
   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectPreSave();
   void onObjectLoaded();

   // -------------------------------------------------------------------------
   // Block layout settings
   // -------------------------------------------------------------------------
   /**
    * Returns the shape of the block.
    */
   virtual Shape getShape() const { return GBS_RECTANGLE; }

   /**
    * Returns the color of the block background.
    */
   virtual QColor getBgColor() const { return QColor( 255, 255, 255 ); }

   /**
    * Returns the pen used to draw the border.
    */
   virtual QPen getBorderPen() const { return isSelected() ? s_selectionPen : s_borderPen; }

   /**
    * Adds a new socket to the block.
    *
    * @return     reference to the newly created socket
    */
   GraphBlockSocket& addSocket();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * A single socket placed on a graph block.
 *
 * Each socket maintains a list of all the connections it participates
 * in so that when it changes it's position, it can inform the connection
 * that it needs to recalculate its bounds.
 * 
 * The socket is responsible for notifying a block that a new connection
 * has been established - providing it's the source socket.
 * The responsibility of a destination socket is just to inform the connection
 * about the changes in its whereabouts.
 */
class GraphBlockSocket : public Object, public QGraphicsItem
{
   DECLARE_CLASS( GraphBlockSocket )


   friend class GraphBlockConnection;

private:
   GraphBlock*                               m_parent;

   GraphBlockSocketPosition                  m_blockSide;
   std::string                               m_name;

   std::vector< GraphBlockConnection* >      m_connections;

   QFont                                     m_font;
   QRectF                                    m_bounds;
   QRectF                                    m_nameBounds;
   QPointF                                   m_position;

public:
   /**
    * Constructor.
    *
    * @param parent
    */
   GraphBlockSocket( GraphBlock* parent = NULL );

   /**
    * Initializes the socket
    *
    * @param pos     socket's position ( left or right )
    * @param name    socket's name
    */
   void initialize( GraphBlockSocketPosition pos, const char* name );

   /**
    * Returns the parent graph block.
    */
   inline GraphBlock& getParentBlock() const { return *m_parent; }

   /**
    * Returns the current socket position.
    */
   inline GraphBlockSocketPosition getPosition() const { return m_blockSide; }

   /**
    * Returns the width of the socket's name.
    */
   float getNameWidth() const;

   /**
    * Recalculates the bounds of the connections this socket is involved in.
    */
   void calculateConnectionBounds();

   // -------------------------------------------------------------------------
   // Interface for connections management
   // -------------------------------------------------------------------------
   /**
    * Adds a new connection between this socket and another one.
    *
    * @param connection
    */
   void addConnection( GraphBlockConnection& connection );

   /**
    * Removes the specified connection from the socket.
    *
    * @param connection
    */
   void removeConnection( GraphBlockConnection& connection );

   /**
    * Checks the connection between this socket and the specified one.
    *
    * @param socket     socket a connection to which we want to confirm.
    */
   bool isConnectedTo( GraphBlockSocket& socket ) const;

   /**
    * Returns a list of all connections the socket has.
    */
   inline const std::vector< GraphBlockConnection* >& getConnections() const { return m_connections; }

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   QRectF boundingRect() const { return m_bounds; }
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

protected:
   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   void mousePressEvent( QGraphicsSceneMouseEvent* event );
   void mouseReleaseEvent( QGraphicsSceneMouseEvent* event );

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectPreSave();
   void onObjectLoaded();

private:
   void calculateBounds();
};

///////////////////////////////////////////////////////////////////////////////

/**
 * Connection between two graph block sockets.
 *
 * The connection manages the information kept in the participating socket
 * (it's sort of a mediator in this context).
 */
class GraphBlockConnection : public Object, public QGraphicsLineItem
{
   DECLARE_CLASS( GraphBlockConnection )

private:
   GraphBlockSocket*       m_source;
   GraphBlockSocket*       m_destination;


public:
   /**
    * Constructor.
    *
    * @param parent
    */
   GraphBlockConnection( GraphBlockSocket* source = NULL, GraphBlockSocket* destination = NULL );

   /**
    * Calculates the bounds of the connection.
    */
   void calculateBounds();

   /**
    * Called when the connection gets removed.
    */
   void onRemoved();

   /**
    * Returns the source socket.
    */
   inline GraphBlockSocket& getSource() { return *m_source; }

   /**
    * Returns the destination socket.
    */
   inline GraphBlockSocket& getDestination() { return *m_destination; }

protected:
   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onObjectLoaded();
};

///////////////////////////////////////////////////////////////////////////////