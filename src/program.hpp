#ifndef PROGRAM_HPP
#define PROGRAM_HPP

#include "sdl.hpp"
#include "types.hpp"
#include "strings.hpp"
#include "json.hpp"

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
          
          char* dropped_filepath_orig;
          dropped_filepath_orig = event.drop.file;
          
          u32 filepath_length = string_length( dropped_filepath_orig );
          
          char* filepath = init_char_star( filepath_length + 1 );
          copy_string_into_char_star( dropped_filepath_orig, filepath, filepath_length );
          
          // SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "filepath_length is %d\n", filepath_length );
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "filepath is %s\n", filepath );
          
          ReadFileResult glb_file = read_entire_file( filepath );
          u32 json_bytes = json_size_in_bytes( &glb_file );
          
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "json_bytes is %d\n", json_bytes );
          
          char* json = init_char_star( json_bytes + 1 );
          pull_out_json_string( &glb_file, json, json_bytes );
          
          SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "json is %s\n", json );
          
          SDL_free( dropped_filepath_orig );
          free( filepath );
          
        } break;
      
      default:
        break;
      }
    }
    
    sdl_flip_frame( sdl_params.window );
    
    
  } while( running );
  
  
}

#endif //PROGRAM_HPP
