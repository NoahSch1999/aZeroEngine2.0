#pragma once
#include "../D3D12Core.h"

namespace aZero
{
	class Camera
	{
	public:
		enum class PROJECTIONTYPE { PERSPECTIVE, ORTOGRAPHIC };

	private:
		DXM::Vector3 m_position = DXM::Vector3::Zero;
		DXM::Matrix m_rotation = DXM::Matrix::Identity;
		float m_fov = 90.f; // Degrees
		float m_nearPlane = 0.1f;
		float m_farPlane = 100.f;
		DXM::Vector2 m_viewDimensions = DXM::Vector2::Zero;
		PROJECTIONTYPE m_projectionType = PROJECTIONTYPE::PERSPECTIVE;

	public:
		Camera() = default;

		Camera(const PROJECTIONTYPE projectionType, const DXM::Vector3& startPosition = DXM::Vector3::Zero,
			const DXM::Vector3& startDirection = DXM::Vector3(0.f, 0.f, 1.f),
			const DXM::Vector2& viewDimensions = DXM::Vector2(500.f, 500.f),
			const float nearPlane = 0.1f, const float farPlane = 100.f, const float fov = 90.f)
			:m_projectionType(projectionType), m_position(startPosition), m_viewDimensions(viewDimensions),
			m_nearPlane(nearPlane), m_farPlane(farPlane), m_fov(fov)
		{
			LookAt(startDirection);
		}

		~Camera()
		{

		}
		
		void SetProjectionType(const Camera::PROJECTIONTYPE projectionType)
		{
			m_projectionType = projectionType;
		}

		void SetFov(const float fov)
		{
			m_fov = fov;
		}

		void SetPlanes(const float nearPlane, const float farPlane)
		{
			m_nearPlane = nearPlane;
			m_farPlane = farPlane;
		}

		void SetPosition(const DXM::Vector3& position) 
		{ 
			m_position = position; 
		}

		void MoveRelative(const DXM::Vector3& offset)
		{
			m_position += offset;
		}

		void SetRotation(const float pitch, const float yaw, const float roll)
		{
			m_rotation = DXM::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
		}

		void RotateRelative(const float pitch, const float yaw, const float roll)
		{
			m_rotation = m_rotation * DXM::Matrix::CreateFromYawPitchRoll(yaw, pitch, roll);
		}

		void LookAt(const DXM::Vector3& targetPosition)
		{
			DXM::Vector3 targetDirection = targetPosition - m_position;

			if (targetDirection.Length() > 0.0001f)
			{
				return;
			}

			targetDirection.Normalize();

			DXM::Vector3 currentForward(DXM::Vector3::Transform({ 0.f, 0.f, 1.f }, m_rotation));
			currentForward.Normalize();

			float rotationAngle = acos(currentForward.Dot(targetDirection));

			DXM::Vector3 rotationAxis = currentForward.Cross(targetDirection);
			rotationAxis.Normalize();

			m_rotation = DXM::Matrix::CreateFromAxisAngle(rotationAxis, rotationAngle);
		}

		DXM::Matrix GetTransform() const
		{
			DXM::Matrix finalMatrix = m_rotation;
			finalMatrix.m[3][0] = m_position.x;
			finalMatrix.m[3][1] = m_position.y;
			finalMatrix.m[3][2] = m_position.z;
			finalMatrix.m[3][3] = 1.f;
			return finalMatrix;
		}

		DXM::Matrix GetProjectionMatrix() const
		{
			if (m_projectionType == Camera::PROJECTIONTYPE::ORTOGRAPHIC)
			{
				return DXM::Matrix::CreateOrthographic(m_viewDimensions.x, m_viewDimensions.y, m_nearPlane, m_farPlane);
			}

			return DXM::Matrix::CreatePerspectiveFieldOfView(m_fov, m_viewDimensions.x / m_viewDimensions.y, m_nearPlane, m_farPlane);
		}
	};
}