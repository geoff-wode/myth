
out vec2 fragTextureCoord;

void main()
{
  gl_Position = WorldViewProjectionMatrix * vec4(Position, 1.0f);

  fragTextureCoord = TextureCoord0;
}
