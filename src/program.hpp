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
      if ( strings_are_equal( "vertex", type ) ) {
        result = vertex_buffer_view_data.byte_length;
      } else if ( strings_are_equal( "index", type ) ) {
        result = index_buffer_view_data.byte_length;
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
    
    u32 get_byte_length ( const char* type ) {
      u32 result = 0;
      if ( strings_are_equal( type, "VERTEX" ) ) {
        result = vertex_buffer_view_data.byte_length;
      } else if ( strings_are_equal( type, "INDEX" ) ) {
        result = index_buffer_view_data.byte_length;
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
    char*       json;
    void*       file_contents;
    u32         json_bytes;
    u32         mesh_count;
    Mesh_Data*  mesh_data_array;
    
    u32         vertex_data_total_bytes = 0;
    u32         index_data_total_bytes  = 0;
    
  public :
    void update_json( ReadFileResult* glb_file ) {
      u32 this_json_bytes = json_size_in_bytes( glb_file );
      json = init_char_star( this_json_bytes + 1 );
      pull_out_json_string( glb_file, json, this_json_bytes );
      json_bytes = this_json_bytes;
    }
    
    void set_file_contents( void* file_contents_in ) {
      file_contents = file_contents_in;
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
    
    void calculate_vertex_data_total_bytes () {
      u32 result = 0;
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 this_byte_length = mesh_data_array[ i ].get_byte_length( "vertex" );
        result += this_byte_length;
      }
      vertex_data_total_bytes = result;
    }
    void calculate_index_data_total_bytes () {
      u32 result = 0;
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 this_byte_length = mesh_data_array[ i ].get_byte_length( "index" );
        result += this_byte_length;
      }
      index_data_total_bytes = result;
    }
    
    u32 get_data_total_bytes ( const char* type ) {
      
      u32 result = 0;
      if ( strings_are_equal ( "vertex", type ) ) {
        result = vertex_data_total_bytes;
      } else if ( strings_are_equal ( "index", type ) ) {
        result = index_data_total_bytes;
      }
      return result;
    }
    
    void get_vertex_data_for_gl() {
      // just a stream of floats for upload to gl
      u32 current_offset = 0;
      for ( u32 i = 0; i < mesh_count; i++ ) {
        u32 offset      = mesh_data_array[ i ].get_binary_offset( "VERTEX" );
        u32 byte_length = mesh_data_array[ i ].get_byte_length( "VERTEX" );
        u32 dfasfdas = 7;
        
      }
      
    }
    
};

bool check_is_glb_file ( ReadFileResult* file ) {
  bool result = false;
  
  GltfHeader* gltf_header = ( GltfHeader* ) file -> contents;
  
  if ( gltf_header -> magic == 1179937895 ) {
    result = true;
  }
  // @TODO : add error message
  return result;
}

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
          
          // SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "filepath_length is %d\n", filepath_length );
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "filepath is %s\n", filepath );
          
          ReadFileResult glb_file = read_entire_file( filepath );
          
          if ( !check_is_glb_file( &glb_file ) ) {
            SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "ERROR - Is not a glb file\n" );
            break;
          }
          
          u32 json_bytes = json_size_in_bytes( &glb_file );
          
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "json_bytes is %d\n", json_bytes );
          
          char* json = init_char_star( json_bytes + 1 );
          pull_out_json_string( &glb_file, json, json_bytes );
          
          Glb_imported_object glb_imported_object;
          glb_imported_object.update_json( &glb_file );
          
          glb_imported_object.set_mesh_count();
          glb_imported_object.populate_mesh_data();
          glb_imported_object.calculate_vertex_data_total_bytes();
          glb_imported_object.calculate_index_data_total_bytes();
          
          glb_imported_object.set_file_contents( glb_file.contents );
          glb_imported_object.get_vertex_data_for_gl();
          
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "json is %s\n", json );
          
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
          
          f32* vertex_data = ( f32* ) malloc ( ( size_t ) vertex_buffer_size );
          u16* index_data  = ( u16* ) malloc ( ( size_t ) index_buffer_size );
          
          int dasf = 7;
          
          free ( vertex_data );
          free ( index_data );
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
