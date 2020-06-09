#ifndef __CAMERA_HPP__
#define __CAMERA_HPP__

#include <GL/gl3w.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <global_func.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD_,
	BACKWARD_,
	LEFT_,
	RIGHT_,
	UP_,
	DOWN_,
};

class Camera {
public:
	glm::vec3 position_{ glm::vec3(0.0f,0.0f,0.0f) };
	glm::vec3 front_{ glm::vec3(0.0f,0.0f,-1.0f) }; 
	glm::vec3 up_{ glm::vec3(0.0f,1.0f,0.0f) };		
	glm::vec3 right_{ glm::vec3(1.f, 0.f, 0.f) };  
	glm::vec3 worldup_{ glm::vec3(0.0f,1.0f,0.0f) };

	float yaw_{ -90.f };
	float pitch_{ 0.f };

	double movement_speed_{ 2.5f }; // 移动速度
	double mouse_sensitivity_{ 0.1f }; // 鼠标灵敏度
	double zoom_{ 60.0f }; // fov角度


	Camera() {}
	Camera(const glm::vec3& position, const glm::vec3& aim)
	{
		setCameraPos(position, aim);
	}
	void setCameraPos(const glm::vec3& position, const glm::vec3& aim) 
	{
		position_ = position;
		front_ = glm::normalize(aim - position_);
		right_ = glm::normalize(glm::cross(front_, worldup_));
		up_ = glm::normalize(glm::cross(right_, front_));

		pitch_ = std::asin(front_.y);
		double l = std::cos(pitch_);
		pitch_ = glm::degrees(pitch_);
		if (l < std::numeric_limits<float>::epsilon())
			throw std::exception("pitch angle near +-90 degrees!");
		else {
			yaw_ = global::sign_positive(front_.z) * std::acos(
				global::clamp(front_.x / l, 1., -1.)
			);
			yaw_ = glm::degrees(yaw_);
		}
	}
	glm::vec3 getPosition(void) const {
		return position_;
	}
	void setMoveSpeed(double speed = 2.5) {
		movement_speed_ = speed;
	}

	glm::mat4 getViewMatrix() {
		return glm::lookAt(position_, position_ + front_, up_);
	}
	void ProcessKeyboard(Camera_Movement direction, double deltaTime)
	{
		float velocity = static_cast<float>(movement_speed_ * deltaTime);

		if (direction == FORWARD_)
			position_ += front_ * velocity;
		else if (direction == BACKWARD_)
			position_ -= front_ * velocity;
		else if (direction == LEFT_)
			position_ -= right_ * velocity;
		else if (direction == RIGHT_)
			position_ += right_ * velocity;
		else if (direction == UP_)
			position_ += up_ * velocity;
		else if (direction == DOWN_)
			position_ -= up_ * velocity;
	}
	void ProcessMouseMovement(double xoffset, double yoffset)
	{
		xoffset *= mouse_sensitivity_;
		yoffset *= mouse_sensitivity_;
		
		pitch_ += yoffset;
		yaw_ += xoffset;
		pitch_ = global::clamp(pitch_, 89.9f, -89.9f);

		front_.y = static_cast<float>(std::sin(glm::radians(pitch_)));
		front_.x = static_cast<float>(std::cos(glm::radians(pitch_)) * std::cos(glm::radians(yaw_)));
		front_.z = static_cast<float>(std::cos(glm::radians(pitch_)) * std::sin(glm::radians(yaw_)));

		front_ = glm::normalize(front_);
		right_ = glm::normalize(glm::cross(front_, worldup_));
		up_ = glm::normalize(glm::cross(right_, front_));
	}

	void ProcessMouseScroll(double yoffset)
	{
		if (zoom_ >= 1.0f && zoom_ <= 80.0f)
			zoom_ -= yoffset;
		if (zoom_ <= 1.0f)
			zoom_ = 1.0f;
		if (zoom_ >= 80.0f)
			zoom_ = 80.0f;
	}
};

#endif // __CAMERA_HPP__