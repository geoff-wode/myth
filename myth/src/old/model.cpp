#include <cstring>
#include <debug.h>
#include <model.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>
#include <glm/glm.hpp>
#include <boost/make_shared.hpp>

//----------------------------------------------------

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoord;
};

static const struct VertexAttribute
{
  GLint index;
  GLint componentCount;
  GLenum compoentType;
  size_t offset;
} Attributes[] =
{
  { 0, 3, GL_FLOAT, offsetof(Vertex, position) },
  { 1, 3, GL_FLOAT, offsetof(Vertex, normal) },
  { 2, 2, GL_FLOAT, offsetof(Vertex, textureCoord) }
};
const size_t AttributeCount = sizeof(Attributes) / sizeof(Attributes[0]);

//----------------------------------------------------

static void LoadMeshes(const aiScene* scene, std::vector<Mesh>& meshes);
static void LoadMaterials(const aiScene* scene, std::vector<Material>& materials);
static boost::shared_ptr<VertexBuffer> LoadMeshVertices(const aiMesh* sceneMesh);
static void LoadMeshIndices(const aiMesh* sceneMesh, Mesh* mesh);
static void InitMesh(Mesh* mesh, const aiMesh* sceneMesh);

//----------------------------------------------------

Model::Model()
{
}

//----------------------------------------------------

Model::~Model()
{
}

//----------------------------------------------------

