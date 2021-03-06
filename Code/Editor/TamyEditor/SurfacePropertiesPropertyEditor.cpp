#include "SurfacePropertiesPropertyEditor.h"
#include "core\Color.h"
#include <QBoxLayout.h>
#include <QLabel.h>
#include <QSpinBox.h>
#include "ColorFrame.h"


///////////////////////////////////////////////////////////////////////////////

SurfacePropertiesPropertyEditor::SurfacePropertiesPropertyEditor( TEditableReflectionProperty< SurfaceProperties >* property )
: QPropertyEditor( property->getLabel().c_str() )
, m_property( property )
{
   setupUi();
}

///////////////////////////////////////////////////////////////////////////////

SurfacePropertiesPropertyEditor::~SurfacePropertiesPropertyEditor()
{
   delete m_property;
   m_property = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::setupUi()
{
   QFrame* frame = new QFrame( this ); addWidget( frame );
   QVBoxLayout* layout = new QVBoxLayout( frame );
   layout->setSpacing( 0 );
   layout->setMargin( 0 );

   const SurfaceProperties& val = m_property->get();
   {
      QFrame* ambientFrame = new QFrame( this ); layout->addWidget( ambientFrame );
      QHBoxLayout* ambientLayout = new QHBoxLayout( ambientFrame );
      ambientLayout->setSpacing( 0 );
      ambientLayout->setMargin( 0 );

      ambientLayout->addWidget( new QLabel( "Ambient:", this ) );
      m_ambientVal = new ColorFrame( ambientFrame, val.getAmbientColor() ); 
      ambientLayout->addWidget( m_ambientVal );
      connect( m_ambientVal, SIGNAL( changed( const QColor& ) ), this, SLOT( ambientChanged( const QColor& ) ) );
   }

   {
      QFrame* diffuseFrame = new QFrame( this ); layout->addWidget( diffuseFrame );
      QHBoxLayout* diffuseLayout = new QHBoxLayout( diffuseFrame );
      diffuseLayout->setSpacing( 0 );
      diffuseLayout->setMargin( 0 );

      diffuseLayout->addWidget( new QLabel( "Diffuse:", this ) );
      m_diffuseVal = new ColorFrame( diffuseFrame, val.getDiffuseColor() ); 
      diffuseLayout->addWidget( m_diffuseVal );
      connect( m_diffuseVal, SIGNAL( changed( const QColor& ) ), this, SLOT( diffuseChanged( const QColor& ) ) );
   }

   {
      QFrame* emissiveFrame = new QFrame( this ); layout->addWidget( emissiveFrame );
      QHBoxLayout* emissiveLayout = new QHBoxLayout( emissiveFrame );
      emissiveLayout->setSpacing( 0 );
      emissiveLayout->setMargin( 0 );

      emissiveLayout->addWidget( new QLabel( "Emissive:", this ) );
      m_emissiveVal = new ColorFrame( emissiveFrame, val.getEmissiveColor() ); 
      emissiveLayout->addWidget( m_emissiveVal );
      connect( m_emissiveVal, SIGNAL( changed( const QColor& ) ), this, SLOT( emissiveChanged( const QColor& ) ) );
   }

   {
      QFrame* specularFrame = new QFrame( this ); layout->addWidget( specularFrame );
      QHBoxLayout* specularLayout = new QHBoxLayout( specularFrame );
      specularLayout->setSpacing( 0 );
      specularLayout->setMargin( 0 );

      specularLayout->addWidget( new QLabel( "Specular:", this ) );
      m_specularVal = new ColorFrame( specularFrame, val.getSpecularColor() ); 
      specularLayout->addWidget( m_specularVal );
      connect( m_specularVal, SIGNAL( changed( const QColor& ) ), this, SLOT( specularChanged( const QColor& ) ) );
   }

   {
      QFrame* powerFrame = new QFrame( this ); layout->addWidget( powerFrame );
      QHBoxLayout* powerLayout = new QHBoxLayout( powerFrame );
      powerLayout->setSpacing( 0 );
      powerLayout->setMargin( 0 );

      powerLayout->addWidget( new QLabel( "Specular power:", this ) );
      m_powerVal = new QDoubleSpinBox( this ); 
      powerLayout->addWidget( m_powerVal );
      m_powerVal->setValue( val.getPower() );
      connect( m_powerVal, SIGNAL( valueChanged( double ) ), this, SLOT( powerChanged( double ) ) );
   }

}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::powerChanged( double val )
{
   SurfaceProperties mat = m_property->get();
   mat.setPower( val );
   m_property->set( mat );
}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::ambientChanged( const QColor& color )
{
   SurfaceProperties mat = m_property->get();
   mat.setAmbientColor( Color( color.redF(), color.greenF(), color.blueF(), color.alphaF() ) );
   m_property->set( mat );
}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::diffuseChanged( const QColor& color )
{
   SurfaceProperties mat = m_property->get();
   mat.setDiffuseColor( Color( color.redF(), color.greenF(), color.blueF(), color.alphaF() ) );
   m_property->set( mat );
}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::specularChanged( const QColor& color )
{
   SurfaceProperties mat = m_property->get();
   mat.setSpecularColor( Color( color.redF(), color.greenF(), color.blueF(), color.alphaF() ) );
   m_property->set( mat );
}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::emissiveChanged( const QColor& color )
{
   SurfaceProperties mat = m_property->get();
   mat.setEmissiveColor( Color( color.redF(), color.greenF(), color.blueF(), color.alphaF() ) );
   m_property->set( mat );
}

///////////////////////////////////////////////////////////////////////////////

void SurfacePropertiesPropertyEditor::onPropertyChanged()
{
   const SurfaceProperties& val = m_property->get();

   m_ambientVal->setColor( val.getAmbientColor() );
   m_diffuseVal->setColor( val.getDiffuseColor() );
   m_emissiveVal->setColor( val.getEmissiveColor() );
   m_specularVal->setColor( val.getSpecularColor() );
   m_powerVal->setValue( val.getPower() );

}

///////////////////////////////////////////////////////////////////////////////
