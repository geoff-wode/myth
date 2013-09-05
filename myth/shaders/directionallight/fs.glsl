
uniform sampler2D sampler;
uniform vec2      samplerRepeat = vec2(1);

uniform struct DirectionalLight
{
  vec3 colour;
  vec3 direction;
  float diffuse;
  float ambient;
} directionalLight =
{
  vec3(1.0f),
  vec3(-1.0f, 0.5f, -0.5f),
  1.0f,
  0.1f
};

in vec3 vertexNormal;
in vec2 vertexTextureCoord;

out vec4 outColour;

void main()
{
  vec4 ambientLight = vec4(directionalLight.colour * directionalLight.ambient, 1.0f);

  vec4 totalLight = ambientLight;

  vec3 N = normalize(vertexNormal);
  float angleOfIncidence = dot(N, directionalLight.direction);
  if (angleOfIncidence > 0.0f)
  {
    vec4 diffuseLight = vec4(directionalLight.colour * directionalLight.diffuse * angleOfIncidence, 1.0f);
    totalLight += diffuseLight;
  }

  vec2 newCoord = vertexTextureCoord / samplerRepeat;
  outColour = texture(sampler, newCoord.xy) * totalLight;
}