bool Model::Load(const char* const name)
{
  char fullPath[FILENAME_MAX];
  if (-1 == GetModuleFileName(NULL, fullPath, sizeof(fullPath)-1))
  {
    ASSERTM(false, "cannot get path to executable\n");
    return false;
  }

  char* lastSlashPos = std::strrchr(fullPath, '\\');
  if (NULL == lastSlashPos)
  {
    lastSlashPos = fullPath;
  }
  else
  {
    ++lastSlashPos;
  }
  const size_t maxCopyLen(glm::min(FILENAME_MAX - strlen(fullPath), FILENAME_MAX - strlen(name)));
  std::strncpy(lastSlashPos, name, maxCopyLen);

  bool loaded = false;

  Assimp::Importer importer;
  const aiScene* scene = importer.ReadFile(fullPath, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
  if (scene)
  {
    LoadMeshes(scene, meshes);
    LoadMaterials(scene, materials);
    LOG("model '%s' loaded\n", name);
  }
  else
  {
    LOG("error: %s\n", name, importer.GetErrorString());
  }

  return loaded;
}

//----------------------------------------------------

void Model::Render(Device* const device)
{
  RenderState r;

  for (size_t i = 0; i < meshes.size(); ++i)
  {
    r.vao = meshes[i].vao;
    r.textureUnits[0].active = true;
    r.textureUnits[0].texture = materials[meshes[i].materialIndex].texture;

    device->DrawIndexed(GL_TRIANGLES, meshes[i].indexCount, meshes[i].indexBuffer->indexType, 0, r);
  }
}

//----------------------------------------------------

static boost::shared_ptr<VertexBuffer> LoadMeshVertices(const aiMesh* sceneMesh)
{
  static const aiVector3D Zero(0);
  std::vector<Vertex> vertices(sceneMesh->mNumVertices);
  for (size_t i = 0; i < sceneMesh->mNumVertices; ++i)
  {
    const aiVector3D* pos = &(sceneMesh->mVertices[i]);
    const aiVector3D* normal = sceneMesh->HasNormals() ? &(sceneMesh->mNormals[i]) : &Zero;
    const aiVector3D* texCoord = sceneMesh->HasTextureCoords(0) ? &(sceneMesh->mTextureCoords[0][i]) : &Zero;

    Vertex v;
    v.position = glm::vec3(pos->x, pos->y, pos->z);
    v.normal = glm::vec3(normal->x, normal->y, normal->z);
    v.textureCoord = glm::vec2(texCoord->x, texCoord->y);
    vertices.push_back(v);
  }

  const size_t vbSize(sizeof(Vertex) * vertices.size());

  boost::shared_ptr<VertexBuffer> vb(new VertexBuffer(vbSize, GL_STATIC_DRAW));
  vb->Enable();
  vb->SetData(vertices.data(), vbSize, 0);
  vb->Disable();

  return vb;
}

//----------------------------------------------------
static void LoadMeshIndices(const aiMesh* sceneMesh, Mesh* mesh)
{
  std::vector<unsigned int> indices(sceneMesh->mNumFaces * 3);
  for (size_t i = 0; i < sceneMesh->mNumFaces; ++i)
  {
    const aiFace* face = &(sceneMesh->mFaces[i]);
    if (face->mNumIndices == 3)
    {
      indices.push_back(face->mIndices[0]);
      indices.push_back(face->mIndices[1]);
      indices.push_back(face->mIndices[2]);
    }
    else
    {
      ASSERTM(false, "mesh is not triangulated\n");
      return;
    }
  }

  mesh->indexCount = indices.size();

  const size_t ibSize(sizeof(unsigned int) * indices.size());
  mesh->indexBuffer = boost::make_shared<IndexBuffer>(ibSize, GL_STATIC_DRAW, GL_UNSIGNED_INT);
  mesh->indexBuffer->Enable();
  mesh->indexBuffer->SetData(indices.data(), ibSize, 0);
  mesh->indexBuffer->Disable();
}

//----------------------------------------------------
static void InitMesh(Mesh* mesh, const aiMesh* sceneMesh)
{
  mesh->materialIndex = sceneMesh->mMaterialIndex;

  mesh->vertexBuffer = LoadMeshVertices(sceneMesh);
  
  LoadMeshIndices(sceneMesh, mesh);

  // Create and initialise the VAO...
  glGenVertexArrays(1, &mesh->vao);
  glBindVertexArray(mesh->vao);

  mesh->indexBuffer->Enable();
  mesh->vertexBuffer->Enable();

  for (size_t i = 0; i < AttributeCount; ++i)
  {
    glEnableVertexAttribArray(Attributes[i].index);
    glVertexAttribPointer(
      Attributes[i].index,
      Attributes[i].componentCount,
      Attributes[i].compoentType,
      GL_FALSE,
      sizeof(Vertex),
      (const void*)Attributes[i].offset);
  }

  glBindVertexArray(0);
  mesh->indexBuffer->Disable();
  mesh->vertexBuffer->Disable();
}

//----------------------------------------------------
static void LoadMeshes(const aiScene* scene, std::vector<Mesh>& meshes)
{
  // Load the individual meshes...
  meshes.resize(scene->mNumMeshes);
  for (size_t i = 0; i < scene->mNumMeshes; ++i)
  {
    InitMesh(&meshes[i], scene->mMeshes[i]);
  }
}

//----------------------------------------------------
static void LoadMaterials(const aiScene* scene, std::vector<Material>& materials)
{
  // This is used when a mesh does not include a texture of its own...
  static boost::shared_ptr<Texture2D> whitePixel;
  if (!whitePixel)
  {
    whitePixel = boost::make_shared<Texture2D>("assets\\images\\whitepixel.png");
    whitePixel->Load();
  }

  for (size_t i = 0; i < scene->mNumMaterials; ++i)
  {
    const aiMaterial* material = scene->mMaterials[i];
    if (material->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
      Material m;
      m.texture = whitePixel;
      aiString path;
      if (AI_SUCCESS == material->GetTexture(aiTextureType_DIFFUSE, 0, &path))
      {
        const char* fileExtension = std::strstr(path.C_Str(), ".pcx");
        if (fileExtension)
        {
          std::strcpy((char*)fileExtension, ".jpg");
        }

        m.texture = boost::make_shared<Texture2D>(std::string("assets\\images\\") + path.data);
        if (!m.texture->Load())
        {
          LOG("mesh texture '%s' not found\n", path.data);
        }
      }
      materials.push_back(m);
    }
  }
}
