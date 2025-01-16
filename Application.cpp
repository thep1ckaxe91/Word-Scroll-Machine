#include "Application.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <iostream>

namespace fs = std::filesystem;
Application::Application()
{
	sf::ContextSettings settings;
	settings.attributeFlags = sf::ContextSettings::Default;
	settings.antiAliasingLevel = 16;

	scrollerWindow = sf::RenderWindow(sf::VideoMode(sf::Vector2u(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)), "Scroller Window", sf::State::Windowed, settings);
	readerWindow = sf::RenderWindow(sf::VideoMode(sf::Vector2u(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)), "Reader Window", sf::Style::Resize, sf::State::Windowed, settings);

	scrollerWindow.setFramerateLimit(60);
	readerWindow.setFramerateLimit(60);


	if (!ImGui::SFML::Init(scrollerWindow)) {
		Logger::log("Can't initialize ImGUI", Logger::HIGH);
	}

	memset(contentBuffer, 0, sizeof(contentBuffer));
	//fs::current_path() lead to solution directory

	if (!font.openFromFile(fs::path("fonts") / "OpenSans-Regular.ttf"))
	{
		Logger::log("Can't open font", Logger::LOW);
	}
	font.setSmooth(true);

}

void Application::run()
{
	while (scrollerWindow.isOpen())
	{

		while (const std::optional e = scrollerWindow.pollEvent())
		{
			ImGui::SFML::ProcessEvent(scrollerWindow, e.value());
			if (e->is<sf::Event::Closed>())
			{
				scrollerWindow.close();
			}
		}
		while (const std::optional e = readerWindow.pollEvent())
		{
			if (e->is<sf::Event::Closed>())
			{
				readerWindow.close();
				scrollerWindow.close();
			}
			if (e->is<sf::Event::Resized>())
			{

				onReaderResize(e->getIf<sf::Event::Resized>());
			}
		}
		update(scrollerWindow, readerWindow, clock.restart());
		draw(scrollerWindow, readerWindow);
	}
	ImGui::SFML::Shutdown();
}

void Application::update(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow, sf::Time dt) {
	ImGui::SFML::Update(scrollerWindow, dt);
}

void Application::draw(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow)
{
	readerWindow.clear();

	readerWindow.display();

	scrollerWindow.clear();

	scrollerWindow.draw(contentSprite);

	ImGui::Begin("Put Content Here");

	if (ImGui::InputTextMultiline("Content", contentBuffer, MAXIMUM_CONTENT_LENGTH))
	{
		// content changed
		// load new rendered contentTexture
		updateContent();
	}

	ImGui::End();

	ImGui::SFML::Render(scrollerWindow);
	scrollerWindow.display();

}

void Application::onReaderResize(const sf::Event::Resized* e)
{
	///FIXME: implement what happen on resizing the reader window

	readerViewSize = e->size;

	updateContent();
}

//expensive operation
void Application::updateContent()
{
	/*
	idea: simply draw a new sf::Text to contentTexture when:
		- met '\n' or '.'
		- to get next word is not enough
	*/

	sf::Text curLine(font, "", fontSize);
	int lastPos = 0;
	for (int i = 0; contentBuffer[i] != '0'; i++)
	{
		bool doSplit = false;
		for(const char& c : delimiter)
			if (c == contentBuffer[i]) {
				doSplit = true;
				break;
			}

		if (doSplit)
		{

		}
		else {

		}
	}
}

// testing content:
/*
Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua.\n
Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat.\n
Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur.\n
Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.
*/