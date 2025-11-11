#shader vertex
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 position, vec2 texCoords>

uniform vec2 offset;
uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy + offset, 0.0, 1.0);
}

#shader fragment
#version 330 core
out vec4 FragColor;

uniform vec4 color;

void main()
{
    FragColor = color;
}