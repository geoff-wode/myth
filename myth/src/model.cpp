#include <debug.h>
#include <model.h>
#include <vector>
#include <glm/glm.hpp>
#include <gl_loader/gl_loader.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <boost/make_shared.hpp>

//---------------------------------------------------------------------

struct Vertex
{
  glm::vec3 position;
  glm::vec3 normal;
  glm::vec2 textureCoord;

  void PrintLog() const
  {
    LOG("p:{%.4f, %.4f, %.4f}, n:{%.4f, %.4f, %.4f}, t:{%.4f, %.4f}\n",
      position.x, position.y, position.z,
      normal.x, normal.y, normal.z,
      textureCoord.x, textureCoord.y);
  }
};

static VertexDeclaration vertexDecl =
{
  sizeof(Vertex),
  {
    VertexAttribute("Position", GL_FLOAT, 3, offsetof(Vertex, position)),
    VertexAttribute("Normal", GL_FLOAT, 3, offsetof(Vertex, normal)),
    VertexAttribute("TextureCoord0", GL_FLOAT, 2, offsetof(Vertex, textureCoord))
  }
};

//---------------------------------------------------------------------

static boost::shared_ptr<ModelMesh> LoadMesh(const aiMesh* sceneMesh);
static boost::shared_ptr<VertexBuffer> LoadMeshVertices(const aiMesh* sceneMesh);
static boost::shared_ptr<IndexBuffer> LoadMeshIndices(const aiMesh* sceneMesh);
static void LoadTextures(const char* const dir, const aiScene* scene, std::vector<boost::shared_ptr<Texture2D>>& textures);

//---------------------------------------------------------------------

Model::Model()
{
}

Model::~Model()
{
}

//---------------------------------------------------------------------

bool Model::Load(const char* const name)
{
  bool loaded = false;

  Assimp::Importer importer;
  const unsigned int processingFlags(aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs);
  const aiScene* scene = importer.ReadFile(name, processingFlags);

  if (scene)
  {
    char dir[FILENAME_MAX] = { '\0' };
    const char* slash = std::strrchr(name, '\\');
    if (NULL != slash)
    {
      strncpy(dir, name, slash - name);
    }
    else
    {
      strncpy(dir, name, sizeof(dir)-1);
    }
    LoadTextures(dir, scene, textures);

    LOG("loaded model: %s - %d meshes\n", name, scene->mNumMeshes);
    for (size_t i = 0; i < scene->mNumMeshes; ++i)
    {
      LOG("  mesh %d:\n", i);
      boost::shared_ptr<ModelMesh> mesh = LoadMesh(scene->mMeshes[i]);
      if (mesh)
      {
        mesh->SetTexture(textures[scene->mMeshes[i]->mMaterialIndex]);
        meshes.push_back(mesh);
      }
      else
      {
        LOG(" mesh %d not valid\n", i);
      }
    }
    loaded = true;
  }
  else
  {
    LOG("failed to load model '%s' - %s\n", name, importer.GetErrorString());
  }

  return loaded;
}

//---------------------------------------------------------------------
void Model::Draw(Device* const device)
{
  for (size_t i = 0; i < meshes.size(); ++i)
  {
    meshes[i]->Draw(device);
  }
}

//---------------------------------------------------------------------
static boost::shared_ptr<ModelMesh> LoadMesh(const aiMesh* sceneMesh)
{
  ASSERTM(sceneMesh->HasFaces() && (3 == sceneMesh->mFaces[0].mNumIndices), "mesh faces are not triangulated");

  if (sceneMesh->HasFaces() && (3 == sceneMesh->mFaces[0].mNumIndices))
  {
    boost::shared_ptr<ModelMesh> mesh(new ModelMesh());

    mesh->SetVertexArray(boost::make_shared<VertexArray>());
    mesh->GetVertexArray()->Bind();

    mesh->SetVertexBuffer( LoadMeshVertices(sceneMesh) );
    mesh->GetVertexBuffer()->Bind();

    mesh->SetIndexBuffer( LoadMeshIndices(sceneMesh) );
    mesh->GetIndexBuffer()->Bind();

    mesh->GetVertexArray()->Configure(&vertexDecl);
    mesh->GetVertexArray()->Unbind();

    mesh->GetIndexBuffer()->Unbind();
    mesh->GetVertexBuffer()->Unbind();

    return mesh;
  }

  return NULL;
}

//---------------------------------------------------------------------
static boost::shared_ptr<VertexBuffer> LoadMeshVertices(const aiMesh* sceneMesh)
{
  static const aiVector3D Zero(0.0f);

  std::vector<Vertex> vertices;

  LOG(" %d vertices:\n", sceneMesh->mNumVertices);
  for (size_t i = 0; i < sceneMesh->mNumVertices; ++i)
  {
    const aiVector3D* position = &(sceneMesh->mVertices[i]);
    const aiVector3D* normal = sceneMesh->HasNormals() ? &(sceneMesh->mNormals[i]) : &Zero;
    const aiVector3D* texCoord = sceneMesh->HasTextureCoords(0) ? &(sceneMesh->mTextureCoords[0][i]) : &Zero;

    Vertex v;
    v.position = glm::vec3(position->x, position->y, position->z);
    v.normal = glm::vec3(normal->x, normal->y, normal->z);
    v.textureCoord = glm::vec2(texCoord->x, texCoord->y);

    vertices.push_back(v);
    //v.PrintLog();
  }

  boost::shared_ptr<VertexBuffer> vb(new VertexBuffer(&vertexDecl, vertices.size(), GL_STATIC_DRAW));
  vb->SetData(vertices.data(), vertices.size(), 0);
  return vb;
}

//---------------------------------------------------------------------
static boost::shared_ptr<IndexBuffer> LoadMeshIndices(const aiMesh* sceneMesh)
{
  std::vector<unsigned int> indices;
  for (size_t i = 0; i < sceneMesh->mNumFaces; ++i)
  {
    const aiFace& face = sceneMesh->mFaces[i];
    indices.push_back(face.mIndices[0]);
    indices.push_back(face.mIndices[1]);
    indices.push_back(face.mIndices[2]);
  }

  boost::shared_ptr<IndexBuffer> ib(new IndexBuffer(GL_UNSIGNED_INT, indices.size(), GL_STATIC_DRAW));
  ib->SetData(indices.data(), indices.size(), 0);
  return ib;
}

//---------------------------------------------------------------------
static void LoadTextures(const char* const dir, const aiScene* scene, std::vector<boost::shared_ptr<Texture2D>>& textures)
{
  static boost::shared_ptr<Texture2D> blank;
  if (!blank)
  {
    blank = boost::make_shared<Texture2D>();
    blank->Load("assets\\images\\blank.jpg");
  }

  for (size_t i = 0; i < scene->mNumMaterials; ++i)
  {
    const aiMaterial* mat = scene->mMaterials[i];
    if (mat->GetTextureCount(aiTextureType_DIFFUSE) > 0)
    {
      aiString path;
      if (AI_SUCCESS == mat->GetTexture(aiTextureType_DIFFUSE, 0, &path))
      {
        char fullpath[FILENAME_MAX];
        sprintf(fullpath, "%s\\%s", dir, path.C_Str());

        boost::shared_ptr<Texture2D> tex(new Texture2D());
        if (tex->Load(fullpath))
        {
          textures.push_back(tex);
        }
        else
        {
          LOG("could not load mesh texture: %s\n", path.C_Str());
        }
      }
    }
    else
    {
      textures.push_back(blank);
    }
  }
}
