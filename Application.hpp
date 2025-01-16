#pragma once
#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "config.hpp"
class Application
{
public:
	Application();

	void run();
private:
	void update(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow, sf::Time dt);

	void draw(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow);

	void onReaderResize(const sf::Event::Resized *e);

	void updateContent();
private:
	sf::RenderWindow scrollerWindow;
	sf::RenderWindow readerWindow;	
	sf::Clock clock;
	sf::RenderTexture contentTexture;
	sf::Sprite contentSprite;
	char contentBuffer[MAXIMUM_CONTENT_LENGTH+1]; //+1 for \0 char
	sf::Font font;
	uint32_t fontSize;
	sf::Vector2f baseContentCenterPosition; // top center for the content align with
	sf::Vector2u readerViewSize;
	const std::string delimiter = ".\n";
};

#endif