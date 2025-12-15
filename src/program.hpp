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
#include "glb_class.hpp"

void init_program() {
  
  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 model;
  glm::mat4 mvp;
  GLint     position_attribute_location;
  GLint     mvp_uniform_location;
  
  bool      glb_loaded = false;
  SDLParams sdl_params;
  
  init_sdl( &sdl_params );
  
  bool32 running = true;
  
  GLsizei total_index_count;
  
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
          total_index_count = ( GLsizei ) glb_imported_object.get_total_index_count();
          
          int dfasfda = 4;
          
          GLuint  vbo;
          GLuint  ibo;
          
          u32  vertex_buffer_size = glb_imported_object.get_data_total_bytes( "VERTEX" );
          u32  index_buffer_size  = glb_imported_object.get_data_total_bytes( "INDEX" );
          
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
          GLfloat*  vertex_data = ( GLfloat* )  glb_imported_object.get_pointer_to_gl_buffer_data( "VERTEX" );
          GLushort* index_data  = ( GLushort* ) glb_imported_object.get_pointer_to_gl_buffer_data( "INDEX" );
          
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
          
          glb_loaded = true;
          
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
    
    if ( glb_loaded ) {
      
      glUniformMatrix4fv( mvp_uniform_location, 1, GL_FALSE, &mvp[0][0] );
      
      glDrawElements( GL_TRIANGLES, total_index_count, GL_UNSIGNED_SHORT, 0 );
      
    }
    
    SDL_GL_SwapWindow( sdl_params.window );
    
  } while( running );
  
  
}

#endif //PROGRAM_HPP
