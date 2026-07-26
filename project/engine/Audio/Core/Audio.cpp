#include "Audio.h"
#include "BPMDetector.h"
#include "Logger.h"
#include "StringUtility.h"
#include <mfapi.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include <cassert>

#pragma comment(lib, "mfplat.lib")
#pragma comment(lib, "Mfreadwrite.lib")
#pragma comment(lib, "mfuuid.lib")

namespace TYEngine
{
	namespace AudioSystem
	{

		using namespace TYEngine::Debugger;

		Audio::~Audio()
		{
			// BGMリソースの解放
			for (auto SourceVoice : sourceVoices_)
			{
				if (SourceVoice != nullptr)
				{
					SourceVoice->DestroyVoice();
					SourceVoice = nullptr;
				}
			}

			// 無音ボイスの破棄
			if (silentVoice_)
			{
				silentVoice_->DestroyVoice();
				silentVoice_ = nullptr;
			}
			for (auto& [name, voice] : categorySilentVoices_)
			{
				if (voice)
				{
					voice->DestroyVoice();
				}
			}
			categorySilentVoices_.clear();


			// カテゴリ用 SubmixVoices の破棄
			for (auto& [name, voice] : categorySubmixVoices_)
			{
				if (voice)
				{
					voice->DestroyVoice();
				}
			}
			categorySubmixVoices_.clear();

			// 解析用 SubmixVoice の破棄
			if (analyzerSubmix_)
			{
				analyzerSubmix_->DestroyVoice();
				analyzerSubmix_ = nullptr;
			}

			if (masterVoice_ != nullptr)
			{
				masterVoice_->DestroyVoice();
				masterVoice_ = nullptr;
			}

			soundDataMap_.clear();

			HRESULT result = MFShutdown();
			if (FAILED(result))
			{
				// ログに警告を出す（デバッグ用）
				Log("Audio Warning: Failed to shutdown media foundation.\n");

				assert(SUCCEEDED(result));
			}


			xAudio2_.Reset();
		}

		void Audio::Initialize(const std::string& directoryPath)
		{
			directoryPath_ = directoryPath;
			HRESULT result;
			// XAudio2エンジンのインスタンス生成
			result = XAudio2Create(&xAudio2_, 0, XAUDIO2_DEFAULT_PROCESSOR);
			assert(SUCCEEDED(result));

			// マスターボイス（最終出力）の生成
			result = xAudio2_->CreateMasteringVoice(&masterVoice_);

			if (FAILED(result))
			{
				// ログに警告を出す（デバッグ用）
				Log("Audio Warning: Failed to initialize audio client.\n");

				assert(SUCCEEDED(result));
			}

			result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);

			if (FAILED(result))
			{
				// ログに警告を出す（デバッグ用）
				Log("Audio Warning: Failed to initialize media foundation.\n");

				assert(SUCCEEDED(result));
			}

			// 解析用サブミックスボイスの作成（周波数解析用）
			CreateAnalyzerSubmix();
			Start();
		}

		void Audio::CreateAnalyzerSubmix()
		{
			// ============================================
			//   ★ SubmixVoice を作る
			// ============================================

			// MasterVoice と同じフォーマットで作る
			XAUDIO2_VOICE_DETAILS details = {};
			masterVoice_->GetVoiceDetails(&details);

			HRESULT result;

			result = xAudio2_->CreateSubmixVoice(
				&analyzerSubmix_,
				details.InputChannels,
				details.InputSampleRate,
				0,
				kAnalyzer // ProcessingStage (下から上へ)
			);
			assert(SUCCEEDED(result));

			// Submix → MasteringVoice へ音を送る
			XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
			sendDesc.Flags = 0;
			sendDesc.pOutputVoice = masterVoice_;

			XAUDIO2_VOICE_SENDS sends = {};
			sends.SendCount = 1;
			sends.pSends = &sendDesc;

			result = analyzerSubmix_->SetOutputVoices(&sends);
			assert(SUCCEEDED(result));

			// ============================================
			//   ★ SubmixVoice に XAPO を付ける
			// ============================================

			// XAPO インスタンス
			analyzerXAPO_ = Microsoft::WRL::Make<MyAnalyzerXAPO>();

			// Effect desc
			XAUDIO2_EFFECT_DESCRIPTOR effectDesc = {};
			effectDesc.InitialState = TRUE;
			effectDesc.OutputChannels = details.InputChannels;
			effectDesc.pEffect = static_cast<IXAPO*>(analyzerXAPO_.Get());

			// Effect chain
			XAUDIO2_EFFECT_CHAIN effectChainXAPO = {};
			effectChainXAPO.EffectCount = 1;
			effectChainXAPO.pEffectDescriptors = &effectDesc;

			// SubmixVoice に XAPO をセット
			result = analyzerSubmix_->SetEffectChain(&effectChainXAPO);
			assert(SUCCEEDED(result));
		}

