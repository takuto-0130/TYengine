#include "Player.h"

using namespace TYEngine;
using namespace Utility;
using namespace Debugger;

void Player::JMInit()
{
	jm_.Load("PlayerConfig.json", true, &err_);
#ifdef _DEBUG
	Log(err_);
#endif // _DEBUG

	// 値反映

}

void Player::DebugJMApply()
{
#ifdef _DEBUG
	
#endif // _DEBUG
}