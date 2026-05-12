#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "sdl.hpp"
#include <cstring>
#include "strings.hpp"
#include "glb_class.hpp"

void load_model_from_filepath ( const char* filepath_in ) {
  SDL_LogInfo ( SDL_LOG_CATEGORY_APPLICATION, "Loading model from filepath : %s\n", filepath_in );
  
  String filepath ( filepath_in );
  
  int x = 4;
  
  Glb_imported_object glb_imported_object ( filepath );
  
  
  int y = 4;
}

void init_program () {
  
  SDLParams sdl_params;
  
  init_sdl( &sdl_params );
  
  load_model_from_filepath ( "C:\\dev\\glbES2Viewer\\assets\\colouredCube.glb" );
  
  bool32 running = true;
  
  do {
    
    SDL_Event event;
    while( SDL_PollEvent( &event ) ) {
      
      switch ( event.type ) {
        
        case SDL_QUIT : {
          running = false;
        } break;
        
        case SDL_DROPFILE : {
          const char* dropped_filepath = event.drop.file;
          
          if ( dropped_filepath != nullptr ) {
            load_model_from_filepath ( dropped_filepath );
            
            SDL_free ( ( void* ) dropped_filepath );
          }
          
        } break;
        
        
        default:
          break;
      }
    }
    
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
    
    
    SDL_GL_SwapWindow( sdl_params.window );
    
  } while( running );
  
  
}

#endif //PROGRAM_HPP
