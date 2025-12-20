#ifndef GLB_CLASS_HPP
#define GLB_CLASS_HPP

#include <cstdlib>
#include "sdl.hpp"
#include "types.hpp"
#include "strings.hpp"
#include "json_reader.hpp"
#include "shaders.hpp"
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/matrix_transform.hpp"
#define STB_IMAGE_IMPLEMENTATION
#include "../ext/stb_image.h"

u32 get_datatype_from_accessors_string ( const char* type ) {
  u32 result = 0xFFFFFFFF;
  
  if ( strings_are_equal ( type, "VEC2" ) ) {
    result =  ACCESSOR_VEC2;
  } else if ( strings_are_equal ( type, "VEC3" ) ) {
    result =  ACCESSOR_VEC3;
  } else if ( strings_are_equal ( type, "VEC4" ) ) {
    result =  ACCESSOR_VEC4;
  } else if ( strings_are_equal ( type, "SCALAR" ) ) {
    result =  ACCESSOR_SCALAR;
  }
  return result;
}


class Glb_Mesh_Data {
  
  private : 
    u32                 mesh_index;
    bool                has_colours   = false;
    bool                has_textures  = false;
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
    u32             vertex_datatype = 0xFFFFFFFF;
    f32*            vertex_data = NULL;
    u32             normal_byte_length;
    u32             normal_count;
    u32             normal_datatype = 0xFFFFFFFF;
    f32*            normal_data = NULL;
    u32             texcoord0_byte_length;
    u32             texcoord0_count;
    u32             texcoord0_datatype = 0xFFFFFFFF;
    f32*            texcoord0_data = NULL;
    u32             colour0_byte_length;
    u32             colour0_count   = 0;
    u32             colour_datatype = 0xFFFFFFFF;
    f32*            colour0_data = NULL;
    u32             index_byte_length;
    u32             index_count;
    u32             index_datatype  = 0xFFFFFFFF;
    u16*            index_data = NULL;
    
    u32 image_buffer_view;
    
  public : 
    
