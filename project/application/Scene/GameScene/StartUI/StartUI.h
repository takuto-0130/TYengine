#pragma once
#include <memory>

class Sprite;
class StartUI
{
public:
	void Initialze();

	void Update();

	void Draw();

private:
	std::unique_ptr<Sprite> ready_;
	std::unique_ptr<Sprite> go_;
};

