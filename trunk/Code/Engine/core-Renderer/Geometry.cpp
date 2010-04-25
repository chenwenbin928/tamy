#include "core-Renderer\Geometry.h"
#include "core-Renderer\GeometryResource.h"
#include "core\BoundingVolume.h"


///////////////////////////////////////////////////////////////////////////////

BEGIN_OBJECT(Geometry, Entity)
END_OBJECT()

///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry()
: m_resource(NULL)
, m_vol(NULL)
{
}

///////////////////////////////////////////////////////////////////////////////

Geometry::Geometry(GeometryResource& resource)
: m_resource(&resource)
{
   m_vol = m_resource->calculateBoundingVolume();
}

///////////////////////////////////////////////////////////////////////////////

Geometry::~Geometry()
{
   delete m_vol; m_vol = NULL;
   m_resource = NULL;
}

///////////////////////////////////////////////////////////////////////////////

void Geometry::render()
{
   m_resource->render();
}

///////////////////////////////////////////////////////////////////////////////

const BoundingVolume& Geometry::calculateBoundingVolume() const
{
   ASSERT(m_vol != NULL, "Geometry entity is not initialized");
   return *m_vol;
}

///////////////////////////////////////////////////////////////////////////////
