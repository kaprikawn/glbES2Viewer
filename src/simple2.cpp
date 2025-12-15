#include <windows.h>
#include <iostream>
#include "sdl.hpp"
#include "types.hpp"
#include "../ext/glm/glm.hpp"
#include "../ext/glm/gtc/matrix_transform.hpp"
#include "glb_class.hpp"

int CALLBACK WinMain( HINSTANCE Instance, HINSTANCE PrevInstance, LPSTR CommandLine, int ShowCode ) {
  
  // SDL_LogInfo( SDL_LOG_CATEGORY_APPLICATION, "works\n" );
  
  SDL_Window*   sdl_window;
  SDL_GLContext sdl_gl_context;
  
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    fprintf(stderr, "Couldn't init SDL2: %s\n", SDL_GetError());
    exit(1);
  }
  
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 0 );
  SDL_GL_SetAttribute( SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES );
  
  int window_width = 800;
  int window_height = 480;
  int window_flags = SDL_WINDOW_OPENGL;
  sdl_window = SDL_CreateWindow("Test",
    SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
    window_width, window_height, window_flags);
    if (!sdl_window) {
      fprintf(stderr, "Failed to create OpenGL window: %s\n", SDL_GetError());
      exit(1);
    }
    
  sdl_gl_context = SDL_GL_CreateContext(sdl_window);
  if (!sdl_gl_context) {
    fprintf(stderr, "Failed to create OpenGL context: %s\n", SDL_GetError());
    exit(2);
  }
  
  const char *shader_vert_src = 
  // "uniform    float u_time;\n"
  "uniform    mat4  uMVP;;\n"
  "attribute  vec3  aPosition;\n"
  "varying vec3 v_color;\n"
  "void main() {\n"
  // "  v_color = vec3(1.0 - 0.5*(aPosition.x+aPosition.y),aPosition);\n"
  // "  float c = cos(u_time), s = sin(u_time);"
  // "  vec2 t = mat2(c, s, -s, c)*(aPosition-vec2(0.33));\n"
  // "  gl_Position = vec4(t.x*3.0/5.0, t.y, 0.0, 1.0);\n"
  "  gl_Position = uMVP * vec4( aPosition, 1.0 );\n"
  // "  gl_Position = vec4(1.0, 1.0, 0.0, 1.0);\n"
  "}\n";
  const char *shader_frag_src =
  "#ifdef GL_ES\n"
  "precision mediump float;\n"
  "#endif\n"
  "void main() {\n"
  "  gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
  "}\n";
  GLint is_compiled;
  GLuint shader_program_id, shader_vert, shader_frag;
  GLenum err = glewInit();
  shader_program_id = glCreateProgram();
  shader_vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader_vert, 1, &shader_vert_src, NULL);
  glCompileShader(shader_vert);
  glGetShaderiv(shader_vert, GL_COMPILE_STATUS, &is_compiled);
  printf("vert shader compiled %d\n", is_compiled);
  glAttachShader(shader_program_id, shader_vert);
  shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader_frag, 1, &shader_frag_src, NULL);
  glCompileShader(shader_frag);
  glGetShaderiv(shader_frag, GL_COMPILE_STATUS, &is_compiled);
  printf("frag shader compiled %d\n", is_compiled);
  glAttachShader(shader_program_id, shader_frag);
  glLinkProgram(shader_program_id);
  glUseProgram(shader_program_id);
  // GLuint u_time_loc = glGetUniformLocation(shader_program_id, "u_time");
  // float u_time = 0.0f;
  
  GLuint mvp_uniform_location = glGetUniformLocation( shader_program_id, "uMVP" );
  f32 aspect_ratio = ( f32 ) window_width / ( f32 ) window_height;
  
  glm::mat4 projection = glm::perspective( glm::radians( 45.0f ), aspect_ratio, 0.1f, 100.0f );
  glm::mat4 view = glm::lookAt(
      glm::vec3( 4, 3, 3 )
    , glm::vec3( 0, 0, 0 )
    , glm::vec3( 0, 1, 0 )
  );
  
  glm::mat4 model = glm::mat4( 1.0f );
  
  glm::mat4 mvp = projection * view * model;
  
  glViewport( 0, 0, ( f32 ) window_width, ( f32 ) window_height );
  
  // create vbo
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  GLfloat vertex_data_raw[] = {
      -1.0f,  1.0f,  1.0f // front top left
    , -1.0f, -1.0f,  1.0f // front bottom left
    ,  1.0f,  1.0f,  1.0f // front top right
    ,  1.0f, -1.0f,  1.0f // front bottom right
    
    ,  1.0f,  1.0f, -1.0f // back top right
    ,  1.0f, -1.0f, -1.0f // back bottom right
    , -1.0f,  1.0f, -1.0f // back top left
    , -1.0f, -1.0f, -1.0f // back bottom left
  };
  
  Glb_imported_object glb_imported_object( "C:\\dev\\glbES2Viewer\\assets\\cube.glb" );
  glb_imported_object.import_glb_file();
  
  u32 new_bytes = glb_imported_object.get_data_total_bytes( "VERTEX" );
  void* vertex_data = malloc ( new_bytes );
  void* alt_vertices = glb_imported_object.get_pointer_to_gl_buffer_data( "VERTEX" );
  memcpy ( vertex_data, alt_vertices, new_bytes );
  // memcpy ( vertex_data, &vertex_data_raw[0], vertex_bytes );
  // bytes = glb_imported_object.get_data_total_bytes( "VERTEX" );
  
  glBufferData( GL_ARRAY_BUFFER, new_bytes, vertex_data, GL_STATIC_DRAW );
  // glBufferData( GL_ARRAY_BUFFER, sizeof( vertex_data_raw ), vertex_data_raw, GL_STATIC_DRAW );
  
  GLuint ibo;
  glGenBuffers ( 1, &ibo );
  glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ibo );
  GLushort index_data[] = { 0, 1, 2 // front A
    , 2, 1, 3 // front B
    , 4, 2, 3 // right A
    , 4, 3, 5 // right B
    , 4, 5, 6 // back A
    , 6, 5, 7 // back B
    , 7, 0, 6 // left A
    , 0, 7, 1 // left B
    , 5, 3, 7 // bottom A
    , 7, 3, 0 // bottom B
    , 2, 4, 0 // top A
    , 0, 4, 6 // top B
  }; 
  // glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
  
  new_bytes = glb_imported_object.get_data_total_bytes( "INDEX" );
  u16* alt_indices = ( u16* ) malloc ( ( size_t ) new_bytes );
  void* alt_indices_raw = glb_imported_object.get_pointer_to_gl_buffer_data( "INDEX" );
  memcpy ( alt_indices, alt_indices_raw, new_bytes );
  
  // glBufferData( GL_ELEMENT_ARRAY_BUFFER, sizeof( index_data ), index_data, GL_STATIC_DRAW );
  glBufferData( GL_ELEMENT_ARRAY_BUFFER, new_bytes, alt_indices, GL_STATIC_DRAW );
  
  // setup vertex attribs
  GLuint position_attribute_location = 0;
  glEnableVertexAttribArray(position_attribute_location);
  glVertexAttribPointer(position_attribute_location, 3, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
  
  glClearColor( 0.4, 0.6, 0.8, 1.0 );
  bool running = true;
  do {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      running = !(event.type == SDL_QUIT
        || (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE));
    }
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // glUniform1f(u_time_loc, u_time += 1.0f/60.0f);
    glUniformMatrix4fv( mvp_uniform_location, 1, GL_FALSE, &mvp[0][0] );
    // glDrawArrays(GL_TRIANGLES, 0, 24);
    // glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0 );
    glDrawElements( GL_TRIANGLES, 36, GL_UNSIGNED_SHORT, 0 );
    SDL_GL_SwapWindow(sdl_window);
  } while (running);
  
  glUseProgram(0);
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  
  glDeleteProgram(shader_program_id);
  glDeleteBuffers(1, &vbo);
  
  SDL_GL_DeleteContext(sdl_gl_context);
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
  
  return 0;
}
