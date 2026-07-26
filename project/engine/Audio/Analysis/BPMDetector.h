#pragma once
#include <vector>
#include <Windows.h> // WAVEFORMATEX用

namespace TYEngine
{
	namespace AudioSystem
	{
		/// <summary>
		/// 音声データ（WAV波形データ）を解析し、自動でBPM（1分間あたりの拍数）を推定算出するクラス。
		/// </summary>
		class BPMDetector
		{
		public:
			/// <summary>
			/// WAVバッファを解析してBPMを推定する
			/// </summary>
			/// <param name="pData">WAVのデータ部分(dataチャンク)の先頭ポインタ</param>
			/// <param name="dataSize">データのバイトサイズ</param>
			/// <param name="wfx">フォーマット情報</param>
			/// <returns>推定されたBPM</returns>
			static float AnalyzeBPM(const BYTE* pData, UINT32 dataSize, const WAVEFORMATEX& wfx);
		};
	}
}