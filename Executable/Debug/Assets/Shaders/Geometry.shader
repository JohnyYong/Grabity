#shader vertex
#version 450 core

layout (location = 0) in vec3 a_Position;
layout (location = 1) in vec4 a_Color;
layout (location = 2) in vec2 a_TexCoord;
layout (location = 3) in float a_TexIndex; // Keep as float to work around integer attribute limitations

uniform mat4 u_ViewProj; // Model-view-projection matrix

out vec4 v_Color;
out vec2 v_TexCoord;
flat out int v_texIndex; // Use 'flat' for precise integer passing

void main()
{
    gl_Position = u_ViewProj * vec4(a_Position, 1.0);

    // Convert to an integer with rounding in vertex shader to avoid rounding issues in the fragment shader
    v_texIndex = int(floor(a_TexIndex + 0.5)); // This rounds `a_TexIndex` to the nearest integer
    v_TexCoord = a_TexCoord;
    v_Color = a_Color;
};

#shader fragment
#version 450 core

layout (location = 0) out vec4 color;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_texIndex; // 'flat' ensures no interpolation of integer value

void main()
{
    // Use v_texIndex directly as an index
    //color = texture(u_Texture[clamp(v_texIndex, 0, 31)], v_TexCoord) * v_Color;
	color = v_Color;
}