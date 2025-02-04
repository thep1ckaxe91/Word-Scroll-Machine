#pragma once
#ifndef APPLICATION_HPP
#define APPLICATION_HPP
#include <SFML/Graphics.hpp>
#include "imgui.h"
#include "imgui-SFML.h"
#include "config.hpp"
#include "WrapText.hpp"
class Application
{
public:
	Application();

	void run();
private:
	void update(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow, sf::Time dt);

	void draw(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow);

	void onReaderResize(const sf::Event::Resized* e);

	void loadContent();

	void updateContentTexture();

	void onScrollerScroll(const sf::Event::MouseWheelScrolled* e);

	void displayGUI();

	void updateCurrentIndexes();

	void changeScreenSize(sf::Vector2u size);

private:
	sf::RenderWindow scrollerWindow;
	sf::RenderWindow readerWindow;
	sf::Clock clock;


	sf::RenderTexture contentTexture;
	sf::Sprite scrollerSprite{contentTexture.getTexture()};
	sf::Sprite readerSprite{contentTexture.getTexture()};
	bool toUpdateTexture = false;

	std::vector<WrapText> contentTexts; // use wrapText ensure there only exists count(\n) amount of wraptext
	std::vector<float> contentOffset; // this got generate along side when load content, store the offset for each contentText
	std::vector<int> currentIndexes; // indexes of current displaying content, this should get check and update for each time scroll or resize window

	sf::Font font;
	int fontSize;
	sf::Vector2f baseContentCenterPosition; // top center for the content align with
	sf::Vector2u readerViewSize;

	float scrollMultiplier = 20;
	float contentYPos = 0; // the current y position of the content, this shall decrease when we going over the content

	bool toggleGUI = true;


};

#endif