		void Audio::Start()
		{
			xAudio2_->StartEngine();

			analyzerSubmix_->GetVoiceDetails(&analyzerDetails_);

			// ---- ★無音バッファ用 Voice を作成（解析器への入力維持用） ----
			silentFormat_.wFormatTag = WAVE_FORMAT_IEEE_FLOAT;
			silentFormat_.nChannels = 1;
			silentFormat_.nSamplesPerSec = analyzerDetails_.InputSampleRate;
			silentFormat_.wBitsPerSample = 32;
			silentFormat_.nBlockAlign = 4;
			silentFormat_.nAvgBytesPerSec = analyzerDetails_.InputSampleRate * 4;
			silentFormat_.cbSize = 0;

			// 無音バッファ（50ms程度）
			int silentSamples = int(analyzerDetails_.InputSampleRate * 0.05f);
			silentBuffer_.assign(silentSamples, 0.0f);

			CreateSilentVoice(silentVoice_, analyzerSubmix_);
			
			EnableSilentFeed(true);
		}

		void Audio::CreateSilentVoice(IXAudio2SourceVoice*& voice, IXAudio2SubmixVoice* sendSubmix)
		{
			HRESULT result;

			XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
			sendDesc.Flags = 0;
			sendDesc.pOutputVoice = sendSubmix;

			XAUDIO2_VOICE_SENDS sends = {};
			sends.SendCount = 1;
			sends.pSends = &sendDesc;

			result = xAudio2_->CreateSourceVoice(
				&voice,
				&silentFormat_,
				0, 2.0f,
				nullptr, &sends, nullptr
			);

			assert(SUCCEEDED(result));

			voice->Start(0);
		}

		void Audio::AddSoundCategory(const std::string& soundCategory)
		{
			if (categorySubmixVoices_.count(soundCategory))
			{
				// キーが存在する場合、処理を中断
				return;
			}

			HRESULT result;

			// analyzerSubmix と同じフォーマットで作る
			XAUDIO2_VOICE_DETAILS details = {};
			analyzerSubmix_->GetVoiceDetails(&details);

			// ボイス
			IXAudio2SubmixVoice* voice;

			XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
			sendDesc.Flags = 0;
			sendDesc.pOutputVoice = analyzerSubmix_;

			XAUDIO2_VOICE_SENDS sends = {};
			sends.SendCount = 1;
			sends.pSends = &sendDesc;

			result = xAudio2_->CreateSubmixVoice(
				&voice,
				details.InputChannels,
				details.InputSampleRate,
				0,
				kCategory,          // ProcessingStage (下から上へ)
				&sends,
				nullptr
			);
			assert(SUCCEEDED(result));

			categorySubmixVoices_[soundCategory] = voice;

			// ============================================
			//   ★ SubmixVoice に XAPO を付ける
			// ============================================

			// XAPO インスタンス
			categoryAnalyzerXAPO_[soundCategory] = Microsoft::WRL::Make<MyAnalyzerXAPO>();

			// Effect desc
			XAUDIO2_EFFECT_DESCRIPTOR effectDesc = {};
			effectDesc.InitialState = TRUE;
			effectDesc.OutputChannels = details.InputChannels;
			effectDesc.pEffect = static_cast<IXAPO*>(categoryAnalyzerXAPO_[soundCategory].Get());

			// Effect chain
			XAUDIO2_EFFECT_CHAIN effectChainXAPO = {};
			effectChainXAPO.EffectCount = 1;
			effectChainXAPO.pEffectDescriptors = &effectDesc;

			// SubmixVoice に XAPO をセット
			result = categorySubmixVoices_[soundCategory]->SetEffectChain(&effectChainXAPO);
			assert(SUCCEEDED(result));

			CreateSilentVoice(categorySilentVoices_[soundCategory], categorySubmixVoices_[soundCategory]);
		}

