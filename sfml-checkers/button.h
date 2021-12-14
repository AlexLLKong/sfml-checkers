#pragma once
#include "SFML/Graphics.hpp"

namespace my {
	enum class ButtonEvent {
		NONE,
		RESIGN,
		RESTART
	};

	struct Button {
		std::shared_ptr<sf::Sprite> Sprite;
		bool Pressed;
		ButtonEvent BtnEvent;
		Button() = delete;
		Button(std::shared_ptr<sf::Sprite> sprite, ButtonEvent buttonEvent)
		{
			Sprite = sprite;
			Pressed = false;
			BtnEvent = buttonEvent;
		}
	};
}