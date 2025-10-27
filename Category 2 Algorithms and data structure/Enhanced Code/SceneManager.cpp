// ===============================
m_pShaderManager->setVec3Value("lightSources[0].specularColor",0.0f,0.0f,0.0f);
m_pShaderManager->setFloatValue("lightSources[0].focalStrength", 32.0f);
m_pShaderManager->setFloatValue("lightSources[0].specularIntensity", 0.05f);


m_pShaderManager->setVec3Value("lightSources[1].position", -3.0f,14.0f,0.0f);
m_pShaderManager->setVec3Value("lightSources[1].ambientColor", 0.01f,0.01f,0.01f);
m_pShaderManager->setVec3Value("lightSources[1].diffuseColor", 0.4f,0.4f,0.4f);
m_pShaderManager->setVec3Value("lightSources[1].specularColor",0.0f,0.0f,0.0f);
m_pShaderManager->setFloatValue("lightSources[1].focalStrength", 32.0f);
m_pShaderManager->setFloatValue("lightSources[1].specularIntensity", 0.05f);


m_pShaderManager->setVec3Value("lightSources[2].position", 0.6f,5.0f,6.0f);
m_pShaderManager->setVec3Value("lightSources[2].ambientColor", 0.01f,0.01f,0.01f);
m_pShaderManager->setVec3Value("lightSources[2].diffuseColor", 0.3f,0.3f,0.3f);
m_pShaderManager->setVec3Value("lightSources[2].specularColor",0.3f,0.3f,0.3f);
m_pShaderManager->setFloatValue("lightSources[2].focalStrength", 12.0f);
m_pShaderManager->setFloatValue("lightSources[2].specularIntensity", 0.5f);


m_pShaderManager->setBoolValue("bUseLighting", true);
}


void SceneManager::PrepareScene() {
LoadSceneTextures();
DefineObjectMaterials();
// Meshes – load once
m_basicMeshes->LoadPlaneMesh();
m_basicMeshes->LoadCylinderMesh();
m_basicMeshes->LoadTorusMesh();
m_basicMeshes->LoadBoxMesh();
m_basicMeshes->LoadSphereMesh();


// ---- Populate objects_ and quadtree (example from original scene) ----
objects_.clear();
quadtree_->clear();


// Helper lambdas to push objects with bounds and draw callbacks
auto addPlane = [&](const glm::vec3& scale, const glm::vec3& pos, const std::string& tex){
SceneObject o; o.bounds = { pos - glm::vec3(scale.x,0.1f,scale.z), pos + glm::vec3(scale.x,0.1f,scale.z) };
o.draw = [=]() {
const float Xr=0, Yr=0, Zr=0; SetTransformations(scale, Xr, Yr, Zr, pos); SetShaderTexture(tex); m_basicMeshes->DrawPlaneMesh(); };
objects_.push_back(o); quadtree_->insert(o);
};
auto addCylinder = [&](const glm::vec3& scale, const glm::vec3& pos, const std::string& tex){
SceneObject o; o.bounds = { pos - scale, pos + scale }; o.draw = [=]() {
const float Xr=0, Yr=0, Zr=0; SetTransformations(scale, Xr, Yr, Zr, pos); SetShaderTexture(tex); m_basicMeshes->DrawCylinderMesh(); };
objects_.push_back(o); quadtree_->insert(o);
};
auto addTorus = [&](const glm::vec3& scale, const glm::vec3& pos, const std::string& tex, float Yr){
SceneObject o; o.bounds = { pos - scale, pos + scale }; o.draw = [=]() {
const float Xr=0, Zr=0; SetTransformations(scale, Xr, Yr, Zr, pos); SetShaderTexture(tex); m_basicMeshes->DrawTorusMesh(); };
objects_.push_back(o); quadtree_->insert(o);
};
auto addBox = [&](const glm::vec3& scale, const glm::vec3& pos, const std::string& tex){
SceneObject o; o.bounds = { pos - scale, pos + scale }; o.draw = [=]() {
const float Xr=0, Yr=0, Zr=0; SetTransformations(scale, Xr, Yr, Zr, pos); SetShaderTexture(tex); m_basicMeshes->DrawBoxMesh(); };
objects_.push_back(o); quadtree_->insert(o);
};
auto addSphere = [&](const glm::vec3& scale, const glm::vec3& pos, const std::string& tex){
SceneObject o; o.bounds = { pos - scale, pos + scale }; o.draw = [=]() {
const float Xr=0, Yr=0, Zr=0; SetTransformations(scale, Xr, Yr, Zr, pos); SetShaderTexture(tex); m_basicMeshes->DrawSphereMesh(); };
objects_.push_back(o); quadtree_->insert(o);
};


addPlane({20.0f,1.0f,10.0f}, {0.0f,0.0f,0.0f}, "top");
addCylinder({1.5f,2.5f,1.5f}, {0.0f,0.0f,6.0f}, "cup");
addTorus({1.0f,1.0f,1.1f}, {-1.25f,1.25f,7.0f}, "cup", 10.0f);
addCylinder({3.0f,0.2f,3.0f}, {5.0f,0.0f,5.0f}, "cup");
addBox({4.0f,1.0f,2.0f}, {-5.0f,0.0f,6.0f}, "board");
addBox({1.0f,1.0f,1.5f}, {-4.0f,1.0f,6.0f}, "cheese");
addSphere({1.0f,1.0f,1.4f}, {-6.0f,1.0f,6.0f}, "bread");
}


void SceneManager::RenderScene() {
// Query a generous range around origin (simple culling demo). In a full
// implementation, pass camera position/ frustum from ViewManager.
AABB viewRange{ {-30.f,-10.f,-30.f}, {30.f,10.f,30.f} };
std::vector<const SceneObject*> visible; visible.reserve(objects_.size());
quadtree_->query(viewRange, visible);


// Issue draw calls only for visible objects
for (const SceneObject* o : visible) { if (o && o->draw) o->draw(); }
}