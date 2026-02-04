#pragma once

// メモリリーク監視用
struct D3DResourceLeakChecker
{
	/// <summary>デストラクタ。メモリリークをチェックし報告する。</summary>
	~D3DResourceLeakChecker();
};

