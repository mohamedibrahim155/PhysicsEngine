#version 330 core
out vec4 FragColor;



uniform vec3 objCol;

void main()
{
    FragColor = vec4(1.0 * objCol,1.0); 
}