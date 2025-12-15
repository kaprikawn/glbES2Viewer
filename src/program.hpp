#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <cstdlib>
#include "sdl.hpp"
#include "types.hpp"
#include "strings.hpp"
#include "json.hpp"
#include "shaders.hpp"
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/matrix_transform.hpp"

class Mesh_Data {
  private : 
    u32 mesh_index;
    GltfBufferViewInfo gltf_buffer_view_info;
    MeshPositionIndices mesh_position_indices;
    
    AccessorData    vertex_accessor_data;
    AccessorData    normal_accessor_data;
    AccessorData    index_accessor_data;
    AccessorData    tex_coord0_accessor_data;
    BufferViewData  vertex_buffer_view_data;
    BufferViewData  normal_buffer_view_data;
    BufferViewData  index_buffer_view_data;
    BufferViewData  tex_coord0_buffer_view_data;
    
  public : 
    
    void set_mesh_index ( u32 mesh_index_value ) {
      mesh_index = mesh_index_value;
    }
    
    u32 get_byte_length ( const char* type ) {
      u32 result = 0;
      if ( strings_are_equal( "VERTEX", type ) ) {
        result = vertex_buffer_view_data.byte_length;
      } else if ( strings_are_equal( "INDEX", type ) ) {
        result = index_buffer_view_data.byte_length;
      }
      return result;
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
      
      vertex_buffer_view_data     = get_buffer_view_data ( vertex_accessor_data.buffer_view, json_string, json_char_count );
      normal_buffer_view_data     = get_buffer_view_data ( normal_accessor_data.buffer_view, json_string, json_char_count );
      index_buffer_view_data      = get_buffer_view_data ( index_accessor_data.buffer_view, json_string, json_char_count );
      tex_coord0_buffer_view_data = get_buffer_view_data ( tex_coord0_accessor_data.buffer_view, json_string, json_char_count );
      
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
    char*           json = NULL;
    u32             json_bytes;
    u32             mesh_count;
    Mesh_Data*      mesh_data_array;
    u32             vertex_data_total_bytes = 0;
    u32             index_data_total_bytes  = 0;
    u32             bin_start_offset = 0;
    
    f32*            gl_vertex_data = NULL; // raw stream of vertices for upload to gl buffer
    s16*            gl_index_data = NULL;
    
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
    
    void read_file_offsets () {
      
      gltf_header = ( GltfHeader* ) glb_file.contents;
      
      u32 gltf_header_size_in_bytes = 12; // magic + version + length
      u32 json_header_in_bytes      = 8; // 4 bytes for chunk length, 4 bytes for chunk type
      u32 json_string_in_bytes      = gltf_header -> json_chunk_length;
      u32 bin_header_in_bytes       = 8; // 4 bytes for chunk length, 4 bytes for chunk type
      bin_start_offset              = gltf_header_size_in_bytes + json_header_in_bytes + json_string_in_bytes + bin_header_in_bytes;
      
    }
    
    void update_json() {
      u32 this_json_bytes = json_size_in_bytes( &glb_file );
      json = init_char_star( this_json_bytes + 1 );
      pull_out_json_string( &glb_file, json, this_json_bytes );
      json_bytes = this_json_bytes;
    }
    
    void set_mesh_count () {
      u32 this_mesh_count = count_meshes ( json, json_bytes );
      mesh_count = this_mesh_count;
    }
    
    void populate_mesh_data() {
      
      size_t bytes = size_t ( mesh_count * sizeof( Mesh_Data ) );
      mesh_data_array = ( Mesh_Data* ) malloc ( bytes );
      
      int g = 8;
      
      for ( u32 i = 0; i < mesh_count; i++ ) {
        mesh_data_array[ i ].set_mesh_index ( i );
        mesh_data_array[ i ].set_buffer_view_data ( json, json_bytes );
        
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
          gl_vertex_data = ( f32* ) malloc ( ( size_t ) vertex_data_total_bytes ); // allocate the amount of data needed to hold vertices
        } else if ( strings_are_equal ( type, "INDEX" ) ) {
          index_data_total_bytes = result;
          gl_index_data = ( s16* ) malloc ( ( size_t ) index_data_total_bytes );
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
    
    void import_data_for_gl ( const char* type ) {
      // just a stream of floats for upload to gl
      f32*  dst_f32;
      s16*  dst_s16 = gl_index_data;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        dst_f32 = ( f32* ) gl_vertex_data;
      }
      
      void* dst;
      
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 offset      = mesh_data_array[ i ].get_binary_offset( type );
        u32 byte_length = mesh_data_array[ i ].get_byte_length( type );
        u32 count       = mesh_data_array[ i ].get_count( type );
        
        if ( strings_are_equal ( type, "VERTEX" ) ) {
          f32* src = ( f32* )( ( char* ) glb_file.contents + bin_start_offset + offset );
          dst = ( void* ) dst_f32;
          memcpy ( dst_f32, src, byte_length );
          dst_f32 += count;
        } else if ( strings_are_equal ( type, "INDEX" ) ) {
          s16* src = ( s16* )( ( char* ) glb_file.contents + bin_start_offset + offset );
          dst = ( void* ) dst_s16;
          memcpy ( dst_s16, src, byte_length );
          dst_s16 += count;
        }
        // u32 dfasfdas = 7;
      }
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
      set_mesh_count();
      populate_mesh_data();
      calculate_data_total_bytes( "VERTEX" );
      calculate_data_total_bytes( "INDEX" );
      import_data_for_gl( "VERTEX" );
      import_data_for_gl( "INDEX" );
    }
    
    void* get_pointer_to_gl_buffer_data ( const char* type ) {
      
      void* result = NULL;
      
      if ( strings_are_equal ( type, "VERTEX" ) ) {
        result = ( void* ) gl_vertex_data;
      } else if ( strings_are_equal ( type, "INDEX" ) ) {
        result = ( void* ) gl_index_data;
      }
      
      return result;
    }
};


void init_program() {
  
  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 model;
  glm::mat4 mvp;
  GLint     position_attribute_location;
  GLint     mvp_uniform_location;

  
  
  SDLParams sdl_params;
  
  init_sdl( &sdl_params );
  
  bool32 running = true;
  
  do {
    
    SDL_Event event;
    while( SDL_PollEvent( &event ) ) {
      
      switch ( event.type ) {
        
        case SDL_QUIT : {
          running = false;
        } break;
        
        case SDL_DROPFILE : {
          
          char* dropped_filepath_orig;
          dropped_filepath_orig = event.drop.file;
          
          u32 filepath_length = string_length( dropped_filepath_orig );
          
          char* filepath = init_char_star( filepath_length + 1 );
          copy_string_into_char_star( dropped_filepath_orig, filepath, filepath_length );
          
          Glb_imported_object glb_imported_object ( filepath );
          glb_imported_object.import_glb_file();
          
          int dfasfda = 4;
          
          GLuint  vbo;
          GLuint  ibo;
          
          u32  vertex_buffer_size = glb_imported_object.get_data_total_bytes( "vertex" );
          u32  index_buffer_size  = glb_imported_object.get_data_total_bytes( "index" );
          
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "vertex_buffer_size = %d\n", vertex_buffer_size );
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "index_buffer_size = %d\n", index_buffer_size );
          
          // set up gl buffers
          GLCall( glGenBuffers( 1, &vbo ) );
          GLCall( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
          GLCall( glBufferData( GL_ARRAY_BUFFER, ( GLsizeiptr ) vertex_buffer_size, 0, GL_STATIC_DRAW ) );
          GLCall( glGenBuffers( 1, &ibo ) );
          GLCall( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ) );
          GLCall( glBufferData( GL_ELEMENT_ARRAY_BUFFER, ( GLsizeiptr ) index_buffer_size, 0, GL_STATIC_DRAW ) );
          
          // upload data to gl
          GLfloat* vertex_data  = ( GLfloat* ) glb_imported_object.get_pointer_to_gl_buffer_data( "VERTEX" );
          s16* index_data       = ( s16* )  glb_imported_object.get_pointer_to_gl_buffer_data( "INDEX" );
          
          int dasfdasfdaf = 17;
          
          GLCall( glBufferData( GL_ARRAY_BUFFER, vertex_buffer_size, vertex_data, GL_STATIC_DRAW ) );
          GLCall( glBufferData( GL_ELEMENT_ARRAY_BUFFER, index_buffer_size, index_data, GL_STATIC_DRAW ) );
          
          ////////////////////
          
          char*           shader_filename       = assets_dir_and_filename( "shaderDebug.glsl" );
          ReadFileResult  shader_file           = read_entire_file( shader_filename );
          u32             shader_program_id     = createShader( shader_file );
          
          glUseProgram( shader_program_id );
          
          position_attribute_location = glGetAttribLocation ( shader_program_id, "aPosition" );
          mvp_uniform_location        = glGetUniformLocation( shader_program_id, "uMVP" );
          
          // load the vertex data
          GLCall( glVertexAttribPointer( position_attribute_location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0 ) );
          GLCall( glEnableVertexAttribArray( position_attribute_location ) );
          
          f32 aspectRatio = ( f32 ) sdl_params.windowWidth / ( f32 ) sdl_params.windowHeight;
          
          projection = glm::perspective( glm::radians( 45.0f ), aspectRatio, 0.1f, 100.0f );
          view = glm::lookAt(
              glm::vec3( 4, 3, 3 )
            , glm::vec3( 0, 0, 0 )
            , glm::vec3( 0, 1, 0 )
          );
          
          model = glm::mat4( 1.0f );
          
          mvp = projection * view * model;
          
          // set the viewport
          glViewport( 0, 0, ( f32 ) sdl_params.windowWidth, ( f32 ) sdl_params.windowHeight );
          
          SDL_free( dropped_filepath_orig );
          free ( shader_filename );
          free( filepath );
          
        } break;
      
      default:
        break;
      }
    }
    
    
    // sdl_flip_frame( sdl_params.window );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    // Send our transformation to the currently bound shader, 
    // in the "MVP" uniform
    glUniformMatrix4fv( mvp_uniform_location, 1, GL_FALSE, &mvp[0][0] );
    
    
    // glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
    
    glEnableVertexAttribArray( position_attribute_location );
    
    SDL_GL_SwapWindow( sdl_params.window );
    
  } while( running );
  
  
}

#endif //PROGRAM_HPP
