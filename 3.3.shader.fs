#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D dvd_texture;
uniform vec3 dvd_color;

void main()
{
    FragColor = texture(dvd_texture, TexCoord) * vec4(dvd_color, 1.0); // our dvd texture we output
}
