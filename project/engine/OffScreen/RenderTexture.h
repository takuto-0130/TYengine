#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include "struct.h"

namespace TYEngine
{
	namespace Core
	{
		class DirectXBasis;
	}
	namespace Graphics
	{
		class SrvManager;
	}
}

namespace TYEngine
{
	namespace OffScreen
	{

		/// <summary>
		/// 描画結果をテクスチャとして保持するためのレンダーターゲットクラス。  
		/// 主にポストエフェクトやオフスクリーン描画に使用する。
		/// </summary>
		class RenderTexture
		{
		public:
			/// <summary>
			/// レンダーテクスチャを初期化する  
			/// RTV・SRV・DSVの生成とリソース確保を行う。
			/// </summary>
			/// <param name="dxBasis">DirectX基盤クラス（デバイス・コマンド関連）</param>
			/// <param name="srvManager">SRV管理クラス</param>
			/// <param name="width">テクスチャ幅</param>
			/// <param name="height">テクスチャ高さ</param>
			/// <param name="format">カラーフォーマット（例：DXGI_FORMAT_R8G8B8A8_UNORM）</param>
			/// <param name="clearColor">クリアカラー</param>
			void Initialize(Core::DirectXBasis* dxBasis, Graphics::SrvManager* srvManager,
				uint32_t width, uint32_t height,
				DXGI_FORMAT format, const Utility::Vector4& clearColor);

			/// <summary>
			/// このレンダーターゲットへの描画を開始する  
			/// RTVおよびDSVをセットし、レンダーターゲットをクリアする。
			/// </summary>
			void BeginRender();

			/// <summary>
			/// このレンダーターゲットへの描画を終了する  
			/// 描画結果をSRVとして利用できるように遷移させる。
			/// </summary>
			void EndRender();

			/// <summary>
			/// 深度バッファをSRV（シェーダリソース）として使用可能な状態に遷移させる
			/// </summary>
			void TransitionDepthToSRV();

			/// <summary>
			/// 深度バッファを書き込み可能な状態（DSV）に遷移させる
			/// </summary>
			void TransitionDepthToWrite();

			/// <summary>
			/// テクスチャリソースを取得する
			/// </summary>
			ID3D12Resource* GetResource() const { return texture_.Get(); }

			/// <summary>
			/// テクスチャのGPUハンドル（SRV）を取得する
			/// </summary>
			D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle() const;

			/// <summary>
			/// SRVインデックスを取得する
			/// </summary>
			uint32_t GetSRVIndex() const { return srvIndex_; }

			/// <summary>
			/// 深度テクスチャのGPUハンドル（SRV）を取得する
			/// </summary>
			D3D12_GPU_DESCRIPTOR_HANDLE GetDepthSRVHandle() const;

		private:
			/// <summary>
			/// 深度ステンシルバッファを生成する  
			/// 深度用リソースとDSV/SRVを作成。
			/// </summary>
			void CreateDepthStencil();

		private:
			/// <summary>DirectX基盤クラス（デバイス、コマンド管理）</summary>
			Core::DirectXBasis* dxBasis_ = nullptr;

			/// <summary>SRVを管理するマネージャ</summary>
			Graphics::SrvManager* srvManager_ = nullptr;

			/// <summary>テクスチャリソース</summary>
			Microsoft::WRL::ComPtr<ID3D12Resource> texture_;

			/// <summary>RTV用ディスクリプタヒープ</summary>
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;

			/// <summary>RTVハンドル</summary>
			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};

			/// <summary>SRVのインデックス</summary>
			uint32_t srvIndex_ = 0;

			/// <summary>カラーフォーマット</summary>
			DXGI_FORMAT format_ = DXGI_FORMAT_R8G8B8A8_UNORM;

			/// <summary>クリアカラー</summary>
			Utility::Vector4 clearColor_ = { 0, 0, 0, 1 };

			/// <summary>テクスチャの幅</summary>
			uint32_t width_ = 0;

			/// <summary>テクスチャの高さ</summary>
			uint32_t height_ = 0;

			/// <summary>現在のリソースステート</summary>
			D3D12_RESOURCE_STATES currentState_ = D3D12_RESOURCE_STATE_RENDER_TARGET;

			/// <summary>深度バッファリソース</summary>
			Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;

			/// <summary>DSV用ディスクリプタヒープ</summary>
			Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;

			/// <summary>DSVハンドル</summary>
			D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle_{};

			/// <summary>深度用SRVのインデックス</summary>
			uint32_t depthSRVIndex_ = 0;
		};

	} // namespace OffScreen
} // namespace TYEngine

