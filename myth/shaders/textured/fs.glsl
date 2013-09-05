uniform sampler2D sampler;
uniform vec2      samplerRepeat = vec2(1);

in vec2 fragTextureCoord;

out vec4 outColour;

void main()
{
  vec2 newCoord = fragTextureCoord / samplerRepeat;
  outColour = texture(sampler, newCoord);
}
