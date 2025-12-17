#ifndef ENTITY_CLASS_HPP
#define ENTITY_CLASS_HPP

#include "glb_class.hpp"
#include "types.hpp"

struct Mesh {
  u32             vertex_byte_length;
  u32             vertex_count;
  f32*            vertex_data;
  u32             color_byte_length = 0;
  u32             color_count = 0;
  f32*            color_data = NULL;
  u32             index_byte_length;
  u32             index_count;
  u16*            index_data;
  u32             vertex_offset_in_gl_buffer_in_bytes;
  u32             index_offset_in_gl_buffer_in_bytes;
};

f32 normalize_color ( u8 value ) {
  if ( value == 255 ) {
    return 1.0f;
  } else if ( value == 0 ) {
    return 0.0f;
  }
  return value * (1.0f / 255.0f); 
}

class Entity_Class {
  private :
    u32             mesh_count = 0xFFFFFFFF;
    Mesh*           mesh_array;
    
  public : 
    
    // constructor - transfer data across from the glb object
    Entity_Class( Glb_imported_object glb_imported_object ) {
      
      u32 current_gl_array_buffer_offset          = 0;
      u32 current_gl_element_array_buffer_offset  = 0;
      
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
        u8*  glb_color0_data = NULL;
        
        bool vertices_have_colours = glb_imported_object.glb_has_colours();
        if ( vertices_have_colours ) {
          glb_color0_data = glb_imported_object.get_color0_data_pointer( i );
        }
        
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
          
          if ( vertices_have_colours ) {
            u8 this_color;
            this_color = *glb_color0_data++;
            *dst_f32++ = normalize_color( this_color ); // r
            this_color = *glb_color0_data++;
            *dst_f32++ = normalize_color( this_color ); // g
            this_color = *glb_color0_data++;
            *dst_f32++ = normalize_color( this_color ); // b
            this_color = *glb_color0_data++;
            this_color = 1.0f;
            *dst_f32++ = normalize_color( this_color ); // a
          } else {
            *dst_f32++ = 1.0f; // r
            *dst_f32++ = 1.0f; // g
            *dst_f32++ = 1.0f; // b
            *dst_f32++ = 1.0f; // a
          }
        }
        
        u16* glb_index_data = glb_imported_object.get_index_data_pointer( i );
        u16* dst_u16 = mesh_array[ i ].index_data;
        memcpy ( dst_u16, glb_index_data, index_data_bytes );
        
        mesh_array[ i ].vertex_offset_in_gl_buffer_in_bytes = current_gl_array_buffer_offset;
        mesh_array[ i ].index_offset_in_gl_buffer_in_bytes  = current_gl_element_array_buffer_offset;
        
        // vertex data
        {
          GLenum          target          = GL_ARRAY_BUFFER;
          GLintptr        offset          = ( GLintptr ) current_gl_array_buffer_offset;
          GLsizeiptr      size            = ( GLsizeiptr ) vertex_data_bytes;
          const GLvoid *  data            = ( const GLvoid* ) mesh_array[ i ].vertex_data;
          GLCall( glBufferSubData( target, offset, size, data ) );
        }
        
         // index data
        {
          GLenum          target          = GL_ELEMENT_ARRAY_BUFFER;
          GLintptr        offset          = current_gl_element_array_buffer_offset;
          GLsizeiptr      size            = index_data_bytes;
          const void*     data            = ( const GLvoid* ) mesh_array[ i ].index_data;
          
          GLCall( glBufferSubData( target, offset, size, data ) );
        }
        
        current_gl_array_buffer_offset          += vertex_data_bytes;
        current_gl_element_array_buffer_offset  += index_data_bytes;
      }
      
      int dafsdasfdsfdas = 87;
    }
    
    u32 get_mesh_count() {
      return mesh_count;
    }
    
    u32 get_index_count( u32 mesh_index ) {
      return mesh_array[ mesh_index ].index_count;
    }
    
    u32 get_index_offset_in_gl ( u32 mesh_index ) {
      return mesh_array[ mesh_index ].index_offset_in_gl_buffer_in_bytes;
    }
    
    u32 get_vertex_offset_in_gl ( u32 mesh_index ) {
      return mesh_array[ mesh_index ].vertex_offset_in_gl_buffer_in_bytes;
    }
    
};

#endif //ENTITY_CLASS_HPP
