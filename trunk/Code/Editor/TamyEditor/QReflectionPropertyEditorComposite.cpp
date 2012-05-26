#include "QReflectionPropertyEditorComposite.h"
#include "QReflectionObjectEditor.h"
#include <QToolBox.h>
#include <QCommonStyle.h>
#include <QStyleOption.h>
#include <QBoxLayout.h>
#include <QFrame.h>
#include <QLabel.h>


///////////////////////////////////////////////////////////////////////////////

QReflectionPropertyEditorComposite::QReflectionPropertyEditorComposite( ReflectionPropertyArray* property )
: ReflectionPropertyEditorComposite( property )
{
   QVBoxLayout* layout = new QVBoxLayout( this );
   layout->setSpacing(0);
   layout->setMargin(0);

   setFrameStyle( QFrame::StyledPanel );

   QString propertyLabel = "<propertyLabel.todo>";
   if (propertyLabel.isEmpty() == false)
   {
      QLabel* label = new QLabel( propertyLabel, this );
      label->setPalette( QPalette( qRgb( 191, 191, 191 ) ) );
      label->setAutoFillBackground( true );

      label->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
      layout->addWidget(label);
   }

   // <editor.todo> use a list to display the array
   m_toolBox = new QToolBox( this );
   m_toolBox->setStyleSheet( "QToolBox::tab {                                 \
                             background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,                 \
                             stop: 0 #60C7FF, stop: 1.0 #149CE0);                                    \
                             border-radius: 5px;                                                  \
                             color: black;                                                           \
                             }                                                                       \
                             QToolBox::tab:selected {                                                \
                             background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1,                 \
                             stop: 0 #149CE0, stop: 1.0 #60C7FF);                                    \
                             font: italic;                                                           \
                             color: black;                                                           \
                             }" );

   m_toolBox->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   layout->addWidget( m_toolBox );

   QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Fixed, QSizePolicy::Expanding );
   layout->addSpacerItem( spacer );
}

///////////////////////////////////////////////////////////////////////////////

QReflectionPropertyEditorComposite::~QReflectionPropertyEditorComposite()
{
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionPropertyEditorComposite::initialize( ReflectionObjectEditor* parentNode )
{
   if ( parentNode )
   {
      QReflectionObjectEditor* qEditor  = static_cast< QReflectionObjectEditor* >( parentNode );
      qEditor->addPropertyEditor( this );
   }

   __super::initialize( parentNode );
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionPropertyEditorComposite::deinitialize( ReflectionObjectEditor* parentNode )
{
   __super::deinitialize( parentNode );
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionPropertyEditorComposite::addPropertyEditor( QWidget* editorWidget )
{
   editorWidget->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   m_toolBox->addItem( editorWidget, "" );
   m_toolBox->setCurrentIndex( m_toolBox->count() - 1 );
}

///////////////////////////////////////////////////////////////////////////////

void QReflectionPropertyEditorComposite::removePropertyEditor( QWidget& editorWidget )
{
   int widgetIdx = m_toolBox->indexOf( &editorWidget );
   m_toolBox->removeItem( widgetIdx );
}

///////////////////////////////////////////////////////////////////////////////
