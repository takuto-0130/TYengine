#include "StringUtility.h"
#include "windows.h"

namespace TYEngine {
namespace Utility {

namespace StringUtility
{
	std::wstring ConvertString(const std::string& str) {
		// 中身がなければ早期リターン
		if (str.empty()) {
			return std::wstring();
		}

		// 多バイト文字列(UTF-8) -> ワイド文字列(UTF-16) への変換
		auto sizeNeeded = MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), NULL, 0);
		// サイズが0なら早期リターン
		if (sizeNeeded == 0) {
			return std::wstring();
		}
		std::wstring result(sizeNeeded, 0);
		MultiByteToWideChar(CP_UTF8, 0, reinterpret_cast<const char*>(&str[0]), static_cast<int>(str.size()), &result[0], sizeNeeded);
		return result;
	}

	std::string ConvertString(const std::wstring& str) {
		// 中身がなければ早期リターン
		if (str.empty()) {
			return std::string();
		}

		// ワイド文字列(UTF-16) -> 多バイト文字列(UTF-8) への変換
		auto sizeNeeded = WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), NULL, 0, NULL, NULL);
		// サイズが0なら早期リターン
		if (sizeNeeded == 0) {
			return std::string();
		}
		std::string result(sizeNeeded, 0);
		WideCharToMultiByte(CP_UTF8, 0, str.data(), static_cast<int>(str.size()), result.data(), sizeNeeded, NULL, NULL);
		return result;
	}
}

} // namespace Utility
} // namespace TYEngine
