#pragma once
#include <string>
/// <summary>
/// デバッグログ出力機能を提供する名前空間。
/// Visual Studioの出力ウィンドウへの文字列出力を行う。
/// </summary>
namespace Logger
{
	/// <summary>
	/// デバッグログを出力する。
	/// </summary>
	/// <param name="message">出力する文字列。</param>
	void Log(const std::string& message);
};

