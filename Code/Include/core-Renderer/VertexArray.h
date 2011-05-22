#pragma once

/// @file   core-Renderer\VertexArray.h
/// @brief  generic wrapper around an array of non-polymorphic vertices

#include <vector>
#include <windows.h>


///////////////////////////////////////////////////////////////////////////////

/**
 * This generic array can wrap different types of vertex types without them
 * having to be polymorphic.
 */
class VertexArray
{
public:
   virtual ~VertexArray() {}

   /**
    * Returns the size of the wrapped vertex structure.
    */
   virtual std::size_t getVtxSize() const = 0;

   /**
    * Returns the FVF vertex specification.
    *
    * @return  vertex FVF
    */
   virtual DWORD getFVF() const = 0;

   /**
    * Returns the number of vertices in the array
    */
   virtual unsigned int size() const = 0;

   /**
    * Copies the vertices to the specified buffer.
    *
    * @param dest    buffer to which the vertices should be copied.
    *                It should be capable to absorb all the vertices.
    */
   virtual void copyTo(void* dest) = 0;
};

///////////////////////////////////////////////////////////////////////////////

/**
 * This type specific wrapper specifies the type of wrapped vertices.
 */
template <typename T>
class TVertexArray : public VertexArray
{
private:
   std::vector<T> m_vertices;

public:
   std::size_t getVtxSize() const {return sizeof(T);}

   DWORD getFVF() const {return T::FVF;}

   unsigned int size() const {return m_vertices.size();}

   void copyTo(void* dest)
   {
      T* typedDest = (T*)(dest);

      unsigned int count = m_vertices.size();
      for (unsigned int i = 0; i < count; ++i)
      {
         *typedDest++ = m_vertices[i];
      }
   }

   void add(const T& vtx) {m_vertices.push_back(vtx);}
};

///////////////////////////////////////////////////////////////////////////////