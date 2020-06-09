#include <iostream>
#include <iomanip>
#include <thread>
#include <vector>
#include <ogl_demos.hpp>
#include <filesystem.hpp>
#include "rope.hpp"

using namespace std;
using uint = unsigned int;

BEGIN_DEMO_DECLARATION(ElasticBand)
public:
	void Initialize(void) override;
	void Display() override;
	void Finalize(void) override;

private:
	enum Attrib_IDs { vPosition = 0, vTexcoords, vNormal};
	enum RopeType { RopeEuler, RopeVerlet };
	RopeType rope_type_{ RopeEuler };
	uint rope_vao_{ 0 }, sphere_vao_{ 0 }, skybox_vao_{ 0 };
	uint rope_vbo_{ 0 }, sphere_vbo_{ 0 }, skybox_vbo_{ 0 };
	uint sphere_ebo_{ 0 };
	uint num_node_per_rope_{ 80 };
	Rope* rope_euler_{ nullptr };
	Rope* rope_verlet_{ nullptr };
	void RenderRope(void);
	void RenderSphere(void);
	void RenderSkybox(void);
END_DEMO_DECLARATION(ElasticBand)

const int SCR_Width = 1024;
const int SCR_Height = 820;

int main()
{
	auto p = ElasticBand::Create(SCR_Width, SCR_Height, "ElasticBand", nullptr, nullptr);
	p->EmplaceShader(
		FileSystem::getPath("src/01-elastic_band/rope.vert").c_str(),
		FileSystem::getPath("src/01-elastic_band/rope.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/01-elastic_band/ball.vert").c_str(),
		FileSystem::getPath("src/01-elastic_band/ball.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/01-elastic_band/skybox.vert").c_str(),
		FileSystem::getPath("src/01-elastic_band/skybox.frag").c_str()
	);
	p->EmplaceTextureCube(vector<string>{
		FileSystem::getPath("resources/textures/skybox/right.jpg").c_str(),
		FileSystem::getPath("resources/textures/skybox/left.jpg").c_str(),
		FileSystem::getPath("resources/textures/skybox/top.jpg").c_str(),
		FileSystem::getPath("resources/textures/skybox/bottom.jpg").c_str(),
		FileSystem::getPath("resources/textures/skybox/front.jpg").c_str(),
		FileSystem::getPath("resources/textures/skybox/back.jpg").c_str()
	}, false);
	p->Run();
	delete p;
	return 0;
}

void ElasticBand::Initialize(void)
{
	camera_.setCameraPos(glm::vec3(0.f, 0.f, 100.f), glm::vec3(0.f, 0.f, 0.f));
	camera_.setMoveSpeed(50);
	if (num_node_per_rope_ < 2)
		throw std::exception("too few nodes");

	if (RopeEuler == rope_type_)
		rope_euler_ = new Rope(glm::vec3(0.f, 20.f, 0.f), glm::vec3(-30.f, 20.f, 0.f), num_node_per_rope_, 1, 150, { 0 });
	else
		rope_verlet_ = new Rope(glm::vec3(0.f, 20.f, 0.f), glm::vec3(-30.f, 20.f, 0.f), num_node_per_rope_, 1, 150, { 0 });

	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_POLYGON_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
	glLineWidth(4);
	glEnable(GL_DEPTH_TEST);
}

