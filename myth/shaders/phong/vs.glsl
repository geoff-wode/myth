
in vec3 Position;
in vec3 Normal;
in vec2 TextureCoord;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTextureCoord;

void main()
{
  gl_Position = WorldViewProjectionMatrix * vec4(Position, 1.0f);

  fragPosition = Position;
  fragNormal = mat3(NormalMatrix) * Normal;
  fragTextureCoord = TextureCoord;
}
