#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "sdl.hpp"
#include "types.hpp"

void init_program() {
  
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
          char* dropped_filepath = event.drop.file;
          SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_INFORMATION, "File dropped on window", dropped_filepath, sdl_params.window );
          SDL_free( dropped_filepath );
        } break;
      
      default:
        break;
      }
    }
    
    sdl_flip_frame( sdl_params.window );
    
    
  } while( running );
  
  
}

#endif //PROGRAM_HPP
