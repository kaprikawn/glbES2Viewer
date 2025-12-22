#ifndef SHADERS_HPP
#define SHADERS_HPP

#include "sdl.hpp"
#include "strings.hpp"

struct Shader_Data {
  u32 shader_id = 0xFFFFFFFF; // referred to as 'program' in docs
  s32 position_attribute_location;
  s32 normal_attribute_location;
  s32 colour_attribute_location;
  s32 mvp_uniform_location;
};

enum Shader_Types {
    SHADER_TYPE_TEXTURED_LIGHT_PER_VERTEX
  , SHADER_TYPE_TEXTURED_LIGHT_PER_FRAGMENT
  , SHADER_TYPE_VERTEX_COLOURS_LIGHT_PER_VERTEX
  , SHADER_TYPE_VERTEX_COLOURS_LIGHT_PER_FRAGMENT
  , SHADER_TYPE_VERTEX_COLOURS_NO_LIGHT
  , SHADER_TYPE_DEBUG
  , SHADER_TYPE_NUM_VALUES
};

const u32 shader_types_num_values = SHADER_TYPE_NUM_VALUES;

Shader_Data shader_data [ shader_types_num_values ];

u32 compile_shader ( u32 type, const char* source ) {
  
  u32 result;
  
  u32 shader_program_id = glCreateShader( type );
  glShaderSource( shader_program_id, 1, &source, nullptr );
  glCompileShader( shader_program_id );
  
  int compile_result;
  glGetShaderiv( shader_program_id, GL_COMPILE_STATUS, &compile_result );
  if( compile_result == GL_FALSE ) {
    int length;
    glGetShaderiv( shader_program_id, GL_INFO_LOG_LENGTH, &length );
    char* message = ( char* )alloca( length * sizeof( char ) );
    glGetShaderInfoLog( shader_program_id, length, &length, message );
    
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "Failed to complile %s shader : %s\n", ( type == GL_VERTEX_SHADER ? "vertex" : "fragment" ), message );
    SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "### SOURCE ### \n %s \n", source );
    glDeleteShader( shader_program_id );
    return 0;
  }
  result = shader_program_id;
  return result;
}

enum ShaderCompilationTypes {
  SHADERTYPENONE = -1, VERTEXSHADER = 0, FRAGMENTSHADER = 1
};

u32 create_shader ( ReadFileResult shader_file ) {
  
  u32 result;
  
  const char* shader_source = ( const char* )shader_file.contents;
  u32 filesize_max       = shader_file.contents_size;
  
  u32 type = SHADERTYPENONE;
  
  char* vertex_shader_source    = ( char* )malloc( filesize_max );
  char* fragment_shader_source  = ( char* )malloc( filesize_max );
  char  current_line [ 4086 ];
  
  u32 current_line_index     = 0;
  u32 line_start            = 0;
  u32 line_end;
  
  u32 vertex_current_index   = 0;
  u32 fragment_current_index = 0;
  
  for( u32 i = 0; i < filesize_max; i++ ) {
    char my_char = ( char )shader_source[ i ];
    if( my_char == 10 || my_char == 13 ) {
      
      line_end = i;
      
      if( strings_are_equal( ( const char* )current_line, "#shader vertex" ) ) {
        type = VERTEXSHADER;
      } else if( strings_are_equal( ( const char* )current_line, "#shader fragment" ) ) {
        type = FRAGMENTSHADER;
      } else {
        
        for( u32 j = line_start; j < line_end; j++ ) {
          
          if( type == VERTEXSHADER ) {
            vertex_shader_source[ vertex_current_index++ ] = ( char )shader_source[ j ];
          } else if( type == FRAGMENTSHADER ) {
            fragment_shader_source[ fragment_current_index++ ] = ( char )shader_source[ j ];
          }
          
        }
        
        if( type == VERTEXSHADER ) {
          vertex_shader_source[ vertex_current_index++ ] = 0x0a;
        } else if( type == FRAGMENTSHADER ) {
          fragment_shader_source[ fragment_current_index++ ] = 0x0a;
        }
      }
      
      for( u32 j = 0; j < filesize_max; j++ )
        current_line[ j ] = '\0';
      current_line_index = 0;
      line_start = i + 1;
    } else {
      current_line[ current_line_index++ ] = my_char;
    }
  }
  
  vertex_shader_source[ vertex_current_index ] = '\0';
  fragment_shader_source[ fragment_current_index ] = '\0';
  
  u32 shader_program_id  = glCreateProgram();
  u32 vs                 = compile_shader( GL_VERTEX_SHADER   , vertex_shader_source );
  u32 fs                 = compile_shader( GL_FRAGMENT_SHADER , fragment_shader_source );
  
  glAttachShader( shader_program_id, vs );
  glAttachShader( shader_program_id, fs );
    
  glLinkProgram( shader_program_id );
  glValidateProgram( shader_program_id );
  
  glDeleteShader( vs );
  glDeleteShader( fs );
  
  free( vertex_shader_source );
  free( fragment_shader_source );
  
  result = shader_program_id;
  
  return result;
}

void compile_shader_by_type_id ( u32 shader_type_id ) {
  
  // lookup Shader_Types enum and copile that shader by it's index
  
  if ( shader_data [ shader_type_id ].shader_id < 0xFFFFFFFF ) {
    return; // already compiled
  }
  
  char* shader_filename = NULL;
  
  if ( shader_type_id == SHADER_TYPE_DEBUG ) {
    shader_filename = assets_dir_and_filename( "shaderDebug.glsl" );
  }
  
  if ( shader_filename == NULL ) {
    
  } else {
    
    ReadFileResult  shader_file       = read_entire_file ( shader_filename );
    u32             shader_program_id = create_shader ( shader_file );
    
    shader_data [ shader_type_id ].shader_id = shader_program_id;
    
    s32 position_attribute_location = glGetAttribLocation ( shader_program_id, "aPosition" );
    s32 normal_attribute_location   = glGetAttribLocation ( shader_program_id, "aNormal" );
    s32 colour_attribute_location   = glGetAttribLocation ( shader_program_id, "aColour" );
    s32 mvp_uniform_location        = glGetUniformLocation ( shader_program_id, "uMVP" );
    
    shader_data [ shader_type_id ].position_attribute_location = position_attribute_location;
    shader_data [ shader_type_id ].normal_attribute_location   = normal_attribute_location;
    shader_data [ shader_type_id ].colour_attribute_location   = colour_attribute_location;
    shader_data [ shader_type_id ].mvp_uniform_location        = mvp_uniform_location;
    
    GLCall( glEnableVertexAttribArray( position_attribute_location ) );
    GLCall( glEnableVertexAttribArray( normal_attribute_location ) );
    GLCall( glEnableVertexAttribArray( colour_attribute_location ) );
    
    free_read_file_result ( &shader_file );
    free ( shader_filename );
  }
}

#endif //SHADERS_HPP
