#pragma once
#include "Utils/JSON/JsonManager.h"

namespace TYEngine
{
	namespace Core
	{
		/// <summary>
		/// エンジン全体の静的設定項目（FPS上限など）のロードおよび保持を行う設定モジュール。
		/// </summary>
		namespace EngineConfig
		{
			/// <summary>設定されたターゲットFPS（デフォルト: 60.0f）。</summary>
			inline float fps_ = 60.0f;

			/// <summary>EngineConfig.json からエンジン設定をロードする。</summary>
			inline void Load()
			{
				Utility::JsonManager jm;
				if (jm.Load("EngineConfig.json"))
				{
					fps_ = jm.Get<float>("graphics.fps", 60.0f);
				}
			}
		}
	} // namespace Core
} // namespace TYEngine