#ifndef ENTITY_CLASS_HPP
#define ENTITY_CLASS_HPP

#include "glb_class.hpp"
#include "types.hpp"

// define the size of a vertex for upload into gl buffer
const u32 VERTEX_BYTES =
  ( sizeof( f32 ) * 4 ) // four float for position ... x, y, z, w
+ ( sizeof( f32 ) * 3 ) // three values for normals
+ ( sizeof( f32 ) * 2 ) // two uv texture coordinates
+ ( sizeof( f32 ) * 4 ) // four floats for colour ... r, g, b, a
;

struct Mesh {
  u32             vertex_byte_length;
  u32             vertex_count;
  f32*            vertex_data;
  u32             index_byte_length;
  u32             index_count;
  u16*            index_data;
  u32             vertex_offset_in_gl_buffer_in_bytes;
  u32             index_offset_in_gl_buffer_in_bytes;
};

class Entity_Class {
  private :
    u32             mesh_count = 0xFFFFFFFF;
    Mesh*           mesh_array;
    
  public : 
    
    // constructor - transfer data across from the glb object
    Entity_Class( Glb_imported_object glb_imported_object ) {
      
      mesh_count = glb_imported_object.get_total_mesh_count();
      mesh_array = ( Mesh* ) malloc ( ( size_t )( sizeof( Mesh ) * mesh_count ) );
      
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 this_vertex_count = glb_imported_object.get_mesh_element_count( i, "VERTEX" );
        u32 this_index_count  = glb_imported_object.get_mesh_element_count( i, "INDEX" );
        
        mesh_array[ i ].vertex_byte_length  = glb_imported_object.get_mesh_byte_length( i, "VERTEX" );
        mesh_array[ i ].index_byte_length   = glb_imported_object.get_mesh_byte_length( i, "INDEX" );
        
        mesh_array[ i ].vertex_count        = this_vertex_count;
        mesh_array[ i ].index_count         = glb_imported_object.get_mesh_element_count( i, "INDEX" );
        
        u32 vertex_data_bytes               = VERTEX_BYTES * this_vertex_count;
        u32 index_data_bytes                = sizeof( u16 ) * this_index_count;
        
        mesh_array[ i ].vertex_data = ( f32* ) malloc ( ( size_t ) vertex_data_bytes );
        mesh_array[ i ].index_data  = ( u16* ) malloc ( ( size_t ) index_data_bytes );
        f32* dst_f32 = mesh_array[ i ].vertex_data;
        
        f32* glb_vertex_data = glb_imported_object.get_float_data_pointer( i, "VERTEX" );
        
        u32 current_count_loaded = 0;
        
        for ( u32 j = 0; j < this_vertex_count; j++ ) {
          *dst_f32++ = *glb_vertex_data++; // x
          *dst_f32++ = *glb_vertex_data++; // y
          *dst_f32++ = *glb_vertex_data++; // z
          *dst_f32++ = 1.0f; // w
          *dst_f32++ = 0.0f; // normal x
          *dst_f32++ = 0.0f; // normal y
          *dst_f32++ = 0.0f; // normal z
          *dst_f32++ = 0.0f; // u
          *dst_f32++ = 0.0f; // v
          *dst_f32++ = 1.0f; // r
          *dst_f32++ = 1.0f; // g
          *dst_f32++ = 1.0f; // b
          *dst_f32++ = 1.0f; // a
        }
        
        u16* glb_index_data = glb_imported_object.get_index_data_pointer( i );
        u16* dst_u16 = mesh_array[ i ].index_data;
        memcpy ( dst_u16, glb_index_data, index_data_bytes );
        
        
      }
      
      int dafsdasfdsfdas = 87;
    }
    
    u32 get_mesh_count() {
      return mesh_count;
    }
    
    u32 get_index_count( u32 mesh_index ) {
      return mesh_array[ mesh_index ].index_count;
    }
    
};

#endif //ENTITY_CLASS_HPP
