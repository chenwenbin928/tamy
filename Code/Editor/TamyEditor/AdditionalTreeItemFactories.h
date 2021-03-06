/// @file   TamyEditor/AdditionalTreeItemFactories.h
/// @brief  additional factories that create tree items
#pragma once

#include "TreeWidgetDescFactory.h"
#include <QString>


///////////////////////////////////////////////////////////////////////////////

/**
 * A wrapper factory that can create directories along the types the wrapped factory can create
 */
class DirectoriesFactoryWrapper : public TreeWidgetDescFactory
{
private:
   TreeWidgetDescFactory*     m_wrappedFactory;
   QString                    m_iconsDir;

public:
   /**
    * Constructor.
    *
    * @param wrappedFactory
    * @param iconsDir
    */ 
   DirectoriesFactoryWrapper( TreeWidgetDescFactory* wrappedFactory, const QString& iconsDir );
   ~DirectoriesFactoryWrapper();

   // -------------------------------------------------------------------------
   // TreeWidgetDescFactory implementation
   // -------------------------------------------------------------------------
   unsigned int typesCount() const;
   void getDesc( unsigned int idx, QString& outDesc, QString& outGroup, QIcon& outIcon ) const;
   void getDesc( const SerializableReflectionType& type, QString& outDesc, QString& outGroup, QIcon& outIcon ) const;
   const SerializableReflectionType* getClass( unsigned int idx ) const;

};

///////////////////////////////////////////////////////////////////////////////