void ElasticBand::Display()
{
	static auto last_time = std::chrono::high_resolution_clock::now();
	auto cur_time = std::chrono::high_resolution_clock::now();
	auto duration_per_frame = (cur_time - last_time);
	if (duration_per_frame.count() != 0) {
		int fps = (int)(1'000'000'000. / (double)duration_per_frame.count());
		cout << "\rfps : " << setw(2) << fps << "        ";
	}
	last_time = cur_time;
	std::this_thread::sleep_for(std::chrono::milliseconds(10));

	//static const float dark_green[] = { 0.2f,0.3f,0.3f,1.f };
	//glClearBufferfv(GL_COLOR, 0, dark_green);
	glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 projection = getProjectionMatrix(0.1, 500);
	glm::mat4 view = camera_.getViewMatrix();
	glm::mat4 model = glm::mat4(1.f);

	auto& rope_shader = shaders_[0];
	rope_shader.use();
	rope_shader.setMat4f("model", model);
	rope_shader.setMat4f("view", view);
	rope_shader.setMat4f("projection", projection);
	rope_shader.setTexUnit("tex", 0);
	RenderRope();
	//RenderRope_test();

	auto& ball_shader = shaders_[1];
	ball_shader.use();
	Rope* rope = nullptr;
	if (RopeEuler == rope_type_)
		rope = rope_euler_;
	else
		rope = rope_verlet_;
	auto& last_node = rope->masses_.back();
	auto& pos = last_node->position_;
	last_node->mass_ = 100;
	model = glm::translate(model, pos);
	model = glm::scale(model, glm::vec3(10));
	ball_shader.setMat4f("model", model);
	ball_shader.setMat4f("view", view);
	ball_shader.setMat4f("projection", projection);
	ball_shader.setTexUnit("skybox", 0);
	ball_shader.setVec3f("cam_pos_world", camera_.position_);
	RenderSphere();

	auto& skybox_render = shaders_[2];
	skybox_render.use();
	skybox_render.setMat4f("view", glm::mat4(glm::mat3(view)));
	skybox_render.setMat4f("projection", projection);
	skybox_render.setTexUnit("skybox", 0);
	RenderSkybox();

#if 1
	int steps_per_frame = 64;
	for (int i = 0; i < steps_per_frame; i++) 
	{
		if (RopeEuler == rope_type_)
			rope_euler_->SimulateEuler(1. / steps_per_frame, glm::vec3(0, -1, 0), 1);
		else
			rope_verlet_->SimulateVerlet(1. / steps_per_frame, glm::vec3(0, -1, 0),1);
	}
#endif
}

void ElasticBand::Finalize(void)
{
	glDeleteBuffers(1, &rope_vbo_);
	glDeleteBuffers(1, &sphere_vbo_);
	glDeleteBuffers(1, &sphere_ebo_);
	glDeleteBuffers(1, &skybox_vbo_);
	glDeleteVertexArrays(1, &rope_vao_);
	glDeleteVertexArrays(1, &sphere_vao_);
	glDeleteVertexArrays(1, &skybox_vao_);
}

void ElasticBand::RenderRope(void)
{
	Rope* rope = nullptr;
	if (RopeEuler == rope_type_)
		rope = rope_euler_;
	else
		rope = rope_verlet_;
	if (0 == rope_vao_) {
		glGenVertexArrays(1, &rope_vao_);
		glBindVertexArray(rope_vao_);
		glGenBuffers(1, &rope_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, rope_vbo_);
		glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * num_node_per_rope_, nullptr, GL_DYNAMIC_DRAW);
		float* rope_data = (float*)glMapNamedBuffer(rope_vbo_, GL_WRITE_ONLY);
		for (int i = 0; i < num_node_per_rope_; ++i) {
			rope_data[3 * i] = rope->masses_[i]->position_.x;
			rope_data[3 * i + 1] = rope->masses_[i]->position_.y;
			rope_data[3 * i + 2] = rope->masses_[i]->position_.z;
		}
		glUnmapNamedBuffer(rope_vbo_);

		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), TOVOIDP(0));
		glEnableVertexAttribArray(vPosition);
	}
	glBindVertexArray(rope_vao_);
	glBindBuffer(GL_ARRAY_BUFFER, rope_vbo_);
	float* rope_data = (float*)glMapNamedBuffer(rope_vbo_, GL_WRITE_ONLY);
	for (int i = 0; i < num_node_per_rope_; ++i) {
		rope_data[3 * i] = rope->masses_[i]->position_.x;
		rope_data[3 * i + 1] = rope->masses_[i]->position_.y;
		rope_data[3 * i + 2] = rope->masses_[i]->position_.z;
	}
	glUnmapNamedBuffer(rope_vbo_);
	glDrawArrays(GL_LINE_STRIP, 0, num_node_per_rope_);
	glBindVertexArray(0);
}

void ElasticBand::RenderSphere()
{
	static uint indices_count = 0;
	if (0 == sphere_vao_)
	{
		glGenVertexArrays(1, &sphere_vao_);
		glGenBuffers(1, &sphere_vbo_);
		glGenBuffers(1, &sphere_ebo_);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		for (int y = 0; y < Y_SEGMENTS; ++y)
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				indices.push_back(y * (X_SEGMENTS + 1) + x);
				indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
			}
		}
		indices_count = indices.size();

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}
		glBindVertexArray(sphere_vao_);
		glBindBuffer(GL_ARRAY_BUFFER, sphere_vbo_);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphere_ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		float stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, stride, TOVOIDP(0));
		glEnableVertexAttribArray(vTexcoords);
		glVertexAttribPointer(vTexcoords, 2, GL_FLOAT, GL_FALSE, stride, TOVOIDP(3 * sizeof(float)));
		glEnableVertexAttribArray(vNormal);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, stride, TOVOIDP(5 * sizeof(float)));
		glBindVertexArray(0);
	}
	glBindVertexArray(sphere_vao_);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubes_.back().getTextureId());
	glDrawElements(GL_TRIANGLE_STRIP, indices_count, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ElasticBand::RenderSkybox()
{
	if (0 == skybox_vao_) 
	{
		float skybox_vertices[] = {
			// positions          
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f
		};

		glGenVertexArrays(1, &skybox_vao_);
		glBindVertexArray(skybox_vao_);
		glGenBuffers(1, &skybox_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, skybox_vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), skybox_vertices, GL_STATIC_DRAW);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), TOVOIDP(0));
		glEnableVertexAttribArray(vPosition);
		glBindVertexArray(0);
	}
	glDepthFunc(GL_LEQUAL);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubes_.front().getTextureId());
	glBindVertexArray(skybox_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);
}

