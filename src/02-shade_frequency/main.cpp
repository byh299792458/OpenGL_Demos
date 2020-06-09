#include <iostream>
#include <ogl_demos.hpp>
#include <filesystem.hpp>

using uint = unsigned int;
//#define DEBUG

BEGIN_DEMO_DECLARATION(ShadeFrequency)
public:
	void Initialize(void) override;
	void Display(void) override;
	void Finalize(void) override;
	enum Attrib_IDs { vPosition = 0, vTexCoords, vNormal};

private:
	uint floor_vao_{ 0 }, skybox_vao_{ 0 }, ball_vao_{ 0 }, quad_vao_{ 0 };
	uint floor_vbo_{ 0 }, skybox_vbo_{ 0 }, ball_vbo_{ 0 }, quad_vbo_{ 0 };
	uint floor_ebo_{ 0 }, ball_ebo_{ 0 }, quad_ebo_{ 0 };
	uint shadow_fbo_{ 0 };
	uint depth_map_{ 0 };

	void RenderScene(bool);
	void RenderFloor(void);
	void RenderBall(void);
	void RenderQuad(void);
	void RenderSkyBox(void);
END_DEMO_DECLARATION(ShadeFrequency);

constexpr int nMeridian = 20; // 经线
constexpr int nParallel = 20; // 纬线
constexpr int nVertices = nParallel * nMeridian + 2;
constexpr int nFace = nMeridian * nParallel * 2;
constexpr int radius = 3;
constexpr double pi = 3.1415926;

const int SCR_Width = 1024;
const int SCR_Height = 768;
const int SHADOW_Width = 1024;
const int SHADOW_Height = 1024;

using std::cin;
using std::cout;
using std::endl;
using std::endl;


int main() {
	auto p = ShadeFrequency::Create(SCR_Width, SCR_Height, "shade_frequency", nullptr, nullptr);

	// 添加着色器程序
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/ground_shading.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/ground_shading.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/phong_shading.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/phong_shading.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/flat_shading.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/flat_shading.frag").c_str(),
		FileSystem::getPath("src/02-shade_frequency/flat_shading.geom").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/floor.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/floor.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/skybox.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/skybox.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/shadow_map.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/shadow_map.frag").c_str()
	);
	p->EmplaceShader(
		FileSystem::getPath("src/02-shade_frequency/debug_fbo.vert").c_str(),
		FileSystem::getPath("src/02-shade_frequency/debug_fbo.frag").c_str()
	);
	
	// 添加纹理
	p->EmplaceTexture2D(
		FileSystem::getPath("resources/textures/wood.png").c_str()
	);
	std::vector<std::string> faces = {
		FileSystem::getPath("resources/textures/skybox/right.jpg"),
		FileSystem::getPath("resources/textures/skybox/left.jpg"),
		FileSystem::getPath("resources/textures/skybox/top.jpg"),
		FileSystem::getPath("resources/textures/skybox/bottom.jpg"),
		FileSystem::getPath("resources/textures/skybox/front.jpg"),
		FileSystem::getPath("resources/textures/skybox/back.jpg"),
	};
	p->EmplaceTextureCube(faces);
	p->Run();
	return 0;
}


void ShadeFrequency::Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	camera_.setCameraPos(glm::vec3(0.f, 10.f, 40.f), glm::vec3(0.f, 0.f, 0.f));
	camera_.setMoveSpeed(30.);
	glGenFramebuffers(1, &shadow_fbo_);
	glGenTextures(1, &depth_map_);
	glBindTexture(GL_TEXTURE_2D, depth_map_);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_Width, SHADOW_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo_);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_map_, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void ShadeFrequency::Display() 
{
	// render shadow depth map
	glViewport(0, 0, SHADOW_Width, SHADOW_Height);
	glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo_);
	glClear(GL_DEPTH_BUFFER_BIT);
	RenderScene(true);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	// render scene
	glViewport(0, 0, SCR_Width, SCR_Height);
	glClearColor(0.2f, 0.2f, 0.3f, 0.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	RenderScene(false);
}

void ShadeFrequency::Finalize() 
{
	glDeleteBuffers(1, &floor_ebo_);
	glDeleteBuffers(1, &floor_vbo_);
	glDeleteBuffers(1, &skybox_vbo_);
	glDeleteBuffers(1, &ball_ebo_);
	glDeleteBuffers(1, &ball_vbo_);
	glDeleteVertexArrays(1, &floor_vao_);
	glDeleteVertexArrays(1, &skybox_vao_);
	glDeleteVertexArrays(1, &ball_vao_);
}

