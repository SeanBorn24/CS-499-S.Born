// ===============================
* Responsibilities:
* - Load textures & materials
* - Maintain a spatial index (Quadtree) of scene objects
* - Issue draw calls for visible objects each frame
* Supports CO3 (modular design) and CO4 (documentation/testing).
*/
class SceneManager {
public:
/**
* @brief Construct a new Scene Manager
* @param pShaderManager Non-owning pointer to an active ShaderManager
*/
SceneManager(ShaderManager* pShaderManager);


/** Destructor – frees meshes and GL textures. */
~SceneManager();


struct TEXTURE_INFO {
std::string tag; ///< texture tag for lookup
uint32_t ID; ///< OpenGL texture id
};


struct OBJECT_MATERIAL {
float ambientStrength{0.2f};
glm::vec3 ambientColor{0.2f};
glm::vec3 diffuseColor{0.8f};
glm::vec3 specularColor{1.0f};
float shininess{16.0f};
std::string tag;
};


// Student-customizable entry points
void PrepareScene();
void RenderScene();


// Exposed for tests
void LoadSceneTextures();
void SetupSceneLights();
void DefineObjectMaterials();


private:
// ---------- Resources & state ----------
ShaderManager* m_pShaderManager = nullptr; // non-owning
std::unique_ptr<ShapeMeshes> m_basicMeshes; // owning


int m_loadedTextures = 0;
TEXTURE_INFO m_textureIDs[16]{};
std::vector<OBJECT_MATERIAL> m_objectMaterials;


// Spatial index
std::unique_ptr<Quadtree> quadtree_;
std::vector<SceneObject> objects_; // backing store for quadtree


// ---------- Texture helpers ----------
bool CreateGLTexture(const char* filename, std::string tag);
void BindGLTextures();
void DestroyGLTextures();
int FindTextureID(std::string tag);
int FindTextureSlot(std::string tag);


// ---------- Materials ----------
bool FindMaterial(std::string tag, OBJECT_MATERIAL& material);


// ---------- Draw helpers ----------
void SetTransformations(
glm::vec3 scaleXYZ,
float XrotationDegrees,
float YrotationDegrees,
float ZrotationDegrees,
glm::vec3 positionXYZ);


void SetShaderColor(float r, float g, float b, float a);
void SetShaderTexture(std::string textureTag);
void SetTextureUVScale(float u, float v);
void SetShaderMaterial(std::string materialTag);
};