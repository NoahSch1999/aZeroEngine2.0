#pragma once
#include "../../Core/AssetTypes/MaterialAsset.h"

namespace aZero
{
	namespace Component
	{
		class UniqueTagComponent
		{
		private:
			std::string m_Tag;

		public:
			UniqueTagComponent(const std::string& Tag)
				:m_Tag(Tag)
			{

			}

			void SetString(const std::string& Tag)
			{
				m_Tag = Tag;
			}

			const std::string& GetString() const
			{
				return m_Tag;
			}
		};

		class TransformComponent
		{
		private:
			bool m_IsDirty = false;
			DXM::Matrix m_WorldMatrix = DXM::Matrix::Identity;
			DXM::Vector3 m_Position = DXM::Vector3::Zero;
			DXM::Vector3 m_Rotation = DXM::Vector3::Zero;
			DXM::Vector3 m_Scale = DXM::Vector3(1.f, 1.f, 1.f);

		public:
			TransformComponent() = default;

			void SetPosition(const DXM::Vector3& Position)
			{
				m_IsDirty = true;
				m_Position = Position;
			}

			void SetPositionRelative(const DXM::Vector3& Position)
			{
				SetPosition(m_Position + Position);
			}

			void SetRotation(const DXM::Vector3& Rotation)
			{
				m_IsDirty = true;
				m_Rotation = Rotation;
			}

			void SetRotationRelative(const DXM::Vector3& Rotation)
			{
				SetPosition(m_Rotation + Rotation);
			}

			void SetScale(double UniformScale)
			{
				m_IsDirty = true;
				m_Scale.x = UniformScale;
				m_Scale.y = UniformScale;
				m_Scale.z = UniformScale;
			}

			void SetScaleRelative(double UniformScale)
			{
				m_IsDirty = true;
				m_Scale.x += UniformScale;
				m_Scale.y += UniformScale;
				m_Scale.z += UniformScale;
			}

			void SetScale(const DXM::Vector3& Scale)
			{
				m_IsDirty = true;
				m_Scale = Scale;
			}

			void SetScaleRelative(const DXM::Vector3& Scale)
			{
				SetScale(m_Scale + Scale);
			}

			DXM::Vector3 GetPosition() const { return m_Position; }
			DXM::Vector3 GetRotation() const { return m_Rotation; }
			DXM::Vector3 GetScale() const { return m_Scale; }

			DXM::Matrix GetWorldMatrix() 
			{ 
				if (m_IsDirty)
				{
					m_IsDirty = false;

					const DXM::Matrix ScaleMatrix = DXM::Matrix::CreateScale(m_Scale);
					const DXM::Matrix RotationMatrix = DXM::Matrix::CreateFromYawPitchRoll(m_Rotation.y, m_Rotation.x, m_Rotation.z);
					const DXM::Matrix TranslationMatrix = DXM::Matrix::CreateTranslation(m_Position);
					m_WorldMatrix = ScaleMatrix * RotationMatrix * TranslationMatrix;

					return m_WorldMatrix;
				}
				else
				{
					return m_WorldMatrix;
				}
			}
		};

		struct MeshComponent
		{
			std::string MeshKey; // TODO - Should be Int
		};

		struct MaterialComponent
		{
			std::string MaterialKey; // TODO - Should be Int
		};

		struct PointLightComponent
		{
			DXM::Vector3 Position;
			float Intensity = 1.f;

			DXM::Vector3 Color;
			float Pad;
		};

		struct SpotLightComponent
		{
			DXM::Vector3 Position;
			float Intensity = 1.f;

			DXM::Vector3 Color;
			float ConeRadius = 1.f;

			DXM::Vector3 Direction;
			float Pad;
		};

		struct DirectionalLightComponent
		{
			DXM::Vector3 Direction;
			float Intensity = 1.f;

			DXM::Vector3 Color;
			float Pad;
		};

		struct RigidBodyComponent
		{
			int dummy = 0;
		};
	}
}