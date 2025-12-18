#ifndef GLB_CLASS_HPP
#define GLB_CLASS_HPP

#include <cstdlib>
#include "sdl.hpp"
#include "types.hpp"
#include "strings.hpp"
#include "json.hpp"
#include "shaders.hpp"
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/matrix_transform.hpp"

class Glb_Mesh_Data {
  
  private : 
    u32                 mesh_index;
    bool                has_colours = false;
    GltfBufferViewInfo  gltf_buffer_view_info;
    MeshPositionIndices mesh_position_indices;
    
    AccessorData    vertex_accessor_data;
    AccessorData    normal_accessor_data;
    AccessorData    index_accessor_data;
    AccessorData    tex_coord0_accessor_data;
    AccessorData    colour0_accessor_data;
    BufferViewData  vertex_buffer_view_data;
    BufferViewData  normal_buffer_view_data;
    BufferViewData  index_buffer_view_data;
    BufferViewData  tex_coord0_buffer_view_data;
    BufferViewData  colour0_buffer_view_data;
    
    u32             vertex_byte_length;
    u32             vertex_count;
    f32*            vertex_data;
    u32             colour0_byte_length;
    u32             colour0_count = 0;
    u8*             colour0_data;
    u32             index_byte_length;
    u32             index_count;
    u16*            index_data;
    
  public : 
    
    void populate_mesh_data ( char* glb_file_binary_data_pointer ) {
      
      u32 byte_offset;
      
      vertex_byte_length  = vertex_buffer_view_data.byte_length;
      vertex_count        = vertex_accessor_data.count;
      byte_offset         = vertex_buffer_view_data.byte_offset;
      vertex_data         = ( f32* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
      
      colour0_byte_length  = colour0_buffer_view_data.byte_length;
      colour0_count        = colour0_accessor_data.count;
      if ( colour0_count > 0 && colour0_byte_length > 0 ) {
        has_colours = true;
      }
      
      if( has_colours ) {
        byte_offset         = colour0_buffer_view_data.byte_offset;
        colour0_data         = ( u8* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
      }
      
      index_byte_length   = index_buffer_view_data.byte_length;
      index_count         = index_accessor_data.count;
      byte_offset         = index_buffer_view_data.byte_offset;
      index_data          = ( u16* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
      
      if ( index_accessor_data.component_type != 5123 ) {
        SDL_LogInfo( SDL_LOG_CATEGORY_ERROR, "ERROR - Indices type is not u16 as expected\n" );
      }
      
    }
    
    bool mesh_has_colours() {
      return has_colours;
    }
    
    u32 get_byte_length ( const char* type ) {
      u32 result = 0xFFFFFFFF;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = vertex_buffer_view_data.byte_length;
      } else if ( strings_are_equal ( type, "INDEX" ) ) {
        result = index_buffer_view_data.byte_length;
      }
      
      return result;
    }
    
    void set_mesh_index ( u32 mesh_index_value ) {
      mesh_index = mesh_index_value;
    }
    
    u32 get_count ( const char* type ) {
      u32 result = 0;
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = vertex_accessor_data.count;
      } else if ( strings_are_equal ( type, "INDEX" ) ) {
        result = index_accessor_data.count;
      }
      return result;
    } 
    
    void set_buffer_view_data ( const char* json_string, u32 json_char_count ) {
      
      mesh_position_indices       = get_mesh_position_indices ( mesh_index, json_string, json_char_count );
      
      vertex_accessor_data        = get_accessor_data ( mesh_position_indices.vertices, json_string, json_char_count );
      normal_accessor_data        = get_accessor_data ( mesh_position_indices.normals, json_string, json_char_count );
      index_accessor_data         = get_accessor_data ( mesh_position_indices.indices, json_string, json_char_count );
      tex_coord0_accessor_data    = get_accessor_data ( mesh_position_indices.texcoord_0, json_string, json_char_count );
      colour0_accessor_data        = get_accessor_data ( mesh_position_indices.colour_0, json_string, json_char_count );
      
      vertex_buffer_view_data     = get_buffer_view_data ( vertex_accessor_data.buffer_view, json_string, json_char_count );
      normal_buffer_view_data     = get_buffer_view_data ( normal_accessor_data.buffer_view, json_string, json_char_count );
      index_buffer_view_data      = get_buffer_view_data ( index_accessor_data.buffer_view, json_string, json_char_count );
      tex_coord0_buffer_view_data = get_buffer_view_data ( tex_coord0_accessor_data.buffer_view, json_string, json_char_count );
      colour0_buffer_view_data     = get_buffer_view_data ( colour0_accessor_data.buffer_view, json_string, json_char_count );
      
      int g = 4;
    }
    
    u32 get_binary_offset( const char* type ) {
      u32 result = 0;
      if ( strings_are_equal( type, "VERTEX" ) ) {
        result = vertex_buffer_view_data.byte_offset;
      } else if ( strings_are_equal( type, "INDEX" ) ) {
        result = index_buffer_view_data.byte_offset;
      }
      return result;
    }
    
    f32* get_float_data_pointer ( const char* type ) {
      f32* result = NULL;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = vertex_data;
      }
      
      return result;
    }
    
