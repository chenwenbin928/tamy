/// @file      TamyEditor/GraphBlock.h
/// @brief     a graphical representation of a graph node
#pragma once

#include <QGraphicsItem>
#include <QColor>
#include <QRectF>
#include <QPen>
#include <QFont>
#include "core/Object.h"


///////////////////////////////////////////////////////////////////////////////

class QPainter;
class QGraphicsScene;

///////////////////////////////////////////////////////////////////////////////

/**
 * A graphical representation of a graph node.
 */
class GraphBlock :  public Object, public QGraphicsItem
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
   const int         CHARACTER_SIZE;

   Object*           m_node;

   Shape             m_shape;
   QPointF           m_position;
   QRectF            m_bounds;
   QRectF            m_captionBounds;

   QColor            m_fillColor;
   static QPen       s_textPen;
   static QPen       s_borderPen;
   static QPen       s_selectionPen;
   QFont             m_font;

   std::string       m_caption;

public:
   /**
    * Constructor.
    */
   GraphBlock( Shape shape = GBS_RECTANGLE, const QColor& fillColor = QColor( 0, 0, 0 ), Object* node = NULL );
   virtual ~GraphBlock();

   /**
    * Checks if the block overlaps the specified position.
    * 
    * @param pos
    */
   bool doesOverlap( const QPointF& pos ) const;

   /**
    * Returns the represented node instance.
    */
   Object* getNode() const { return m_node; }

   /**
    * Caches the block's state for storing purposes.
    */
   void saveState();

   /**
    * Restores the cached block state after the load.
    */
   void restoreState();

   // -------------------------------------------------------------------------
   // QGraphicsItem implementation
   // -------------------------------------------------------------------------
   /**
    * Returns the bounds of the block.
    */
   inline QRectF boundingRect() const { return m_bounds; }

   /**
    * Paints the block.
    */
   void paint( QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget );

   // -------------------------------------------------------------------------
   // Object implementation
   // -------------------------------------------------------------------------
   void onPropertyChanged( Property& property );

private:
   void calculateBounds();
};

///////////////////////////////////////////////////////////////////////////////

