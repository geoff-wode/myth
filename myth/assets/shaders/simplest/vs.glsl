
void main()
{
  gl_Position = WorldViewProjectionMatrix * vec4(Position, 1.0f);
}
