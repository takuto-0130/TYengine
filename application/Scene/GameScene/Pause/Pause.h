#pragma once
#include <memory>
class Sprite;
class Pause
{
public:
	void Initialze();

	void Update();

	void Draw();

private:
	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;
};

