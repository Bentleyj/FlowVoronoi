#version 120
#extension GL_ARB_texture_rectangle : enable
#extension GL_EXT_gpu_shader4 : enable

uniform sampler2DRect tex0;
uniform float blurAmnt;

void main()
{
    vec2 pos = gl_TexCoord[0].xy;
    vec4 color = vec4(0.0, 0.0, 0.0, 0.0);
    
    color += 1.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * -4.0));
    color += 2.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * -3.0));
    color += 3.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * -2.0));
    color += 4.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * -1.0));
    
    color += 5.0 * texture2DRect(tex0, pos + vec2(0, blurAmnt));
    
    color += 4.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * 1.0));
    color += 3.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * 2.0));
    color += 2.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * 3.0));
    color += 1.0 * texture2DRect(tex0, pos + vec2(0.0, blurAmnt * 4.0));
    
    color /= 25.0;
//    color = texture2DRect(tex0, gl_TexCoord[0].xy);
    
    gl_FragColor = color;
}