#version 460 core

layout(triangles) in; // say that we take triangles as input   // the layout parameters "invocations=2" would make this shader to be called 2 times per vertex, with different gl_InvocationId
layout(line_strip, max_vertices=3) out;

uniform vec3 wireframe_color; // the color of the lines we emit

out(location=0) vec3 color;

/*
Geometry Shaders provide the following built-in input variables:
in gl_PerVertex
{
  vec4 gl_Position;
  float gl_PointSize;
  float gl_ClipDistance[];
} gl_in[]; // arrays have a length() method, in our case gl_in.length() is always 3
*/


void main(void)
{
    color = wireframe_color;
    gl_Position = gl_in[0].gl_Position; gl_Position.z = gl_Position.z - 0.0001; EmitVertex(); // make the line nearer by decreasing z a bit
    gl_Position = gl_in[1].gl_Position; gl_Position.z = gl_Position.z - 0.0001; EmitVertex();
    gl_Position = gl_in[2].gl_Position; gl_Position.z = gl_Position.z - 0.0001; EmitVertex();
    EndPrimitive();
}