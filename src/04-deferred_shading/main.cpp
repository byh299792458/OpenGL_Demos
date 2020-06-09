#include <iostream>
#include <vector>
#include <ogl_demos.hpp>
#include <filesystem.hpp>
#include <model.hpp>

using namespace std;
using uint = unsigned int;

BEGIN_DEMO_DECLARATION(DeferredShading)
public:
	void Initialize(void) override;
	void Display(void) override;
	void Finalize(void) override;
private:
	Model* nanosuit;
	vector<glm::vec3> object_positions_;
	vector<glm::vec3> light_positions_;
	vector<glm::vec3> light_colors_;
	uint gBuffer, gPosition, gNormal, gAlbedoSpec, rboDepth;
	enum Attrib { vPosition, vNormal, vTexcoords};
	enum VaoIds { kScreenVao, kLightSrcBoxVao, NumVaos };
	enum VboIds { kScreenVbo, kLightSrcBoxVbo, NumVbos };
	uint vaos[NumVaos];
	uint vbos[NumVbos];
END_DEMO_DECLARATION(DeferredShading)

uint SCR_Width = 800, SCR_Height = 600;

constexpr float screen[] = {
	// positions        // texture Coords
	-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
	-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
	 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
	 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

constexpr float light_source_box[] = {
	// back face
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
	 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
	-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
	-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
	// front face
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
	 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
	 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
	 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
	-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
	-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
	// left face
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
	-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
	// right face
	 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
	 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
	 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
	 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
	// bottom face
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
	 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
	-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
	-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
	// top face
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
	 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
	-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
	-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left   
};

int main()
{
	auto p = DeferredShading::Create(SCR_Width, SCR_Height, "deferred shading", nullptr, nullptr);
	try {
		p->EmplaceShader(
			FileSystem::getPath("src/04-deferred_shading/geometry_pass.vert").c_str(),
			FileSystem::getPath("src/04-deferred_shading/geometry_pass.frag").c_str()
		);
		p->EmplaceShader(
			FileSystem::getPath("src/04-deferred_shading/light_pass.vert").c_str(),
			FileSystem::getPath("src/04-deferred_shading/light_pass.frag").c_str()
		);
		p->EmplaceShader(
			FileSystem::getPath("src/04-deferred_shading/light_box.vert").c_str(),
			FileSystem::getPath("src/04-deferred_shading/light_box.frag").c_str()
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

void DeferredShading::Initialize(void)
{
	glEnable(GL_DEPTH_TEST);
	camera_.setCameraPos(glm::vec3(0.f, 0.f, 9.f), glm::vec3(0.f, 0.f, 0.f));
	camera_.setMoveSpeed(5.f);

	nanosuit = new Model(
		FileSystem::getPath("resources/objects/nanosuit/nanosuit.obj").c_str()
	);
	object_positions_.push_back(glm::vec3(-3.0, -3.0, -3.0));
	object_positions_.push_back(glm::vec3(0.0, -3.0, -3.0));
	object_positions_.push_back(glm::vec3(3.0, -3.0, -3.0));
	object_positions_.push_back(glm::vec3(-3.0, -3.0, 0.0));
	object_positions_.push_back(glm::vec3(0.0, -3.0, 0.0));
	object_positions_.push_back(glm::vec3(3.0, -3.0, 0.0));
	object_positions_.push_back(glm::vec3(-3.0, -3.0, 3.0));
	object_positions_.push_back(glm::vec3(0.0, -3.0, 3.0));
	object_positions_.push_back(glm::vec3(3.0, -3.0, 3.0));

	glGenFramebuffers(1, &gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	// position
	glGenTextures(1, &gPosition);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_Width, SCR_Height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
	// normal
	glGenTextures(1, &gNormal);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_Width, SCR_Height, 0, GL_RGB, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
	// color + specular color buffer
	glGenTextures(1, &gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_Width, SCR_Height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);
	// create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_Width, SCR_Height);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	
	constexpr int num_lights = 32;
	auto& light_pass_shader = shaders_[1];
	light_pass_shader.use();
	light_pass_shader.setTexUnit("gPosition", 0);
	light_pass_shader.setTexUnit("gNormal", 1);
	light_pass_shader.setTexUnit("gAlbedoSpec", 2);
	srand(std::chrono::high_resolution_clock::now().time_since_epoch().count());
	for (int i = 0; i < num_lights; ++i) {
		float x = rand() % 100 / 100.f * 6.f - 3.f;
		float y = rand() % 100 / 100.f * 6.f - 4.f;
		float z = rand() % 100 / 100.f * 6.f - 3.f;
		light_positions_.push_back(glm::vec3(x, y, z));
		float r = rand() % 100 / 200.f + 0.5f;
		float g = rand() % 100 / 200.f + 0.5f;
		float b = rand() % 100 / 200.f + 0.5f;
		light_colors_.push_back(glm::vec3(r, g, b));
	}
	glGenVertexArrays(NumVaos, vaos);
	glGenBuffers(NumVbos, vbos);
	glBindVertexArray(vaos[kScreenVao]);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[kScreenVbo]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(screen), screen, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), TOVOIDP(0));
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vTexcoords, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), TOVOIDP(3 * sizeof(float)));
	glEnableVertexAttribArray(vTexcoords);

	glBindVertexArray(vaos[kLightSrcBoxVao]);
	glBindBuffer(GL_ARRAY_BUFFER, vbos[kLightSrcBoxVbo]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(light_source_box), light_source_box, GL_STATIC_DRAW);
	glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), TOVOIDP(0));
	glEnableVertexAttribArray(vPosition);
	glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), TOVOIDP(3 * sizeof(float)));
	glEnableVertexAttribArray(vNormal);
	glVertexAttribPointer(vTexcoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), TOVOIDP(6 * sizeof(float)));
	glEnableVertexAttribArray(vTexcoords);
}