void ShadeFrequency::RenderFloor(void)
{
	if (0 == floor_vao_) 
	{
		const float floor_data[] = {
			// positions          // texture Coords  // normal
			-1.f,  -1.f,   -1.f,   0.f, 2.f,        0.f, 1.f, 0.f,
			-1.f,  -1.f,    1.f,   0.f, 0.f,        0.f, 1.f, 0.f,
			 1.f,  -1.f,    1.f,   2.f, 0.f,        0.f, 1.f, 0.f,
			 1.f,  -1.f,   -1.f,   2.f, 2.f,        0.f, 1.f, 0.f,
		};
		const uint floor_idx[] = {
			0, 1, 2,
			0, 2, 3,
			0, 3, 2,
			0, 2, 1,
		};
		glGenVertexArrays(1, &floor_vao_);
		glBindVertexArray(floor_vao_);
		glGenBuffers(1, &floor_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, floor_vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(floor_data), floor_data, GL_STATIC_DRAW);
		glGenBuffers(1, &floor_ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, floor_ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(floor_idx), floor_idx, GL_STATIC_DRAW);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), TOVOIDP(0));
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), TOVOIDP(3 * sizeof(float)));
		glEnableVertexAttribArray(vTexCoords);
		glVertexAttribPointer(vNormal, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
			TOVOIDP(5 * sizeof(float)));
		glEnableVertexAttribArray(vNormal);
	}
	glBindVertexArray(floor_vao_);
	glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, TOVOIDP(0));
	glBindVertexArray(0);
}

void ShadeFrequency::RenderSkyBox(void)
{
	if (0 == skybox_vao_) {
		const float skybox_vertices[] = 
		{
			// positions          
			-1.0f,  1.0f, -1.0f,  // 背面
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,   // 左面
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			 1.0f, -1.0f, -1.0f,   // 右面
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,

			-1.0f, -1.0f,  1.0f,    // 前面
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f,  1.0f,

			-1.0f,  1.0f, -1.0f,    // 上面
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f, -1.0f,

			-1.0f, -1.0f, -1.0f,    // 下面
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
	glBindVertexArray(skybox_vao_);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void ShadeFrequency::RenderBall(void) {
	if (0 == ball_vao_) 
	{
		// 着色球
		glGenVertexArrays(1, &ball_vao_);
		glBindVertexArray(ball_vao_);
		// 生成顶点数据
		glGenBuffers(1, &ball_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, ball_vbo_);
		int vertex_size = 3 * sizeof(float) * nVertices;
		glBufferData(GL_ARRAY_BUFFER, vertex_size, nullptr, GL_STATIC_DRAW);
		float* ballbuffer = (float*)glMapNamedBuffer(ball_vbo_, GL_WRITE_ONLY);
		ballbuffer[0] = 0.f;      //x
		ballbuffer[1] = radius;   //y
		ballbuffer[2] = 0.f;      //z
		int k = 3;
		for (int i = 0; i < nParallel; ++i) {
			for (int j = 0; j < nMeridian; ++j) {
				float theta = 180.f / (nParallel + 1) * (i + 1);
				float phi = 360.f / nMeridian * j;
				float r = radius * std::sin(glm::radians(theta));
				ballbuffer[k + 0] = r * std::cos(glm::radians(phi));
				ballbuffer[k + 1] = radius * std::cos(glm::radians(theta));
				ballbuffer[k + 2] = -r * std::sin(glm::radians(phi));
				k += 3;
			}
		}
		ballbuffer[k + 0] = 0.f;
		ballbuffer[k + 1] = -radius;
		ballbuffer[k + 2] = 0.f;
		glUnmapNamedBuffer(ball_vbo_);

		// 生成索引数据
		glGenBuffers(1, &ball_ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ball_ebo_);
		int index_size = 3 * sizeof(unsigned int) * nFace;
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, index_size, nullptr, GL_STATIC_DRAW);
		unsigned int* ballIdx = (unsigned int*)glMapNamedBuffer(ball_ebo_, GL_WRITE_ONLY);
		// 第一层
		int i = 0;
		for (; i < nMeridian - 1; ++i) {
			ballIdx[i * 3] = 0;
			ballIdx[i * 3 + 1] = i + 1;
			ballIdx[i * 3 + 2] = i + 2;
		}
		ballIdx[i * 3] = 0;
		ballIdx[i * 3 + 1] = i + 1;
		ballIdx[i * 3 + 2] = 1;
		// 中间 nParallel-1 层
		unsigned int cur_Idx = 1;
		unsigned int cur_face = nMeridian;
		for (int i = 0; i < nParallel - 1; ++i)
		{
			int j = 0;
			for (; j < nMeridian - 1; ++j)
			{
				ballIdx[3 * cur_face] = cur_Idx;
				ballIdx[3 * cur_face + 1] = cur_Idx + nMeridian;
				ballIdx[3 * cur_face + 2] = cur_Idx + nMeridian + 1;
				cur_face++;
				ballIdx[3 * cur_face] = cur_Idx;
				ballIdx[3 * cur_face + 1] = cur_Idx + nMeridian + 1;
				ballIdx[3 * cur_face + 2] = cur_Idx + 1;
				cur_face++;
				cur_Idx++;
			}
			ballIdx[3 * cur_face] = cur_Idx;
			ballIdx[3 * cur_face + 1] = cur_Idx + nMeridian;
			ballIdx[3 * cur_face + 2] = cur_Idx + 1;
			cur_face++;
			ballIdx[3 * cur_face] = cur_Idx;
			ballIdx[3 * cur_face + 1] = cur_Idx + 1;
			ballIdx[3 * cur_face + 2] = cur_Idx - j;
			cur_face++;
			cur_Idx++;
		}
		// 最后一层
		unsigned int lastIdx = nVertices - 1;
		for (int i = 1; i < nMeridian + 1; ++i)
		{
			int cur_face = nFace - i;
			ballIdx[cur_face * 3] = lastIdx;
			ballIdx[cur_face * 3 + 1] = lastIdx - (i - 1 == 0 ? nMeridian : i - 1);
			ballIdx[cur_face * 3 + 2] = lastIdx - i;
		}
		glUnmapNamedBuffer(ball_ebo_);

		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), TOVOIDP(0));
		glBindVertexArray(0);
	}
	glBindVertexArray(ball_vao_);
	glDrawElements(GL_TRIANGLES, 3 * nFace, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void ShadeFrequency::RenderQuad(void)
{
	if (0 == quad_vao_) {
		float quad[] = {
			-1.f,  1.f, 0.f, 0.f, 1.f,
			 1.f, -1.f, 0.f, 1.f, 0.f,
			-1.f, -1.f, 0.f, 0.f, 0.f,
			 1.f,  1.f, 0.f, 1.f, 1.f,
		};
		uint quad_idx[] = {
			0, 1, 3,
			0, 2, 1,
		};
		glGenVertexArrays(1, &quad_vao_);
		glBindVertexArray(quad_vao_);
		glGenBuffers(1, &quad_vbo_);
		glBindBuffer(GL_ARRAY_BUFFER, quad_vbo_);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quad), quad, GL_STATIC_DRAW);
		glGenBuffers(1, &quad_ebo_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quad_ebo_);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_idx), quad_idx, GL_STATIC_DRAW);
		glVertexAttribPointer(vPosition, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), TOVOIDP(0));
		glEnableVertexAttribArray(vPosition);
		glVertexAttribPointer(vTexCoords, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 
			TOVOIDP(3 * sizeof(float)));
		glEnableVertexAttribArray(vTexCoords);
		glBindVertexArray(0);
	}
	glBindVertexArray(quad_vao_);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, TOVOIDP(0));
	glBindVertexArray(0);
}

