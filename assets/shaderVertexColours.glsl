#shader vertex
#version 100

attribute vec4  aPosition;
attribute vec4  aColour;
varying   vec4  vColour;
uniform   mat4  uMVP;

void main() {
  vColour     = aColour;
  gl_Position = uMVP * aPosition;
}

#shader fragment
#version 100

precision mediump float;

varying vec4      vColour;

void main() {
  vec4 colour = vec4( 1.0, 0.0, 0.0, 1.0 );
  colour = vColour;
  // colour = vec4( 1.0, 0.0, 0.0, 1.0 );
  gl_FragColor = colour;
}