    u16* get_index_data_pointer() {
      return index_data;
    }
    
    u8* get_colour0_data_pointer () {
      return colour0_data;
    }
    
};

enum GL_COMPONENT_TYPE {
    GL_COMPONENT_TYPE_SIGNED_BYTE    = 5120 // 8 bits
  , GL_COMPONENT_TYPE_UNSIGNED_BYTE  = 5121 // 8 bits
  , GL_COMPONENT_TYPE_SIGNED_SHORT   = 5122 // 16 bits
  , GL_COMPONENT_TYPE_UNSIGNED_SHORT = 5123 // 16 bits
  , GL_COMPONENT_TYPE_UNSIGNED_INT   = 5125 // 32 bits
  , GL_COMPONENT_TYPE_FLOAT          = 5126 // Signed 32 bits
};

class Glb_imported_object {
  private :
    char*           filepath = NULL;
    u32             filesize;
    ReadFileResult  glb_file;
    GltfHeader*     gltf_header; // no need to free, points to another pointer
    char*           glb_file_binary_data_pointer; // no need to free, points to another pointer
    char*           json = NULL;
    u32             json_bytes;
    u32             mesh_count;
    Glb_Mesh_Data*  mesh_data_array;
    u32             vertex_data_total_bytes = 0;
    u32             index_data_total_bytes  = 0;
    u32             bin_start_offset        = 0;
    u32             total_vertex_count      = 0;
    u32             total_index_count       = 0;
    
    bool            has_colours = false;
    bool            has_any_errors = false;
    
  public :
    
    // constructor
    Glb_imported_object ( const char* filepath_in, u32 filepath_length_in = 0 ) {
      
      u32 filepath_length = filepath_length_in;
      
      if ( filepath_length_in <= 0 ) {
        filepath_length = string_length( filepath_in );
      }
      
      filepath = init_char_star ( filepath_length + 1 );
      
      copy_string_into_char_star ( filepath_in, filepath, filepath_length );
      
      // @TODO : check if file exists
      
    }
    
    bool glb_has_colours() {
      return mesh_data_array[0].mesh_has_colours();
    }
    
    void read_file_offsets () {
      
      gltf_header = ( GltfHeader* ) glb_file.contents;
      
      u32 gltf_header_size_in_bytes = 12; // magic + version + length
      u32 json_header_in_bytes      = 8; // 4 bytes for chunk length, 4 bytes for chunk type
      u32 json_string_in_bytes      = gltf_header -> json_chunk_length;
      u32 bin_header_in_bytes       = 8; // 4 bytes for chunk length, 4 bytes for chunk type
      bin_start_offset              = gltf_header_size_in_bytes + json_header_in_bytes + json_string_in_bytes + bin_header_in_bytes;
      
      glb_file_binary_data_pointer  = ( char* )( ( char* ) glb_file.contents + bin_start_offset );
      
    }
    
    void update_json() {
      u32 this_json_bytes = json_size_in_bytes( &glb_file );
      json = init_char_star( this_json_bytes + 1 );
      pull_out_json_string( &glb_file, json, this_json_bytes );
      json_bytes = this_json_bytes;
      int fadsfdasf = 19;
    }
    
