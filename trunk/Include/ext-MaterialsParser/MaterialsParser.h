#pragma once

#include <string>
#include "core-Renderer\Material.h"
#include "core\ResourceStorage.h"


///////////////////////////////////////////////////////////////////////////////

class GraphicalEntitiesFactory;
class MultipleElement;
class Material;

///////////////////////////////////////////////////////////////////////////////

/**
 * This class loads the material definitions from the XML file
 * and adds them to a ResourceManager instance
 */
class MaterialsParser
{
private:
   MultipleElement* m_parser;

   Material* m_parsedMaterial;

public:
   MaterialsParser(GraphicalEntitiesFactory& factory, 
                   ResourceStorage<Material>& matStorage);
   ~MaterialsParser();

   void load(const std::string& fileName);

   bool isMaterialParsed() const;
   Material& getMaterialParsed();
   void setMaterialParsed(Material& mat);
};

///////////////////////////////////////////////////////////////////////////////