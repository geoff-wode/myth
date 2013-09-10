struct DirectionalLight
{
  vec3 colour;
  vec3 direction;
  float diffuse;
  float ambient;
};

uniform sampler2D sampler;
uniform vec2      samplerRepeat = vec2(1);

// Default is for a white light left, above and behind the (0,0,0) point.
uniform DirectionalLight light =
  {
    vec3(1.0f),
    vec3(-1.0f, 0.5f, -0.5f),
    1.0f,
    0.1f
  };

// Specular reflection parameters. Default is for a non-reflective surface.
uniform vec3 specularColour = vec3(0);
uniform float shininess = 0.0f;

in vec3 vertexWorldPos;
in vec3 vertexNormal;
in vec2 vertexTextureCoord;

out vec4 outColour;

void main()
{
  vec3 ambientLight = light.colour * light.ambient;
  vec3 totalLight = ambientLight;

  vec3 N = normalize(vertexNormal);
  vec3 L = normalize(light.direction);

  float angleOfIncidence = dot(N, L);
  if (angleOfIncidence > 0.0f)
  {
    vec3 diffuseLight = light.colour * light.diffuse * angleOfIncidence;
    totalLight += diffuseLight;

    vec3 V = normalize(CameraPosition.xyz - vertexWorldPos);
    vec3 H = normalize(L + V);

    float specularPower = pow(max(dot(N, H), 0.0f), shininess);
    vec3 specularLight = specularColour * specularPower * light.colour;
    totalLight += specularLight;
  }

  vec2 newCoord = vertexTextureCoord / samplerRepeat;
  outColour = texture(sampler, newCoord.xy) * vec4(totalLight, 1.0f);
}
