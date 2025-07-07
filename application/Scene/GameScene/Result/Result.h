#pragma once
#include <memory>
class Sprite;
class ResultClass
{
public:
	void Initialze();

	void Update();

	void Draw();

private:
	std::unique_ptr<Sprite> back_;
	std::unique_ptr<Sprite> text_;
	std::unique_ptr<Sprite> scoretext_;
	std::unique_ptr<Sprite> spaceSpr_;
};

