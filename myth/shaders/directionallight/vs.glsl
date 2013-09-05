
layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TextureCoord;

out vec3 vertexWorldPos;
out vec3 vertexNormal;
out vec2 vertexTextureCoord;

void main()
{
  vec4 P = vec4(Position, 1.0f);
  vec4 N = vec4(Normal, 0.0f);

  gl_Position = WorldViewProjectionMatrix * P;

  vertexWorldPos = (WorldMatrix * P).xyz;
  vertexNormal = (NormalMatrix * N).xyz;
  vertexTextureCoord = TextureCoord;
}
