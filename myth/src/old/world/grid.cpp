#include <world/grid.h>
#include <debug.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <gl_loader/gl_loader.h>
#include <boost/make_shared.hpp>
#include <vector>

//--------------------------------------------------------------------------------------------

struct Vertex
{
  glm::vec3 position;
  glm::vec2 textureCoord;
};

//--------------------------------------------------------------------------------------------

// Create the tile's vertex buffer.
// N x N vertices are created on the XZ plane and centred on (0,0,0).
// Texture coordinates range from (0,0) at (0,0,0) to (1,1) at (N,0,N).
static void BuildVertices(int size, std::vector<Vertex>& vertices);

// Create the tile's index buffer.
static void BuildIndices(int size, std::vector<unsigned short>& indices);

//--------------------------------------------------------------------------------------------

boost::shared_ptr<Asset> Grid::Create(size_t numCells)
{
  boost::shared_ptr<Asset> grid(new Asset());

  glGenVertexArrays(1, &grid->vao);
  glBindVertexArray(grid->vao);

  const size_t size(numCells + 1);

  grid->drawType = GL_TRIANGLE_STRIP;
  grid->drawFirstVertex = 0;
  grid->drawVertexCount = 2 * size * (size-1);

  grid->shader = boost::make_shared<Shader>("assets\\shaders\\texturedfloor");
  grid->texture = boost::make_shared<Texture2D>("assets\\images\\marble_biegetiled_df_.dds");
  grid->texture->Load();

  grid->shader->SetUniform("sampler", 0);
  grid->shader->SetUniform("material.emissive", glm::vec3(0.0f));
  grid->shader->SetUniform("material.ambient", glm::vec3(1.0f));
  grid->shader->SetUniform("material.diffuse", glm::vec3(1.0f));
  grid->shader->SetUniform("material.specular", glm::vec3(0.0f));
  grid->shader->SetUniform("material.shininess", 0.0f);

  std::vector<Vertex> vertices(size * size);
  BuildVertices(size, vertices);
  grid->vertexBuffer = boost::make_shared<VertexBuffer>(sizeof(Vertex) * vertices.size(), GL_STATIC_DRAW);
  grid->vertexBuffer->Enable();
  grid->vertexBuffer->SetData(vertices.data(), sizeof(Vertex) * vertices.size(), 0);

  std::vector<unsigned short> indices(grid->drawVertexCount);
  BuildIndices(size, indices);
  grid->indexBuffer = boost::make_shared<IndexBuffer>(sizeof(unsigned short) * grid->drawVertexCount, GL_STATIC_DRAW, GL_UNSIGNED_SHORT);
  grid->indexBuffer->Enable();
  grid->indexBuffer->SetData(indices.data(), sizeof(unsigned short) * grid->drawVertexCount, 0);

  glEnableVertexAttribArray(grid->shader->GetAttributeIndex("Position"));
  glVertexAttribPointer(grid->shader->GetAttributeIndex("Position"), 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, position));
  glEnableVertexAttribArray(grid->shader->GetAttributeIndex("TextureCoord"));
  glVertexAttribPointer(grid->shader->GetAttributeIndex("TextureCoord"), 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, textureCoord));

  glBindVertexArray(0);
  grid->vertexBuffer->Disable();
  grid->indexBuffer->Disable();

  return grid;
}

//--------------------------------------------------------------------------------------------


static void BuildVertices(int size, std::vector<Vertex>& vertices)
{
	const float step(1.0f / (float)size);
	int index = 0;
	for (int z = 0; z < size; ++z)
	{
		for (int x = 0; x < size; ++x)
		{
			Vertex v;
			v.position = glm::vec3((float)x - (0.5f * (size-1)), 0, -z + (0.5f * (size-1)));
			v.textureCoord = glm::vec2(step * x, step * z);
			vertices[index++] = v;
		}
	}
}

//--------------------------------------------------------------------------------------------
static void BuildIndices(int size, std::vector<unsigned short>& indices)
{
	int index = 0;
	int z = 0;
	while (z < size - 1)
	{
		for (int x = 0; x < size; x++)
		{
			indices[index++] = x + (z * size);
			indices[index++] = x + ((z + 1) * size);
		}
		++z;
		if (z < size - 1)
		{
			for (int x = size - 1; x >= 0; x--)
			{
				indices[index++] = x + ((z + 1) * size);
				indices[index++] = x + (z * size);
			}
		}
		++z;
	}
}
