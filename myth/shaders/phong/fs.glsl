uniform sampler2D sampler;

// Global intensity of ambient light...
uniform float ambientLight = 0.1f;

// Defines world-space position and channel intensity of a light source...
//uniform struct Light
//{
//  vec3 position;
//  vec3 colour;
//  float attenuation;  // controls the "speed" at which the light attenuates with distance
//} light;

// Defines surface properties of an object...
uniform struct Material
{
  vec3 emissive;    // do you glow?
  vec3 ambient;     // general background light scattering
  vec3 diffuse;     // when lit, how reflective is it?
  vec3 specular;    // if shiney, what colour of shiney?
  float shininess;  // exponent; bigger == sharper, tighter highlights
} material;

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTextureCoord;

out vec4 outColour;

vec3 ComputeLight(Light light, vec3 worldPos, vec3 worldNormal)
{
  // Emissive light is just a value...
  vec3 emissive = material.emissive;

  // Ambient is based on global ambient light level...
  vec3 ambient = material.ambient * ambientLight;

  vec3 totalLight = emissive + ambient;

  if (light.enabled)
  {
    // Diffuse relies on angle between surface normal and light...
    vec3 N = normalize(worldNormal);
    vec3 L = normalize(light.position.xyz - worldPos);
    float diffuseLight = clamp(dot(N,L), 0.0f, 1.0f);
    vec3 diffuse = material.diffuse * light.colour.rgb * diffuseLight;

    // Only compute specular highlights if the surface is facing the light...
    vec3 specular = vec3(0.0f);
    if (diffuseLight > 0.0f)
    {
      // Specular relies on angle between viewer and surface...
      vec3 V = normalize(CameraPosition.xyz - worldPos);
      // Get the vector half-way betwwen the light and the viewer
      vec3 H = normalize(L + V);

      float specularLight = pow(max(dot(N, H), 0), material.shininess);
      specular = material.specular * light.colour.rgb * specularLight;
    }

    float distanceToLight = length(light.position.xyz - worldPos);
    float attenuation = 1.0f / (1.0f + (light.colour.a * pow(distanceToLight, 2)));

    totalLight += attenuation * (diffuse + specular);
  }

  return totalLight;
}

void main()
{
  vec4 surfaceColour = texture(sampler, fragTextureCoord);

  // Get the world position and normal of the point on the surface...
  vec3 worldFragPos = vec3(WorldMatrix * vec4(fragPosition, 1));
  vec3 worldNormal = normalize(mat3(NormalMatrix) * fragNormal);

  vec3 lighting = ComputeLight(Lights[0], worldFragPos, worldNormal);

  outColour.rgb = surfaceColour.rgb * lighting;
  outColour.a = 1;
}