void ShadeFrequency::RenderScene(bool is_render_shadow)
{
	glm::vec3 light_color = glm::vec3(2.f) * glm::vec3(1.f, 1.f, 1.f);
	double t = TimeElapsed_Milliseconds() / 5000.;
	glm::vec3 light_position(30.f * std::sin(t),
		20.f, 30.f * std::cos(t));
	auto diffuse_color = light_color * glm::vec3(0.5);
	auto ambient_color = diffuse_color * glm::vec3(0.2);
	auto camera_pos = camera_.getPosition();
	auto model = glm::mat4(1.0f);

	auto view = getViewMatrix();
	auto light_space_view = glm::lookAt(light_position, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
	auto projection = getProjectionMatrix(0.1, 300);
	auto light_space_projection = glm::ortho(-50.f, 50.f, -50.f, 50.f, 0.1f, 300.f);
	auto light_space_trans = light_space_projection * light_space_view;

	if (is_render_shadow) 
	{
		auto& shadow_map_shader = shaders_[5];
		shadow_map_shader.use();
		shadow_map_shader.setMat4f("model", model);
		shadow_map_shader.setMat4f("light_space_trans", light_space_trans);
		RenderBall();
		model = glm::translate(model, glm::vec3(10, 0, 0));
		shadow_map_shader.setMat4f("model", model);
		RenderBall();
		model = glm::translate(model, glm::vec3(-20, 0, 0));
		shadow_map_shader.setMat4f("model", model);
		RenderBall();
		RenderFloor();
	}
	else {
#ifdef DEBUG
		auto& debug_shder = shaders_.back();
		debug_shder.use();
		debug_shder.setTexUnit("texture2d", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, depth_map_);
		RenderQuad();
#else 
		// 第一个球，ground着色
		auto& ground_shader = shaders_[0];
		model = glm::translate(model, glm::vec3(0, 0, 0));
		ground_shader.use();
		ground_shader.setMat4f("model", (model));
		ground_shader.setMat4f("projection", (projection));
		ground_shader.setMat4f("view", (view));
		ground_shader.setVec3f("material.ambient", 1.0f, 0.f, 0.f);
		ground_shader.setVec3f("material.diffuse", 1.0f, 0.f, 0.f);
		ground_shader.setVec3f("material.specular", 0.5f, 0.5f, 0.5f);
		ground_shader.setFloat("material.shininess", 32.0f);

		ground_shader.setVec3f("light.ambient", ambient_color);
		ground_shader.setVec3f("light.diffuse", diffuse_color);
		ground_shader.setVec3f("light.specular", 1.f, 1.f, 1.f);
		ground_shader.setVec3f("light.position", (light_position));
		ground_shader.setVec3f("viewPos", (camera_pos));
		RenderBall();

		// 第二个球，phong着色
		auto& phong_shader = shaders_[1];
		model = glm::translate(model, glm::vec3(10, 0, 0));
		phong_shader.use();
		phong_shader.setMat4f("model", (model));
		phong_shader.setMat4f("projection", (projection));
		phong_shader.setMat4f("view", (view));
		phong_shader.setVec3f("material.ambient", 1.0f, 0.f, 0.f);
		phong_shader.setVec3f("material.diffuse", 1.0f, 0.f, 0.f);
		phong_shader.setVec3f("material.specular", 0.5f, 0.5f, 0.5f);
		phong_shader.setFloat("material.shininess", 32.0f);

		phong_shader.setVec3f("light.ambient", (ambient_color));
		phong_shader.setVec3f("light.diffuse", (diffuse_color));
		phong_shader.setVec3f("light.specular", 1.f, 1.f, 1.f);
		phong_shader.setVec3f("light.position", (light_position));
		phong_shader.setVec3f("viewPos", (camera_pos));
		RenderBall();

		// 第三个球，flat着色
		auto& flat_shader = shaders_[2];
		model = glm::translate(model, glm::vec3(-20, 0, 0));
		flat_shader.use();
		flat_shader.setMat4f("model", (model));
		flat_shader.setMat4f("projection", (projection));
		flat_shader.setMat4f("view", (view));
		flat_shader.setVec3f("material.ambient", 1.0f, 0.f, 0.f);
		flat_shader.setVec3f("material.diffuse", 1.0f, 0.f, 0.f);
		flat_shader.setVec3f("material.specular", 0.5f, 0.5f, 0.5f);
		flat_shader.setFloat("material.shininess", 32.0f);

		flat_shader.setVec3f("light.ambient", (ambient_color));
		flat_shader.setVec3f("light.diffuse", (diffuse_color));
		flat_shader.setVec3f("light.specular", 1.f, 1.f, 1.f);
		flat_shader.setVec3f("light.position", (light_position));
		flat_shader.setVec3f("viewPos", (camera_pos));
		RenderBall();

		// 地板
		auto& floor_shader = shaders_[3];
		model = glm::mat4(1);
		model = glm::scale(model, glm::vec3(30, 7, 30));
		floor_shader.use();
		floor_shader.setMat4f("projection", (projection));
		floor_shader.setMat4f("view", view);
		floor_shader.setMat4f("model", model);
		floor_shader.setMat4f("light_space_trans", light_space_trans);
		floor_shader.setTexUnit("material.floor_texture", 0);
		floor_shader.setVec3f("material.specular", 0.5f, 0.5f, 0.5f);
		floor_shader.setFloat("material.shininess", 32.0f);
		floor_shader.setVec3f("light.ambient", (ambient_color));
		floor_shader.setVec3f("light.diffuse", (diffuse_color));
		floor_shader.setVec3f("light.specular", 1.f, 1.f, 1.f);
		floor_shader.setVec3f("light.position", (light_position));
		floor_shader.setVec3f("viewPos", (camera_pos));
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture_2ds_[0].getTextureId());
		floor_shader.setTexUnit("depth_map", 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, depth_map_);
		RenderFloor();

		// 天空盒
		glDepthFunc(GL_LEQUAL);
		auto& skybox_shader = shaders_[4];
		skybox_shader.use();
		auto& no_translation_view = glm::mat4(glm::mat3(view));
		skybox_shader.setMat4f("view", (no_translation_view));
		skybox_shader.setMat4f("projection", (projection));
		skybox_shader.setTexUnit("skybox_sampler", 0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubes_[0].getTextureId());
		RenderSkyBox();
		glDepthFunc(GL_LESS); // set depth function back to default
#endif // DEBUG
	}
}




