#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect texture0;

void main() {
    //vec2 pos = gl_TexCoord[0].xy;

    gl_FragColor = gl_Color;
}