#include <iostream>
#include <ogl_demos.hpp>
#include <filesystem.hpp>


using namespace std;

BEGIN_DEMO_DECLARATION(BumpMapping)
public:
	void Initialize() override;
	void Display() override;
	void Finalize() override;
private:
	enum Vao_Ids { kBrickVao, NumVaos };
	enum Vbo_Ids { kBrickVbo, NumVbos };
	enum Ebo_Ids { kBrickEbo, NumEbos };
	enum Attrib_Ids {vPosition, vTexCoords, vNormal};

	unsigned int vaos[NumVaos];
	unsigned int vbos[NumVbos];
	unsigned int ebos[NumEbos];

END_DEMO_DECLARATION(BumpMapping)

constexpr float vertices[] = {
	 10.f,  10.f, 0.f, 1.f, 1.f,    0.f, 0.f, 1.f,
	 10.f, -10.f, 0.f, 1.f, 0.f,    0.f, 0.f, 1.f,
	-10.f, -10.f, 0.f, 0.f, 0.f,    0.f, 0.f, 1.f,
	-10.f,  10.f, 0.f, 0.f, 1.f,    0.f, 0.f, 1.f,
};

constexpr unsigned int indices[] = {
	0, 1, 2,
	0, 2, 3,
};

int main() {
	OglDEMOS* p = BumpMapping::Create(800, 600, "bump mapping", nullptr, nullptr);
	try {
		p->EmplaceShader(
			FileSystem::getPath("src/03-mapping/bump_mapping.vert").c_str(),
			FileSystem::getPath("src/03-mapping/bump_mapping.frag").c_str()
		);
		p->EmplaceTexture2D(
			FileSystem::getPath("resources/textures/brickwall/brickwall.jpg").c_str()
		);
		p->EmplaceTexture2D(
			FileSystem::getPath("resources/textures/brickwall/brickwall_normal.jpg").c_str()
		);
	}
	catch (...) {
		delete p;
		p = nullptr;
		return 0;
	}
	p->Run();
	return 0;
}


void BumpMapping::Initialize()
{
	camera_.setCameraPos(glm::vec3(0.f, 0.f, 40.f), glm::vec3(0.f, 0.f, 0.f));
	camera_.setMoveSpeed(40.);
	glGenVertexArrays(NumVaos, vaos);
	glGenBuffers(NumVbos, vbos);
	glGenBuffers(NumEbos, ebos);

	glBindVertexArray(vaos[kBrickVao]);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[kBrickVbo]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebos[kBrickEbo]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
	
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), TOVOIDP(0));
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		TOVOIDP(3 * sizeof(float)));
	glEnableVertexAttribArray(vTexCoords);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
		TOVOIDP(5 * sizeof(float)));
	glEnableVertexAttribArray(vNormal);
}

void BumpMapping::Display()
{
	glClearColor(0.2f, 0.3f, 0.1f, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto time = TimeElapsed_Milliseconds() / 1000.;
	auto view = getViewMatrix();
	auto projection = getProjectionMatrix(0.1, 300);
	glm::vec3 light_color(1.f, 1.f, 1.f);
	float radius = 5.f;
	//glm::vec3 light_position(radius * std::sin(time), radius * std::cos(time), 2.f);
	glm::vec3 light_position(10.f, 10.f, 4.f);
	auto diffuse_color = light_color * glm::vec3(0.5);
	auto ambient_color = diffuse_color * glm::vec3(0.2);
	auto camera_pos = camera_.getPosition();

	auto& bump_mapping_shader = shaders_[0];
	bump_mapping_shader.use();
	bump_mapping_shader.setMat4f("view", (view));
	bump_mapping_shader.setMat4f("projection", (projection));
	bump_mapping_shader.setTexUnit("material.color", 0);
	bump_mapping_shader.setTexUnit("material.normal", 1);
	bump_mapping_shader.setVec3f("material.specular", 0.5f, 0.5f, 0.5f);
	bump_mapping_shader.setFloat("material.shininess", 32.0f);
	bump_mapping_shader.setVec3f("light.ambient", (ambient_color));
	bump_mapping_shader.setVec3f("light.diffuse", (diffuse_color));
	bump_mapping_shader.setVec3f("light.specular", 1.f, 1.f, 1.f);
	bump_mapping_shader.setVec3f("light.position", (light_position));
	bump_mapping_shader.setVec3f("viewPos", (camera_pos));
	glBindVertexArray(vaos[kBrickVao]);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture_2ds_[0].getTextureId());
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, texture_2ds_[1].getTextureId());
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, TOVOIDP(0));
	glBindVertexArray(0);
}

void BumpMapping::Finalize()
{
	glDeleteBuffers(NumEbos, ebos);
	glDeleteBuffers(NumVbos, vbos);
	glDeleteVertexArrays(NumVaos, vaos);
}