    void populate_mesh_data ( char* glb_file_binary_data_pointer ) {
      
      u32 byte_offset;
      
      vertex_byte_length  = vertex_buffer_view_data.byte_length;
      vertex_count        = vertex_accessor_data.count;
      byte_offset         = vertex_buffer_view_data.byte_offset;
      vertex_data         = ( f32* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
      
      normal_byte_length  = normal_buffer_view_data.byte_length;
      normal_count        = normal_accessor_data.count;
      byte_offset         = normal_buffer_view_data.byte_offset;
      normal_data         = ( f32* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
      
      texcoord0_byte_length = tex_coord0_buffer_view_data.byte_length;
      texcoord0_count       = tex_coord0_accessor_data.count;
      if ( texcoord0_byte_length > 0 && texcoord0_count > 0 ) {
        has_textures = true;
        byte_offset     = tex_coord0_buffer_view_data.byte_offset;
        texcoord0_data  = ( f32* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
        colour_datatype = get_datatype_from_accessors_string ( tex_coord0_accessor_data.type );
      }
      
      colour0_byte_length  = colour0_buffer_view_data.byte_length;
      colour0_count        = colour0_accessor_data.count;
      if ( colour0_count > 0 && colour0_byte_length > 0 ) {
        has_colours = true;
        byte_offset         = colour0_buffer_view_data.byte_offset;
        colour0_data         = ( f32* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
        colour_datatype     = get_datatype_from_accessors_string ( colour0_accessor_data.type );
      }
      
      index_byte_length   = index_buffer_view_data.byte_length;
      index_count         = index_accessor_data.count;
      byte_offset         = index_buffer_view_data.byte_offset;
      index_data          = ( u16* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
      
      if ( index_accessor_data.component_type != 5123 ) {
        SDL_LogInfo( SDL_LOG_CATEGORY_ERROR, "ERROR - Indices type is not u16 as expected\n" );
      }
      
      vertex_datatype = get_datatype_from_accessors_string ( vertex_accessor_data.type );
      normal_datatype = get_datatype_from_accessors_string ( normal_accessor_data.type );
      index_datatype  = get_datatype_from_accessors_string ( index_accessor_data.type );
      
    }
    
    bool mesh_has_colours() {
      return has_colours;
    }
    
    bool this_mesh_has_textures() {
      return has_textures;
    }
    
    u32 get_byte_length ( const char* type ) {
      u32 result = 0xFFFFFFFF;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = vertex_buffer_view_data.byte_length;
      } else if ( strings_are_equal ( type, "NORMAL" ) ) {
        result = normal_buffer_view_data.byte_length;
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
    
    void set_buffer_view_data ( nlohmann::json parsed_json ) {
      
      mesh_position_indices       = get_mesh_position_indices ( mesh_index, parsed_json );
      
      vertex_accessor_data        = get_accessor_data ( mesh_position_indices.vertices, parsed_json );
      normal_accessor_data        = get_accessor_data ( mesh_position_indices.normals, parsed_json );
      index_accessor_data         = get_accessor_data ( mesh_position_indices.indices, parsed_json );
      
      vertex_buffer_view_data     = get_buffer_view_data ( vertex_accessor_data.buffer_view, parsed_json );
      normal_buffer_view_data     = get_buffer_view_data ( normal_accessor_data.buffer_view, parsed_json );
      index_buffer_view_data      = get_buffer_view_data ( index_accessor_data.buffer_view, parsed_json );
      
      if ( mesh_position_indices.texcoord_0_populated ) {
        tex_coord0_accessor_data    = get_accessor_data ( mesh_position_indices.texcoord_0, parsed_json );
        tex_coord0_buffer_view_data = get_buffer_view_data ( tex_coord0_accessor_data.buffer_view, parsed_json );
        has_textures                = true;
      } else {
        has_textures = false;
      }
      
      if ( mesh_position_indices.colour_0_populated ) {
        colour0_accessor_data       = get_accessor_data ( mesh_position_indices.colour_0, parsed_json );
        colour0_buffer_view_data    = get_buffer_view_data ( colour0_accessor_data.buffer_view, parsed_json );
        has_colours = true;
      } else {
        has_colours = false;
      }
      
      
      int g = 4;
    }
    
    u32 get_binary_offset ( const char* type ) {
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
      } else if ( strings_are_equal ( type, "NORMAL" ) ) {
        result = normal_data;
      } else if ( strings_are_equal ( type, "TEXCOORD0" ) ) {
        result = texcoord0_data;
      }
      
      return result;
    }
    
    u16* get_index_data_pointer() {
      return index_data;
    }
    
    f32* get_colour0_data_pointer () {
      return colour0_data;
    }
    
    u32 get_color_accessor_data_type () {
      return colour_datatype;
    }
    
};

class Glb_imported_object {
  private :
    char*           filepath = NULL;
    u32             filesize;
    ReadFileResult  glb_file;
    GltfHeader*     gltf_header; // no need to free, points to another pointer
    char*           glb_file_binary_data_pointer; // no need to free, points to another pointer
    char*           json_header_string = NULL;
    u32             json_header_bytes;
    nlohmann::json  parsed_json;
    u32             mesh_count;
    Glb_Mesh_Data*  mesh_data_array;
    u32             index_data_total_bytes  = 0;
    u32             bin_start_offset        = 0;
    u32             total_vertex_count      = 0;
    u32             total_index_count       = 0;
    u32             texture_buffer_view     = 0;
    BufferViewData  texture_buffer_view_data;
    
    bool            has_colours     = false;
    bool            has_textures    = false;
    bool            has_any_errors  = false;
    
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
      json_header_string = init_char_star( this_json_bytes + 1 );
      pull_out_json_string( &glb_file, json_header_string, this_json_bytes );
      SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "%s\n", json_header_string );
      json_header_bytes = this_json_bytes;
      
      std::string json_str_temp ( json_header_string );
      parsed_json = nlohmann::json::parse( json_str_temp );
      
      int fadsfdasf = 19;
    }
    
    void set_totol_mesh_count () {
      u32 this_mesh_count = count_meshes ( parsed_json );
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
        mesh_data_array[ i ].set_buffer_view_data ( parsed_json);
        mesh_data_array[ i ].populate_mesh_data( glb_file_binary_data_pointer );
        
        u32 this_vertex_count = mesh_data_array[ i ].get_count( "VERTEX" );
        u32 this_index_count  = mesh_data_array[ i ].get_count( "INDEX" );
        total_vertex_count   += this_vertex_count;
        total_index_count    += this_index_count;
        
        int f = 43;
      }
    }
    
    u32 get_index_data_total_bytes () {
      
      u32 result = 0;
      
      if ( index_data_total_bytes > 0 ) {
        result =  index_data_total_bytes;
        return result;
      }
      
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 this_byte_length = mesh_data_array[ i ].get_byte_length( "INDEX" );;
        result += this_byte_length;
      }
      
      index_data_total_bytes = result;
      
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
      populate_image_data();
      // calculate_data_total_bytes( "VERTEX" );
      // calculate_data_total_bytes( "INDEX" );
      
    }
    
    u32 get_total_vertex_count() {
      return total_vertex_count;
    }
    
    u32 get_total_index_count() {
      u32 result = total_index_count;
      return result;
    }
    
    void populate_image_data () {
        texture_buffer_view = get_image_buffer_view_index ( parsed_json );
        
        if ( texture_buffer_view < 0xFFFFFFFF ) {
          
          texture_buffer_view_data  = get_buffer_view_data ( texture_buffer_view, parsed_json );
          u32 byte_offset           = texture_buffer_view_data.byte_offset;
          uchar* texture_data_raw   = ( uchar* )( ( char* ) glb_file_binary_data_pointer + byte_offset );
          
          
          // don't need to store image data so just upload straight to gl
          s32             texture_width, texture_height, texture_bpp;
          u32             image_data_bytes    = texture_buffer_view_data.byte_length;
          uchar*          texture_data        = stbi_load_from_memory ( texture_data_raw, image_data_bytes, &texture_width, &texture_height, &texture_bpp, 4 );
          
          u32 tbos;
          GLCall( glGenTextures( 1, &tbos ) );
          GLCall( glBindTexture( GL_TEXTURE_2D, tbos ) );
          GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ) );
          GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ) );
          GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ) );
          GLCall( glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ) );
          // https://stackoverflow.com/questions/23150123/loading-png-with-stb-image-for-opengl-texture-gives-wrong-colors
          GLCall( glPixelStorei( GL_UNPACK_ALIGNMENT, 1 ) );
          GLCall( glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, texture_width, texture_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, texture_data ) );
          GLCall( glBindTexture( GL_TEXTURE_2D, tbos ) );
          int dfasfdasfdas = 7;
    
        }
    }
    
    f32* get_float_data_pointer ( u32 mesh_index, const char* type ) {
      f32* result = NULL;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = mesh_data_array[ mesh_index ].get_float_data_pointer( type );
      } else if ( strings_are_equal ( type, "NORMAL" ) ) {
        result = mesh_data_array[ mesh_index ].get_float_data_pointer( type );
      } else if ( strings_are_equal ( type, "TEXCOORD0" ) ) {
        result = mesh_data_array[ mesh_index ].get_float_data_pointer( type );
      }
      
      return result;
    }
    
    u16* get_index_data_pointer ( u32 mesh_index ) {
      return mesh_data_array[ mesh_index ].get_index_data_pointer();
    }
    
    f32* get_colour0_data_pointer ( u32 mesh_index ) {
      return mesh_data_array[ mesh_index ].get_colour0_data_pointer();
    }
    
    u32 get_color_accessor_data_type ( u32 mesh_index ) {
      return mesh_data_array[ mesh_index ].get_color_accessor_data_type();
    }
    
    bool mesh_has_textures ( u32 mesh_index ) {
      return mesh_data_array[ mesh_index ].this_mesh_has_textures();
    }
};

#endif //GLB_CLASS_HPP