    void set_totol_mesh_count () {
      u32 this_mesh_count = count_meshes ( json, json_bytes );
      mesh_count = this_mesh_count;
    }
    
    u32 get_total_mesh_count () {
      return mesh_count;
    }
    
    u32 get_mesh_byte_length ( u32 mesh_index, const char* type ) {
      u32 result = 0xFFFFFFFF;
      result = mesh_data_array[ mesh_index ].get_byte_length( type );
      return result;
    }
    
    u32 get_mesh_element_count( u32 mesh_index, const char* type ) {
      u32 result = 0xFFFFFFFF;
      result = mesh_data_array[ mesh_index ].get_count( type );
      return result;
    }
    
    void populate_mesh_data() {
      
      size_t bytes = size_t ( mesh_count * sizeof( Glb_Mesh_Data ) );
      mesh_data_array = ( Glb_Mesh_Data* ) malloc ( bytes );
      
      total_vertex_count  = 0;
      total_index_count   = 0;
      
      for ( u32 i = 0; i < mesh_count; i++ ) {
        mesh_data_array[ i ].set_mesh_index ( i );
        mesh_data_array[ i ].set_buffer_view_data ( json, json_bytes );
        mesh_data_array[ i ].populate_mesh_data( glb_file_binary_data_pointer );
        
        u32 this_vertex_count = mesh_data_array[ i ].get_count( "VERTEX" );
        u32 this_index_count  = mesh_data_array[ i ].get_count( "INDEX" );
        total_vertex_count   += this_vertex_count;
        total_index_count    += this_index_count;
        
        int f = 43;
      }
    }
    
    void calculate_data_total_bytes ( const char* type ) {
      u32 result = 0;
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 this_byte_length = 0;
        if ( strings_are_equal ( type, "VERTEX" ) ) {
          this_byte_length = mesh_data_array[ i ].get_byte_length( "VERTEX" );
        } else if ( strings_are_equal ( type, "INDEX" ) ) {
          this_byte_length = mesh_data_array[ i ].get_byte_length( "INDEX" );
        }
        
        result += this_byte_length;
        if ( strings_are_equal ( type, "VERTEX" ) ) {
          vertex_data_total_bytes = result;
        } else if ( strings_are_equal ( type, "INDEX" ) ) {
          index_data_total_bytes = result;
        }
      }
    }
    
    u32 get_data_total_bytes ( const char* type ) {
      
      u32 result = 0;
      if ( strings_are_equal ( "VERTEX", type ) ) {
        result = vertex_data_total_bytes;
      } else if ( strings_are_equal ( "INDEX", type ) ) {
        result = index_data_total_bytes;
      }
      return result;
    }
    
    bool is_glb_file () {
      bool result = false;
      
      if ( gltf_header -> magic == 1179937895 ) {
        result = true;
      }
      // @TODO : add error message
      return result;
    }
    
    void import_glb_file() {
      
      glb_file = read_entire_file ( filepath );
      read_file_offsets();
      
      if ( !is_glb_file() ) {
        SDL_LogInfo( SDL_LOG_CATEGORY_ERROR, "ERROR not glb file\n" );
      }
      
      update_json();
      set_totol_mesh_count();
      populate_mesh_data();
      calculate_data_total_bytes( "VERTEX" );
      calculate_data_total_bytes( "INDEX" );
      
    }
    
    u32 get_total_vertex_count() {
      return total_vertex_count;
    }
    
    u32 get_total_index_count() {
      u32 result = total_index_count;
      return result;
    }
    
    f32* get_float_data_pointer ( u32 mesh_index, const char* type ) {
      f32* result = NULL;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = mesh_data_array[ mesh_index ].get_float_data_pointer( type );
      }
      
      return result;
    }
    
    u16* get_index_data_pointer ( u32 mesh_index ) {
      return mesh_data_array[ mesh_index ].get_index_data_pointer();
    }
    
    u8* get_colour0_data_pointer ( u32 mesh_index ) {
      return mesh_data_array[ mesh_index ].get_colour0_data_pointer();
    }
};

#endif //GLB_CLASS_HPP
