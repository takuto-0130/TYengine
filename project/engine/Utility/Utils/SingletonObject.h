#pragma once
#include <memory>
#include <mutex>
#include <cassert>

namespace TYEngine
{
	namespace Utility
	{

		template <typename T>
		class SingletonObject
		{
		public:
			// インスタンスの取得
			static T* GetInstance()
			{
				std::call_once(initFlag_, []()
					{
						// friend 登録があっても make_unique は使えないため new を使用
						instance_.reset(new T());
					});

				// 再生成を許さないため、破棄後に呼ぶとエラーにする
				assert(instance_ != nullptr && "Attempted to access Singleton after destruction.");

				return instance_.get();
			}

			// 明示的な破棄
			static void Destroy()
			{
				instance_.reset(); // メモリ解放
			}

		protected:
			SingletonObject() = default;
			virtual ~SingletonObject() = default;
			SingletonObject(const SingletonObject&) = delete;
			SingletonObject& operator=(const SingletonObject&) = delete;

		private:
			static std::unique_ptr<T> instance_;
			static std::once_flag initFlag_;
		};

		// 静的メンバの定義
		template <typename T>
		std::unique_ptr<T> SingletonObject<T>::instance_ = nullptr;

		template <typename T>
		std::once_flag SingletonObject<T>::initFlag_;

	} // namespace Utility
} // namespace TYEngine


#if 0
#include <iostream>

// --- 派生クラスの実装 ---
class Class : public SingletonObject<Class>
{
	// 親クラスが private なコンストラクタを呼べるようにする
	friend class SingletonObject<Class>;
	// std::unique_ptr が private なデストラクタを呼べるようにする
	friend struct std::default_delete<Class>;

private:
	// コンストラクタを private にして外部からの new を禁止
	Class()
	{
		std::cout << "Class: 生成されました" << std::endl;
	}

	~Class()
	{
		std::cout << "Class: 破棄されました" << std::endl;
	}

public:
	void Update()
	{
		std::cout << "Class Update: 更新しました" << std::endl;
	}
};
#endif