#include "EnumPropertyEditor.h"
#include <QComboBox>


///////////////////////////////////////////////////////////////////////////////

EnumPropertyEditor::EnumPropertyEditor( TEditableReflectionProperty< ReflectionEnum >* property )
   : QPropertyEditor( property->getLabel().c_str() )
   , m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

EnumPropertyEditor::~EnumPropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void EnumPropertyEditor::setupUi()
{
   // create the edit widget
   QComboBox* edit = new QComboBox( this );
   addWidget( edit );

   // fill the widget with initial data
   std::vector< std::string > enumerators;
   const ReflectionEnum& enumType = static_cast< const ReflectionEnum& >( m_property->getType() );

   enumType.getEnumerators( enumerators );
   for ( std::vector< std::string >::const_iterator it = enumerators.begin(); it != enumerators.end(); ++it )
   {
      edit->addItem( it->c_str() );
   }

   // select the currently held value
   int val = reinterpret_cast< const int& >( m_property->get() );
   unsigned  int idx = enumType.getIndex( val );
   edit->setCurrentIndex( idx );
   
   connect( edit, SIGNAL( currentIndexChanged( const QString& ) ), this, SLOT( valChanged( const QString& ) ) );
}

///////////////////////////////////////////////////////////////////////////////

void EnumPropertyEditor::valChanged( const QString& val )
{
   std::string newVal = val.toStdString();
   
   const ReflectionEnum& enumType = static_cast< const ReflectionEnum& >( m_property->getType() );
   int newEnumVal = enumType.getValue( newVal );
   m_property->set( reinterpret_cast< const ReflectionEnum& >( newEnumVal ) );
}

///////////////////////////////////////////////////////////////////////////////
