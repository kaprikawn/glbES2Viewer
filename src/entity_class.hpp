#ifndef ENTITY_CLASS_HPP
#define ENTITY_CLASS_HPP

#include "glb_class.hpp"
#include "types.hpp"

struct Mesh {
  u32             vertex_byte_length;
  u32             vertex_count;
  f32*            vertex_data;
  u32             colour_byte_length = 0;
  u32             colour_count = 0;
  f32*            colour_data = NULL;
  u32             index_byte_length;
  u32             index_count;
  u16*            index_data;
  u32             vertex_offset_in_gl_buffer_in_bytes;
  u32             index_offset_in_gl_buffer_in_bytes;
};

f32 normalize_colour ( u8 value ) {
  if ( value == 255 ) {
    return 1.0f;
  } else if ( value == 0 ) {
    return 0.0f;
  }
  return value * ( 1.0f / 255.0f ); 
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
        
        f32* glb_vertex_data    = glb_imported_object.get_float_data_pointer( i, "VERTEX" );
        f32* glb_normal_data    = glb_imported_object.get_float_data_pointer( i, "NORMAL" );
        f32* glb_texcoord0_data = NULL;
        if ( glb_imported_object.mesh_has_textures( i ) ) {
          glb_texcoord0_data = glb_imported_object.get_float_data_pointer( i, "TEXCOORD0" );
        }
        f32* glb_colour0_data = NULL;
        u32 color_accessor_data_type;
        
        bool vertices_have_colours = glb_imported_object.glb_has_colours();
        if ( vertices_have_colours ) {
          glb_colour0_data = glb_imported_object.get_colour0_data_pointer( i );
          
          color_accessor_data_type = glb_imported_object.get_color_accessor_data_type ( i );
          
        }
        
        u32 current_count_loaded = 0;
        
        for ( u32 j = 0; j < this_vertex_count; j++ ) {
          *dst_f32++ = *glb_vertex_data++; // x
          *dst_f32++ = *glb_vertex_data++; // y
          *dst_f32++ = *glb_vertex_data++; // z
          *dst_f32++ = 1.0f; // w
          *dst_f32++ = *glb_normal_data++; // normal x
          *dst_f32++ = *glb_normal_data++; // normal y
          *dst_f32++ = *glb_normal_data++; // normal z
          
          if ( glb_texcoord0_data == NULL ) {
            *dst_f32++ = 0.0f; // u
            *dst_f32++ = 0.0f; // v
          } else {
            *dst_f32++ = *glb_texcoord0_data++; // u
            *dst_f32++ = *glb_texcoord0_data++; // v
          }
          
          if ( vertices_have_colours ) {
            
            *dst_f32++ = *glb_colour0_data++; // r
            *dst_f32++ = *glb_colour0_data++; // g
            *dst_f32++ = *glb_colour0_data++; // b
            if ( color_accessor_data_type == ACCESSOR_VEC3 ) {
              *dst_f32++ = 1.0f; // a
            } else if ( color_accessor_data_type == ACCESSOR_VEC4 ) {
              *dst_f32++ = 1.0f; // a
              glb_colour0_data++;
            } else {
              SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "ERROR - Unexpected colour datatype\n" );
            }
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
          GLintptr        offset          = ( GLintptr )      current_gl_array_buffer_offset;
          GLsizeiptr      size            = ( GLsizeiptr )    vertex_data_bytes;
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
    
    u32 get_vertex_offset_in_gl ( u32 mesh_index, const char* type ) {
      u32 result = 0;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = mesh_array[ mesh_index ].vertex_offset_in_gl_buffer_in_bytes;
      } else if ( strings_are_equal ( type, "NORMAL" ) ) {
        result = mesh_array[ mesh_index ].vertex_offset_in_gl_buffer_in_bytes;
        u32 bytes_before_colours = u32 ( sizeof( f32 ) * 4 ); // there are four floats in the vertex before you get to normals data so we need to offset by that
        result += bytes_before_colours;
      } else if ( strings_are_equal ( type, "COLOR0" ) ) {
        result = mesh_array[ mesh_index ].vertex_offset_in_gl_buffer_in_bytes;
        u32 bytes_before_colours = u32 ( sizeof( f32 ) * 9 ); // there are nine floats in the vertex before you get to colour data so we need to offset by that
        result += bytes_before_colours;
      }
      return result;
    }
    
};

#endif //ENTITY_CLASS_HPP