void DeferredShading::Display(void)
{
	glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glm::mat4 projection = getProjectionMatrix(0.1, 100.);
	glm::mat4 view = getViewMatrix();
	glm::mat4 model = glm::mat4(1.f);
	auto& geometry_pass_shader = shaders_[0];
	geometry_pass_shader.use();
	geometry_pass_shader.setMat4f("projection", (projection));
	geometry_pass_shader.setMat4f("view", (view));
	for (uint i = 0; i < object_positions_.size(); ++i)
	{
		model = glm::mat4(1.f);
		model = glm::translate(model, object_positions_[i]);
		model = glm::scale(model, glm::vec3(0.25f));
		geometry_pass_shader.setMat4f("model", (model));
		nanosuit->Draw(geometry_pass_shader);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto& light_pass_shader = shaders_[1];
	light_pass_shader.use();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
	const float linear = 0.7;
	const float quadratic = 1.8;
	for (int i = 0; i < light_positions_.size(); ++i) {
		light_pass_shader.setVec3f("lights[" + to_string(i) + "].position", (light_positions_[i]));
		light_pass_shader.setVec3f("lights[" + to_string(i) + "].color", (light_colors_[i]));
		light_pass_shader.setFloat("lights[" + to_string(i) + "].linear", linear);
		light_pass_shader.setFloat("lights[" + to_string(i) + "].quadratic", quadratic);
	}
	auto& view_pos = camera_.getPosition();
	light_pass_shader.setVec3f("view_pos", (view_pos));
	glBindVertexArray(vaos[kScreenVao]);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);

	glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
	glBlitFramebuffer(0, 0, SCR_Width, SCR_Height, 0, 0, SCR_Width, SCR_Height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	auto& light_box_shader = shaders_[2];
	light_box_shader.use();
	light_box_shader.setMat4f("projection", (projection));
	light_box_shader.setMat4f("view", (view));
	for (int i = 0; i < light_positions_.size(); ++i) {
		glm::mat4 model = glm::mat4(1.f);
		model = glm::translate(model, light_positions_[i]);
		model = glm::scale(model, glm::vec3(0.124f));
		light_box_shader.setMat4f("model", (model));
		light_box_shader.setVec3f("color", (light_colors_[i]));
		glBindVertexArray(vaos[kLightSrcBoxVao]);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
	}
}

void DeferredShading::Finalize(void)
{
	glDeleteBuffers(NumVbos, vbos);
	glDeleteVertexArrays(NumVaos, vaos);
	glDeleteTextures(1, &gPosition);
	glDeleteTextures(1, &gNormal);
	glDeleteTextures(1, &gAlbedoSpec);
	glDeleteRenderbuffers(1, &rboDepth);
	glDeleteFramebuffers(1, &gBuffer);
}
