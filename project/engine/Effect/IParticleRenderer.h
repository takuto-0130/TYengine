#pragma once
#include "DirectXBasis.h"
#include "SrvManager.h"
#include "Camera.h"
#include "TextureManager.h"
#include "ParticleParam.h"
#include "IParticleBehaviour.h"
#include <struct.h>
#include <random>
#include <list>
#include <wrl.h>
#include <numbers>

namespace TYEngine
{
	namespace Effect
	{

		/// <summary>
		/// パーティクル描画の基底クラス。
		/// GPUリソース管理、インスタンシング描画、エミッター制御の共通機能を提供する。
		/// 具象クラス（PlaneParticleなど）でリソース生成や初期化の実装を行う。
		/// </summary>
		class IParticleRenderer
		{
		public:
			virtual ~IParticleRenderer();

			/// <summary>
			/// 初期化処理。
			/// </summary>
			/// <param name="dx">DirectX基盤。</param>
			/// <param name="srv">SRVマネージャ。</param>
			/// <param name="cam">カメラ。</param>
			virtual void Initialize(Core::DirectXBasis* dx, Graphics::SrvManager* srv, TYEngine::CameraSystem::Camera* cam);

			/// <summary>更新処理。</summary>
			virtual void Update();

			/// <summary>描画処理。</summary>
			virtual void Draw();


			/// <summary>
			/// パーティクル発生装置（エミッター）の設定構造体。
			/// </summary>
			struct Emitter
			{
				Utility::Transform transform{        ///< 発生源のトランスフォーム（位置・回転・スケール）
					{1.0f, 1.0f, 1.0f},
					{0.0f, 0.0f, 0.0f},
					{0.0f, 0.0f, 0.0f}
				};
				uint32_t count = 5;         ///< 一度の発生数
				float frequency = 0.5f;     ///< 発生頻度（秒）
				float frequencyTime = 0.0f; ///< 発生タイマー
				Utility::Vector4 color = { 1,1,1,1 };///< 基本カラー
				Utility::Vector3 velocity = { 0.0f, 0.0f, 0.0f }; ///< 基本初速度
				bool randomVel = false;     ///<速度をランダムにするか
			};

			virtual void SetEmitter(Emitter& emitter) { emitter_ = emitter; }

			virtual void TriggerEmit();

			void SetBehaviour(std::unique_ptr<IParticleBehaviour> b)
			{
				behaviour_ = std::move(b);
			}

			struct ParticleP
			{
				Utility::Transform transform;
				Utility::Vector3 velocity;
				Utility::Vector4 color;
				float lifeTime;
				float currentTime;
			};
		protected:

			struct ParticleForGPU
			{
				Utility::Matrix4x4 WVP;
				Utility::Matrix4x4 World;
				Utility::Vector4 color;
			};

			struct CameraForGPUP
			{
				Utility::Vector3 worldPosition;
			};

			struct VertexData
			{
				Utility::Vector4 position;
				Utility::Vector2 texCoord;
				Utility::Vector3 normal;
			};
			struct Material
			{
				Utility::Vector4 color;
				int32_t enableLighting;
				float padding[3];
				Utility::Matrix4x4 uvTransform;
			};

			virtual void CreateResources() = 0;

			void CreateRootSignature();
			void LoadShader();
			void CreatePipelineState();
			virtual ParticleParam MakeNewParticle(std::mt19937& random, const Emitter& emitter);
			virtual std::list<ParticleParam> Emit(std::mt19937& random);

		protected:
			float kDeltaTime = 1.0f / 60.0f;

			static const uint32_t kMaxInstance = 32767;

			bool useBillboard_ = true;

			bool useTrigger_ = true;

			uint32_t vertexCount_ = 0;

			Core::DirectXBasis* dxBasis_ = nullptr;
			Graphics::SrvManager* srvManager_ = nullptr;
			TYEngine::CameraSystem::Camera* camera_ = nullptr;

			Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource_;
			Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
			Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource_;
			Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;

			D3D12_VERTEX_BUFFER_VIEW vertexBufferView_{};
			uint32_t srvIndex_ = 0;
			uint32_t textureIndex_ = 0;

			ParticleForGPU* instancingData_ = nullptr;
			Material* materialData_ = nullptr;
			CameraForGPUP* cameraData_ = nullptr;
			VertexData* vertexData_ = nullptr;

			std::list<ParticleParam> particles_;
			std::random_device seedGene_;
			uint32_t numInstance_ = 0;

			Emitter emitter_;

			Utility::Transform transform_{};

			Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
			Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState_;
			Microsoft::WRL::ComPtr<IDxcBlob> vsBlob_;
			Microsoft::WRL::ComPtr<IDxcBlob> psBlob_;

			D3D12_BLEND_DESC blendDesc_{};
			D3D12_RASTERIZER_DESC rasterizerDesc_{};

			std::unique_ptr<IParticleBehaviour> behaviour_;
		};

	} // namespace Effect
} // namespace TYEngine
