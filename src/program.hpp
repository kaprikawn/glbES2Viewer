#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include <cstdlib>
#include "sdl.hpp"
#include "types.hpp"
#include "strings.hpp"
#include "json_reader.hpp"
#include "shaders.hpp"
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/matrix_transform.hpp"
#include "glb_class.hpp"
#include "entity_class.hpp"

u32 program_id_currently_in_use = 0xFFFFFFFF;

void init_program() {
  
  glm::mat4 projection;
  glm::mat4 view;
  glm::mat4 model;
  glm::mat4 mvp;
  
  bool      glb_loaded = false;
  SDLParams sdl_params;
  
  init_sdl( &sdl_params );
  
  bool32 running = true;
  
  GLsizei total_index_count;
  u32     mesh_count = 0;
  u32     current_offset_in_gl_array_buffer         = 0;
  u32     current_offset_in_gl_element_array_buffer = 0;
  Entity_Class* entity_class = NULL;
  GLsizei stride = GLsizei( 13 * sizeof( f32 ) ); // 13 floats per vertex
  
  f32     y_rotation = 0;
  
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
          
          u32  vertex_buffer_size = VERTEX_BYTES * glb_imported_object.get_total_vertex_count();
          u32  index_buffer_size  = glb_imported_object.get_index_data_total_bytes();
          
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "vertex_buffer_size = %d\n", vertex_buffer_size );
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "index_buffer_size = %d\n", index_buffer_size );
          
          // set up gl buffers
          GLCall( glGenBuffers( 1, &vbo ) );
          GLCall( glBindBuffer( GL_ARRAY_BUFFER, vbo ) );
          GLCall( glBufferData( GL_ARRAY_BUFFER, ( GLsizeiptr ) vertex_buffer_size, 0, GL_STATIC_DRAW ) );
          GLCall( glGenBuffers( 1, &ibo ) );
          GLCall( glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo ) );
          GLCall( glBufferData( GL_ELEMENT_ARRAY_BUFFER, ( GLsizeiptr ) index_buffer_size, 0, GL_STATIC_DRAW ) );
          
          entity_class                = new Entity_Class ( glb_imported_object );
          mesh_count                  = entity_class -> get_mesh_count();
          
          // load the vertex data
          
          f32 aspectRatio = ( f32 ) sdl_params.window_width / ( f32 ) sdl_params.window_height;
          
          projection = glm::perspective( glm::radians( 45.0f ), aspectRatio, 0.1f, 100.0f );
          view = glm::lookAt(
              glm::vec3( 14, 13, 13 )
            , glm::vec3( 0, 0, 0 )
            , glm::vec3( 0, 1, 0 )
          );
          
          model = glm::mat4( 1.0f );
          
          mvp = projection * view * model;
          
          // set the viewport
          glViewport( 0, 0, ( f32 ) sdl_params.window_width, ( f32 ) sdl_params.window_height );
          
          glb_loaded = true;
          
          SDL_free( dropped_filepath_orig );
          free( filepath );
          
        } break;
      
      default:
        break;
      }
    }
    
    
    // sdl_flip_frame( sdl_params.window );
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    if ( glb_loaded ) {
      
      for ( u32 i = 0; i < mesh_count; i++ ) {
        
        s32 position_attribute_location = entity_class -> get_shader_data_for_mesh_index ( i, SHADER_DATA_TYPE_POSITIONS );
        s32 colour_attribute_location   = entity_class -> get_shader_data_for_mesh_index ( i, SHADER_DATA_TYPE_COLOURS );
        s32 normal_attribute_location   = entity_class -> get_shader_data_for_mesh_index ( i, SHADER_DATA_TYPE_NORMALS );
        s32 mvp_uniform_location        = entity_class -> get_shader_data_for_mesh_index ( i, SHADER_DATA_TYPE_MVP );
        
        y_rotation += 1.0f;
        while ( y_rotation > 360.0f ) {
          y_rotation -= 360.0f;
        }
        
        u32 program_id_for_this_mesh = entity_class -> get_program_id_for_mesh_index ( i );
        if ( program_id_for_this_mesh != program_id_currently_in_use ) {
          glUseProgram ( ( GLuint ) program_id_for_this_mesh );
          program_id_currently_in_use = program_id_for_this_mesh;
        }
        
        model = glm::mat4( 1.0f );
        float angle = glm::radians( y_rotation ); // Convert degrees to radians
        model = glm::rotate( model, angle, glm::vec3( 0.0f, 1.0f, 0.0f ) );
        mvp = projection * view * model;
        
        glUniformMatrix4fv( mvp_uniform_location, 1, GL_FALSE, &mvp[0][0] );
        
        GLsizei       index_count_for_mesh    = ( GLsizei )       entity_class -> get_index_count( i );
        const GLvoid* index_offset_in_buffer  = ( const GLvoid* ) entity_class -> get_index_offset_in_gl( i );
        const GLvoid* pointer_positions       = ( const GLvoid* ) entity_class -> get_vertex_offset_in_gl( i, "VERTEX" );
        const GLvoid* pointer_normals         = ( const GLvoid* ) entity_class -> get_vertex_offset_in_gl( i, "NORMAL" );
        const GLvoid* pointer_colours         = ( const GLvoid* ) entity_class -> get_vertex_offset_in_gl( i, "COLOR0" );
        
        GLCall( glVertexAttribPointer( position_attribute_location, 4, GL_FLOAT, GL_FALSE, stride, pointer_positions ) );
        GLCall( glVertexAttribPointer( normal_attribute_location  , 3, GL_FLOAT, GL_FALSE, stride, pointer_normals ) );
        GLCall( glVertexAttribPointer( colour_attribute_location  , 4, GL_FLOAT, GL_FALSE, stride, pointer_colours ) );
        GLCall( glDrawElements( GL_TRIANGLES, index_count_for_mesh, GL_UNSIGNED_SHORT, index_offset_in_buffer ) );
        
      }
      
    }
    
    SDL_GL_SwapWindow( sdl_params.window );
    
  } while( running );
  
  
}

#endif //PROGRAM_HPP