		void Audio::Update()
		{
			if (!silentFeedEnabled_) return;

			XAUDIO2_VOICE_STATE st{};
			silentVoice_->GetState(&st);

			// 無音バッファを定期的に供給して解析パイプラインを動かし続ける
			if (st.BuffersQueued < 3)
			{
				XAUDIO2_BUFFER buf{};
				buf.AudioBytes = static_cast<UINT32>(silentBuffer_.size() * sizeof(float));
				buf.pAudioData = reinterpret_cast<const BYTE*>(silentBuffer_.data());

				silentVoice_->SubmitSourceBuffer(&buf);
			}

			for (auto& [name, voice] : categorySilentVoices_)
			{
				voice->GetState(&st);
				if (st.BuffersQueued < 3)
				{
					XAUDIO2_BUFFER buf{};
					buf.AudioBytes = static_cast<UINT32>(silentBuffer_.size() * sizeof(float));
					buf.pAudioData = reinterpret_cast<const BYTE*>(silentBuffer_.data());

					voice->SubmitSourceBuffer(&buf);
				}
			}
		}

		void Audio::EnableSilentFeed(bool enable)
		{
			silentFeedEnabled_ = enable;

			if (enable) 
			{
				silentVoice_->Start();
				for (auto& [name, voice] : categorySilentVoices_)
				{
					voice->Start();
				}
			}
			else 
			{
				silentVoice_->Stop();
				for (auto& [name, voice] : categorySilentVoices_)
				{
					voice->Stop();
				}
			}
		}

		void Audio::Stop(int resourceNum)
		{
			sourceVoices_[resourceNum]->Stop();
			sourceVoices_[resourceNum]->FlushSourceBuffers();
		}

		void Audio::Pause(int resourceNum)
		{
			sourceVoices_[resourceNum]->Stop();
		}

		void Audio::Resume(int resourceNum)
		{
			sourceVoices_[resourceNum]->Start();
		}

		void Audio::Pitch(int resourceNum, float pitch)
		{
			sourceVoices_[resourceNum]->SetFrequencyRatio(pitch);
		}

		void Audio::SetMasterVolume(float volume)
		{
			masterVoice_->SetVolume(volume);
		}

		void Audio::SetCategoryVolume(const std::string& soundCategory, float volume)
		{
			categorySubmixVoices_[soundCategory]->SetVolume(volume);
		}

		void Audio::SetSoundVolume(int resourceNum, float volume)
		{
			sourceVoices_[resourceNum]->SetVolume(volume);
		}

		float Audio::GetMasterVolume()
		{
			float volume = 0;
			masterVoice_->GetVolume(&volume);
			return volume;
		}

		float Audio::GetCategoryVolume(const std::string& soundCategory)
		{
			float volume = 0;
			categorySubmixVoices_[soundCategory]->GetVolume(&volume);
			return volume;
		}

		float Audio::GetSoundVolume(int resourceNum)
		{
			float volume = 0;
			sourceVoices_[resourceNum]->GetVolume(&volume);
			return volume;
		}

