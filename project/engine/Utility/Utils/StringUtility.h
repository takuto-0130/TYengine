#pragma once
#include <string>

// 文字列の汎用関数
namespace StringUtility
{
	// string → wstring の変換
	std::wstring ConvertString(const std::string& str);

	// wstring → string の変換
	std::string ConvertString(const std::wstring& str);
};

