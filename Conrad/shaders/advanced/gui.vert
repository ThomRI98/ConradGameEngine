#version 150 core

in vec2 in_Vertex;
in vec2 in_TexCoord0;

// No need for an ortho matrix here, just draw the pixels

/* Modelview matrix */
uniform mat4 modelview;

out vec2 frag_texCoord0;

void main()
{
	frag_texCoord0 = in_TexCoord0;
	gl_Position = modelview * vec4(in_Vertex, 0.0, 1.0);
}