		void Audio::Load(const std::string& filename, const std::string& extension)
		{
			if (soundDataMap_.count(filename))
			{
				// すでに読み込み済みならスキップ
				return;
			}

			// ファイルオープン
			std::ifstream file;
			std::string filePath = directoryPath_;
			filePath += filename;
			filePath += extension;
			file.open(filePath, std::ios_base::binary);
			assert(file.is_open());

			// 文字列変換
			std::wstring filePathW = Utility::StringUtility::ConvertString(filePath);
			HRESULT result;
			// SourceReader作成
			Microsoft::WRL::ComPtr<IMFSourceReader> pReader;
			result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &pReader);
			if (FAILED(result))
			{
				// ログに警告を出す（デバッグ用）
				Log("Audio Warning: " + filePath + " not found.\n");

				assert(SUCCEEDED(result));
			}
			// フォーマット指定
			Microsoft::WRL::ComPtr<IMFMediaType> pPMCType;
			MFCreateMediaType(&pPMCType);
			pPMCType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
			pPMCType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
			result = pReader->SetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), nullptr, pPMCType.Get());
			if (FAILED(result))
			{
				// ログに警告を出す（デバッグ用）
				Log("Audio Warning: Failed to PCM format.\n");

				assert(SUCCEEDED(result));
			}
			// フォーマット取得
			Microsoft::WRL::ComPtr<IMFMediaType> pOutType;
			pReader->GetCurrentMediaType(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), &pOutType);
			WAVEFORMATEX* waveFormat = nullptr;
			MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);

			SoundData sData = {};
			sData.wfex = *waveFormat;

			CoTaskMemFree(waveFormat);

			while (true)
			{
				Microsoft::WRL::ComPtr<IMFSample> pSample;
				DWORD streamIndex = 0, flags = 0;
				LONGLONG llTimeStamp = 0;
				// 読み込み
				result = pReader->ReadSample(static_cast<DWORD>(MF_SOURCE_READER_FIRST_AUDIO_STREAM), 0, &streamIndex, &flags, &llTimeStamp, &pSample);
				// ストリームの末尾に達したら抜ける
				if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

				if (pSample)
				{
					Microsoft::WRL::ComPtr<IMFMediaBuffer> pBuffer;
					pSample->ConvertToContiguousBuffer(&pBuffer);

					BYTE* pData = nullptr;
					DWORD maxLength = 0, currentLength = 0;
					pBuffer->Lock(&pData, &maxLength, &currentLength);
					sData.buffer.insert(sData.buffer.end(), pData, pData + currentLength);
					pBuffer->Unlock();
				}
			}
			// 1. 全データサイズを取得
			size_t totalDataSize = sData.buffer.size();

			if (totalDataSize > 0)
			{
				// 2. 再生時間の長さを計算 (データサイズ / ブロックアラインメント)
				// ※ nBlockAlign は (チャンネル数 * ビット数 / 8)
				sData.playSoundLength = static_cast<uint32_t>(totalDataSize / sData.wfex.nBlockAlign);

				// 3. BPM解析の実行
				sData.bpm = BPMDetector::AnalyzeBPM(sData.buffer.data(), static_cast<UINT32>(totalDataSize), sData.wfex);

				// 4. ログ出力
				Log(filename + ": BPM " + std::to_string(sData.bpm) + "\n");
			}

			// 最後にマップへ登録
			soundDataMap_[filename] = std::move(sData);
		}

		void Audio::SoundUnload(const std::string& filename)
		{
			if (soundDataMap_.erase(filename) == 0)
			{
				Log("Sound not loaded.\n");
			}
		}

		int Audio::Play(const std::string& filename, bool isLoop, const std::string& soundCategory)
		{
			HRESULT result;

			// ロード済みデータから検索
			auto it = soundDataMap_.find(filename);
			if (it == soundDataMap_.end())
			{
				Log("Sound not loaded.\n");
				return -1;
			}
			SoundData& soundData = it->second;

			// 空いているSourceVoiceを探す
			int sourceNum = SearchSourceVoice(sourceVoices_.data());

			if (sourceNum == -1)
			{
				Log("No sound resource available.\n");
				return -1;
			}

			// 必要なら古いVoiceを破棄して再利用
			if (sourceVoices_[sourceNum] != nullptr)
			{
				sourceVoices_[sourceNum]->Stop();
				sourceVoices_[sourceNum]->FlushSourceBuffers();
				sourceVoices_[sourceNum]->DestroyVoice();
				sourceVoices_[sourceNum] = nullptr;
			}

			// 出力先(SubmixVoice)の決定
			XAUDIO2_SEND_DESCRIPTOR sendDesc = {};
			sendDesc.Flags = 0;
			if (soundCategory == "")
			{
				// カテゴリ指定なし -> Analyzerへ直接
				sendDesc.pOutputVoice = analyzerSubmix_;
			}
			else
			{
				// カテゴリ指定あり -> 該当Submixへ
				auto itSub = categorySubmixVoices_.find(soundCategory);
				if (itSub == categorySubmixVoices_.end())
				{
					Log("SoundCategory not found.\n");
					sendDesc.pOutputVoice = analyzerSubmix_;
				}
				else
				{
					sendDesc.pOutputVoice = itSub->second;
				}
			}

			XAUDIO2_VOICE_SENDS sends = {};
			sends.SendCount = 1;
			sends.pSends = &sendDesc;

			// 波形フォーマットをもとにSourceVoice生成
			if (FAILED(xAudio2_->CreateSourceVoice(&sourceVoices_[sourceNum],
				&soundData.wfex, 0,
				XAUDIO2_DEFAULT_FREQ_RATIO,
				nullptr,
				&sends)))
			{
				Log("Failed to create source voice.\n");
				return -1;
			}

			// バッファ設定（ループ設定含む）
			XAUDIO2_BUFFER buf = SetBuffer(isLoop, soundData);

			// 再生開始
			result = sourceVoices_[sourceNum]->SubmitSourceBuffer(&buf);
			result = sourceVoices_[sourceNum]->Start();

			return sourceNum;
		}

		Microsoft::WRL::ComPtr<MyAnalyzerXAPO> Audio::GetAnalyzerXAPO(const std::string& soundCategory)
		{
			if (soundCategory == "")
			{
				// カテゴリ指定なし -> defaultAnalyzer
				return analyzerXAPO_;
			}
			else
			{
				// カテゴリ指定あり -> 該当 Analyzer へ
				auto itSub = categorySubmixVoices_.find(soundCategory);
				if (itSub == categorySubmixVoices_.end())
				{
					Debugger::Log("SoundCategory not found.\n");
					return analyzerXAPO_;
				}
				else
				{
					return categoryAnalyzerXAPO_[soundCategory];
				}
			}
		}

		int Audio::SearchSourceVoice(IXAudio2SourceVoice** sourceVoices)
		{
			// 今回再生するリソース
			int sourceVoiceNum = -1;

			// リソースのバッファ
			unsigned int soundBuffer = 0;

			// 使用できる再生リソースを検索
			for (int i = 0; i < kMaxPlayWave; i++)
			{
				if (sourceVoices[i] == nullptr)
				{
					sourceVoiceNum = i;
					break;
				}
				// 現在の状態を取得
				XAUDIO2_VOICE_STATE state;
				sourceVoices[i]->GetState(&state);

				// バッファが0ならば再生可能と判断
				if (state.BuffersQueued == 0)
				{
					sourceVoiceNum = i;
					break;
				}
				else
				{
					// 初期値もしくはバッファが最小の場合は入れ替え
					if (soundBuffer == 0 || soundBuffer > state.BuffersQueued)
					{
						soundBuffer = state.BuffersQueued;
						sourceVoiceNum = i;
					}
				}
			}

			return sourceVoiceNum;
		}

		XAUDIO2_BUFFER Audio::SetBuffer(bool loop, const SoundData& sound)
		{
			// バッファの初期化
			XAUDIO2_BUFFER buffer = {};

			// バッファ設定
			buffer.pAudioData = sound.buffer.data(); // vectorの先頭アドレス
			buffer.AudioBytes = static_cast<UINT32>(sound.buffer.size());
			buffer.PlayBegin = 0;
			buffer.PlayLength = sound.playSoundLength;

			// ループ設定
			if (loop)
			{
				buffer.LoopBegin = 0;
				buffer.LoopLength = sound.playSoundLength;
				buffer.LoopCount = XAUDIO2_LOOP_INFINITE;
			}

			return buffer;
		}

		bool Audio::IsPlaying(int resourceNum)
		{
			if (resourceNum < 0 || resourceNum >= kMaxPlayWave || sourceVoices_[resourceNum] == nullptr)
			{
				return false;
			}
			XAUDIO2_VOICE_STATE state;
			sourceVoices_[resourceNum]->GetState(&state, XAUDIO2_VOICE_NOSAMPLESPLAYED);
			return state.BuffersQueued > 0;
		}

		UINT64 Audio::GetPlaybackSamples(int resourceNum)
		{
			if (resourceNum < 0 || resourceNum >= kMaxPlayWave || sourceVoices_[resourceNum] == nullptr)
			{
				return 0;
			}
			XAUDIO2_VOICE_STATE state;
			sourceVoices_[resourceNum]->GetState(&state, 0);
			return state.SamplesPlayed;
		}

	} // namespace Audio
} // namespace TYEngine
