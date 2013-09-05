
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TextureCoord;

out vec3 vertexNormal;
out vec2 vertexTextureCoord;

void main()
{
  gl_Position = WorldViewProjectionMatrix * vec4(Position, 1.0f);

  vertexNormal = NormalMatrix * vec4(Normal, 0.0f);
  vertexTextureCoord = TextureCoord;
}
