#include <windows.h>
#include <iostream>
#include "sdl.hpp"

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
  
  int video_width = 800;
	int video_height = 480;
	int window_flags = SDL_WINDOW_OPENGL;
	sdl_window = SDL_CreateWindow("Test",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		video_width, video_height, window_flags);
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
  "uniform float u_time;\n"
  "attribute vec2 aPosition;\n"
  "varying vec3 v_color;\n"
  "void main() {\n"
  "	v_color = vec3(1.0 - 0.5*(aPosition.x+aPosition.y),aPosition);\n"
  "	float c = cos(u_time), s = sin(u_time);"
  "	vec2 t = mat2(c, s, -s, c)*(aPosition-vec2(0.33));\n"
  "	gl_Position = vec4(t.x*3.0/5.0, t.y, 0.0, 1.0);\n"
  // "	gl_Position = vec4(1.0, 1.0, 0.0, 1.0);\n"
  "}\n";
  const char *shader_frag_src =
  "#ifdef GL_ES\n"
  "precision mediump float;\n"
  "#endif\n"
  "void main() {\n"
  "	gl_FragColor = vec4( 1.0, 0.0, 0.0, 1.0 );\n"
  "}\n";
  GLint is_compiled;
  GLuint program, shader_vert, shader_frag;
  GLenum err = glewInit();
  program = glCreateProgram();
  shader_vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(shader_vert, 1, &shader_vert_src, NULL);
  glCompileShader(shader_vert);
  glGetShaderiv(shader_vert, GL_COMPILE_STATUS, &is_compiled);
  printf("vert shader compiled %d\n", is_compiled);
  glAttachShader(program, shader_vert);
  shader_frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(shader_frag, 1, &shader_frag_src, NULL);
  glCompileShader(shader_frag);
  glGetShaderiv(shader_frag, GL_COMPILE_STATUS, &is_compiled);
  printf("frag shader compiled %d\n", is_compiled);
  glAttachShader(program, shader_frag);
	glLinkProgram(program);
	glUseProgram(program);
	GLuint u_time_loc = glGetUniformLocation(program, "u_time");
	float u_time = 0.0f;
  
	// create vbo
	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	GLfloat vertex_data[] = {-0.5f, -0.5f, -0.5f, // 0
    0.5f, -0.5f, -0.5f, // 1
    0.5f,  0.5f, -0.5f, // 2
    -0.5f,  0.5f, -0.5f, // 3
    -0.5f, -0.5f,  0.5f, // 4
    0.5f, -0.5f,  0.5f, // 5
    0.5f,  0.5f,  0.5f, // 6
    -0.5f,  0.5f,  0.5f  // 7
  };
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_data), vertex_data, GL_STATIC_DRAW);
  
  GLuint  ibo;
  glGenBuffers ( 1, &ibo );
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
  GLuint index_data[] = { 0, 1, 2, 2, 3, 0, 2, 6, 7, 2, 3, 7, 0, 4, 5, 0, 1, 5, 0, 2, 6, 0, 4, 6, 1, 3, 7, 1, 5, 7, 0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7 }; 
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_data), index_data, GL_STATIC_DRAW);
  
	// setup vertex attribs
	GLuint aPosition = 0;
	glEnableVertexAttribArray(aPosition);
	glVertexAttribPointer(aPosition, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
  
	glClearColor(0.4, 0.6, 0.8, 1.0);
	bool running = true;
	do {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			running = !(event.type == SDL_QUIT
				|| (event.type == SDL_KEYDOWN && event.key.keysym.sym == SDLK_ESCAPE));
		}
    
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUniform1f(u_time_loc, u_time += 1.0f/60.0f);
		glDrawArrays(GL_TRIANGLES, 0, 24);
		SDL_GL_SwapWindow(sdl_window);
	} while (running);
  
	glUseProgram(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
  
	glDeleteProgram(program);
	glDeleteBuffers(1, &vbo);
  
  SDL_GL_DeleteContext(sdl_gl_context);
	SDL_DestroyWindow(sdl_window);
	SDL_Quit();
  
  return 0;
}
