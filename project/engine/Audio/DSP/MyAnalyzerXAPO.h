#pragma once
#include <xapobase.h>
#include <wrl.h>
#include <vector>
#include <atomic>

#include "BiquadFilter.h"

namespace TYEngine
{
	namespace AudioSystem
	{

		class Audio;

		/// <summary>
		/// カスタムXAPO（Cross-Platform Audio Processing Object）。
		/// オーディオストリームに挿入し、解析用データ（波形、FFT）を取得する。
		/// </summary>
		class __declspec(uuid("2dde0a3b-45d5-4a48-a9e6-a3a8129ef91a"))
			MyAnalyzerXAPO : public Microsoft::WRL::RuntimeClass<
			Microsoft::WRL::RuntimeClassFlags<Microsoft::WRL::ClassicCom>,
			Microsoft::WRL::FtmBase>,
			public CXAPOParametersBase
		{
		private:
			friend class Audio;
			friend class Microsoft::WRL::Details::MakeAllocator<MyAnalyzerXAPO>;
		public:
			// あいまいさを解決するため、IUnknown のメソッドを明示的に委譲する
			STDMETHOD(QueryInterface)(REFIID riid, _COM_Outptr_ void** ppvObject) override
			{
				if (!ppvObject) return E_POINTER;
				return CXAPOParametersBase::QueryInterface(riid, ppvObject);
			}
			STDMETHOD_(ULONG, AddRef)() override
			{
				return CXAPOParametersBase::AddRef();
			}
			STDMETHOD_(ULONG, Release)() override
			{
				return CXAPOParametersBase::Release();
			}
			MyAnalyzerXAPO();
			~MyAnalyzerXAPO();

			static const UINT32 FFT_SIZE = 1024;     ///< FFTサンプルサイズ
			static const UINT32 DELAY_FRAMES = 5;    ///< 遅延フレーム数
			static const UINT32 WAVEFORM_SIZE = 441; ///< 波形バッファサイズ
			static const UINT32 BEAT_ANALYE_BUFFER = 64;

			enum EQBand
			{
				LPF,
				HPF,
				BPF,
				Low,
				Mid,
				High,
				BandNum
			};

		public:
			/// <summary>
			/// プロセス前のロック処理。
			/// フォーマットの確認やパラメータの更新を行う。
			/// </summary>
			HRESULT __stdcall LockForProcess(
				UINT32 inputLockedParameterCount,
				const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* inputLockedParameters,
				UINT32 outputLockedParameterCount,
				const XAPO_LOCKFORPROCESS_BUFFER_PARAMETERS* outputLockedParameters
			) override;

			/// <summary>
			/// オーディオ処理の実行。
			/// ストリームからデータをコピーし、解析用バッファに格納する。
			/// </summary>
			void __stdcall Process(
				UINT32 InputProcessParameterCount,
				const XAPO_PROCESS_BUFFER_PARAMETERS* pInputProcessParameters,
				UINT32 OutputProcessParameterCount,
				XAPO_PROCESS_BUFFER_PARAMETERS* pOutputProcessParameters,
				BOOL IsEnabled
			) override;

			// スレッドセーフに最新波形データを取得する
			void GetLatestWaveform(std::vector<float>& dest, size_t count);

			// サンプリングレートとチャンネル数のゲッター
			UINT32 GetSampleRate() const { return sampleRate_; }
			UINT32 GetChannels() const { return channels_; }

			// アトミックなイコライザーパラメータのセッター・ゲッター
			void SetEQGain(float low, float mid, float high) {
				lowGain_.store(low, std::memory_order_relaxed);
				midGain_.store(mid, std::memory_order_relaxed);
				highGain_.store(high, std::memory_order_relaxed);
				parametersChanged_.store(true, std::memory_order_release);
			}
			void GetEQGain(float& low, float& mid, float& high) const {
				low = lowGain_.load(std::memory_order_relaxed);
				mid = midGain_.load(std::memory_order_relaxed);
				high = highGain_.load(std::memory_order_relaxed);
			}
			void SetFiltersHz(float lp, float hp, float bp) {
				LPHz_.store(lp, std::memory_order_relaxed);
				HPHz_.store(hp, std::memory_order_relaxed);
				BPHz_.store(bp, std::memory_order_relaxed);
				parametersChanged_.store(true, std::memory_order_release);
			}
			void GetFiltersHz(float& lp, float& hp, float& bp) const {
				lp = LPHz_.load(std::memory_order_relaxed);
				hp = HPHz_.load(std::memory_order_relaxed);
				bp = BPHz_.load(std::memory_order_relaxed);
			}

			std::vector<BiquadFilter>& GetBiquadFilter(EQBand type) { return eqFilters_[type]; }

		private:
			void UpdateEQ();

		private:
			/// <summary>入力チャンネル数。</summary>
			UINT32 channels_ = 0;

			// パラメータ更新通知用フラグ
			std::atomic<bool> parametersChanged_{true};

			// スレッドセーフリングバッファ
			std::vector<float> ringBuffer_;
			std::atomic<size_t> writeIndex_{0};

			// イコライザー用フィルタ (外側: バンド数, 内側: チャンネル数)
			// 例: index 0=Low, 1=Mid, 2=High
			std::vector<std::vector<BiquadFilter>> eqFilters_;

			// 現在のサンプリングレート
			UINT32 sampleRate_ = 44100;
			// Gain 用パラメータ
			std::atomic<float> lowGain_ = 0.0f;
			std::atomic<float> midGain_ = 0.0f;
			std::atomic<float> highGain_ = 0.0f;
			std::atomic<float> LPHz_ = 16000.0f;
			std::atomic<float> HPHz_ = 10.0f;
			std::atomic<float> BPHz_ = 2000.0f;

		};

	} // namespace Audio
} // namespace TYEngine
