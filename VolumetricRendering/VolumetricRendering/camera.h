#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

enum Camera_Movement {
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT
};

const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2000.f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 60.0f;
const float MAX_FOV = 100.0f;

class Camera
{
public:
	glm::vec3 m_position;
	glm::vec3 m_front;
	glm::vec3 m_up;
	glm::vec3 m_right;
	glm::vec3 m_world_up;
	
	float Yaw;
	float Pitch;
	
	float MovementSpeed;
	float MouseSensitivity;
	float Zoom;

	Camera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : m_front(glm::vec3(0.0f, 0.0f, -5.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		m_position = position;
		m_world_up = up;
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}
	
	Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : m_front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
	{
		m_position = glm::vec3(posX, posY, posZ);
		m_world_up = glm::vec3(upX, upY, upZ);
		Yaw = yaw;
		Pitch = pitch;
		updateCameraVectors();
	}

	glm::mat4 getViewMatrix()
	{
		updateCameraVectors();
		return glm::lookAt(m_position, m_position + m_front, m_up);
	}

	void processKeyboard(Camera_Movement direction, float deltaTime)
	{
		float velocity = MovementSpeed * deltaTime;
		if (direction == FORWARD)
			m_position += m_front * velocity;
		if (direction == BACKWARD)
			m_position -= m_front * velocity;
		if (direction == LEFT)
			m_position -= m_right * velocity;
		if (direction == RIGHT)
			m_position += m_right * velocity;
	}


	float random2D(glm::vec2 st)
	{
		return glm::fract(glm::sin(glm::dot(st, glm::vec2(12.9898f, 78.233f))) * 43758.5453123f);
	}

	float interpolatedNoise(int ind, float x, float y) 
	{
		float integer_X = floor(x);
		float fractional_X = glm::fract(x);
		float integer_Y = floor(y);
		float fractional_Y = glm::fract(y);

		glm::vec2 randomInput = glm::vec2(integer_X, integer_Y);

		float a = random2D(randomInput);
		float b = random2D(randomInput + glm::vec2(1.0, 0.0));
		float c = random2D(randomInput + glm::vec2(0.0, 1.0));
		float d = random2D(randomInput + glm::vec2(1.0, 1.0));

		glm::vec2 w = glm::vec2(fractional_X, fractional_Y);
		w = w * w * w * (10.0f + w * (-15.0f + 6.0f * w));

		float k0 = a,
			k1 = b - a,
			k2 = c - a,
			k3 = d - c - b + a;

		return k0 + k1 * w.x + k2 * w.y + k3 * w.x * w.y;
	}

	float perlin(glm::vec2 st, int octaves, float freq, float gDispFactor) 
	{
		glm::mat2 m;
		m[0][0] = 0.8;
		m[0][1] = -0.6;
		m[1][0] = 0.6;
		m[1][1] = 0.8;

		float persistence = 0.5;
		float total = 0.0,
			frequency = 0.005 * freq,
			amplitude = gDispFactor;

		for (int i = 0; i < octaves; ++i) 
		{
			frequency *= 2.0;
			amplitude *= persistence;

			glm::vec2 v = frequency * st;

			total += interpolatedNoise(0, v.x, v.y) * amplitude;
		}

		return total * total * total;
	}

	void projectCameraPosition() {
		glm::vec2 st = glm::vec2(m_position.x, m_position.z);
		float y = perlin(st, 13, 0.01, 20.);
		m_position.y = y;
	}

	void processMouseMovement(float xoffset, float yoffset, bool constrainPitch = true)
	{
		xoffset *= MouseSensitivity;
		yoffset *= MouseSensitivity;

		Yaw += xoffset;
		Pitch += yoffset;

		if (constrainPitch)
		{
			if (Pitch > 89.0f)
				Pitch = 89.0f;
			if (Pitch < -89.0f)
				Pitch = -89.0f;
		}

		updateCameraVectors();
	}

	void processMouseScroll(float yoffset)
	{
		if (Zoom >= 1.0f && Zoom <= MAX_FOV)
			Zoom -= yoffset;
		if (Zoom <= 1.0f)
			Zoom = 1.0f;
		if (Zoom >= MAX_FOV)
			Zoom = MAX_FOV;
	}

	void invertPitch() 
	{
		this->Pitch = -Pitch;
		updateCameraVectors();
	}

private:
	void updateCameraVectors()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		front.y = sin(glm::radians(Pitch));
		front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
		m_front = glm::normalize(front);
		m_right = glm::normalize(glm::cross(m_front, m_world_up));
		m_up = glm::normalize(glm::cross(m_right, m_front));
	}
};