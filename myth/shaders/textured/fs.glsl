uniform sampler2D sampler;

in vec2 fragTextureCoord;

out vec4 outColour;

void main()
{
  outColour = texture(sampler, fragTextureCoord);
}
