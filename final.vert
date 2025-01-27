//  Identity vertex shader
#version 120

void main()
{
   //  Set vertex coordinates
   gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
}
