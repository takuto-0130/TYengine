#include "Logger.h"
#include "windows.h"

namespace Logger
{
	void Log(const std::string & message) {
		(void)message;
#ifdef _DEBUG
		// デバッグウィンドウへの文字列出力
		OutputDebugStringA(message.c_str());
#endif // _DEBUG
	}
}
