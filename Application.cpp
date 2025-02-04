#include "Application.hpp"
#include "WrapText.hpp"
#include "Logger.hpp"
#include <filesystem>
#include <iostream>
#include <thread>
#include <fstream>
#include <string>
#include <algorithm>
#include <SFML/System.hpp>
#ifdef _WIN32
#define IS_WINDOW _
#include <Windows.h>
#endif

#if defined(macintosh) || defined(Macintosh) || defined(__APPLE__) || defined(__MACH__) || defined(__linux__)
#define IS_NOT_WINDOW _
#endif

namespace fs = std::filesystem;
using Key = sf::Keyboard::Key;

Application::Application()
{
#ifdef IS_WINDOW
	SetConsoleOutputCP(CP_UTF8);
#endif
	sf::ContextSettings settings;
	settings.attributeFlags = sf::ContextSettings::Default;
	settings.antiAliasingLevel = 8;
	try {
		scrollerWindow = sf::RenderWindow(sf::VideoMode(sf::Vector2u(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)), "Scroller Window - Press F1 to toggle Control", sf::Style::Close | sf::Style::Titlebar, sf::State::Windowed, settings);
		readerWindow = sf::RenderWindow(sf::VideoMode(sf::Vector2u(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT)), "Reader Window", sf::Style::Resize | sf::Style::Close, sf::State::Windowed, settings);
	}
	catch (std::exception e)
	{
		Logger::log(e.what(), Logger::HIGH);
	}
	baseContentCenterPosition = { readerWindow.getSize().x / 2.f, 0 };

	scrollerWindow.setFramerateLimit(60);
	readerWindow.setFramerateLimit(60);
	if (!ImGui::SFML::Init(scrollerWindow)) {
		Logger::log("Can't initialize ImGUI", Logger::HIGH);
	}
	//fs::current_path() lead to solution directory

	//TODO: maybe add a font file choose GUI
	if (!font.openFromFile(fs::path("fonts") / "OpenSans-Regular.ttf"))
	{
		Logger::log("Can't open font", Logger::MEDIUM);
	}
	font.setSmooth(true);
	fontSize = 72;

	changeScreenSize(readerWindow.getSize());
	readerSprite.setScale({ 1,-1 });

	loadContent();
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
			else if (e->is<sf::Event::MouseWheelScrolled>())
			{
				onScrollerScroll(e->getIf<sf::Event::MouseWheelScrolled>());
			}
			else if (e->is<sf::Event::MouseButtonReleased>())
			{
				if (toUpdateTexture)
				{
					toUpdateTexture = false;
					updateContentTexture();
				}
			}
			else if (e->is<sf::Event::KeyPressed>())
			{
				auto key = e->getIf<sf::Event::KeyPressed>()->code;

				switch (key)
				{
				case Key::F1:
					toggleGUI = !toggleGUI;
					break;
				default:
					break;
				}

			}
		}
		while (const std::optional e = readerWindow.pollEvent())
		{
			if (e->is<sf::Event::Closed>())
			{
				readerWindow.close();
				scrollerWindow.close();
			}
			if (const auto* resized = e->getIf<sf::Event::Resized>())
			{

				sf::FloatRect visibleArea({ 0.f, 0.f }, sf::Vector2f(resized->size));
				readerWindow.setView(sf::View(visibleArea));
				onReaderResize(resized);
			}
		}
		update(scrollerWindow, readerWindow, clock.restart());
		draw(scrollerWindow, readerWindow);
	}
	ImGui::SFML::Shutdown();
}

void Application::update(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow, sf::Time dt) {
	ImGui::SFML::Update(scrollerWindow, dt);

	contentTexture.clear();
	for (int i : currentIndexes)
	{
		contentTexts[i].setPosition(
			{
				baseContentCenterPosition.x - contentTexts[i].getLocalBounds().size.x / 2,
				baseContentCenterPosition.y + contentOffset[i]
			}
		);
		contentTexture.draw(contentTexts[i]);
	}
	// calling display will actually flip the buffer to the screen, in that sense, if we call display, we need to flip readerSprite instead of scroller
	contentTexture.display();
}

void Application::draw(sf::RenderWindow& scrollerWindow, sf::RenderWindow& readerWindow)
{
	readerWindow.clear();

	readerWindow.draw(readerSprite);

	readerWindow.display();

	scrollerWindow.clear(sf::Color::White);
	scrollerWindow.draw(scrollerSprite);

	if (toggleGUI)
		this->displayGUI();
	ImGui::SFML::Render(scrollerWindow);
	scrollerWindow.display();
}

void Application::displayGUI()
{
	ImGui::Begin("Control window");

	if (ImGui::Button("Load content")) {
		loadContent();
	}

	if (ImGui::SliderFloat("Scroll Speed", &scrollMultiplier, 0.f, 50.f))
	{

	}

	if (ImGui::SliderInt("Font Size", &fontSize, 10, 300))
	{
		toUpdateTexture = true;
	}

	//if (ImGui::Button("Flip X"))
	//{
	//	readerSprite.scale({ -1,1 });
	//}

	//if (ImGui::Button("Flip Y"))
	//{
	//	readerSprite.scale({ 1,-1 });
	//}

	if (ImGui::Button("Rotate 90 cw"))
	{
		changeScreenSize({ contentTexture.getSize().y, contentTexture.getSize().x });
		readerSprite.rotate(sf::degrees(90));
	}


	ImGui::Text("Please inform if there're any problem, bug or feature B/S want to add!");
	ImGui::End();

}

void Application::onScrollerScroll(const sf::Event::MouseWheelScrolled* e)
{
	float deltaScroll = e->delta; //convert to world coordinate y-axis

	baseContentCenterPosition.y += deltaScroll * scrollMultiplier;

	updateCurrentIndexes();
}

void Application::onReaderResize(const sf::Event::Resized* e)
{

	baseContentCenterPosition = { readerWindow.getSize().x / 2.f, 0};

	changeScreenSize(readerWindow.getSize());

	readerSprite.setRotation(sf::degrees(0));
}

void Application::changeScreenSize(sf::Vector2u size)
{
	baseContentCenterPosition = { size.x / 2.f, 0 };

	contentTexture = sf::RenderTexture{ size };

	scrollerSprite.setTexture(contentTexture.getTexture());
	readerSprite.setTexture(contentTexture.getTexture());
	scrollerSprite.setTextureRect(sf::IntRect({ 0, 0 }, { (int)size.x, (int)size.y }));
	readerSprite.setTextureRect(sf::IntRect({ 0, 0 }, { (int)size.x, (int)size.y }));

	readerSprite.setOrigin({ size.x / 2.f , size.y / 2.f });
	scrollerSprite.setOrigin({ size.x / 2.f , size.y / 2.f });

	scrollerSprite.setPosition({ scrollerWindow.getSize().x / 2.f, scrollerWindow.getSize().y / 2.f });
	readerSprite.setPosition({ readerWindow.getSize().x / 2.f, readerWindow.getSize().y / 2.f });
	//FIXME: edit the scale of the scrollerSprite so that it fit in one of 2 screen dimension

	float factor;
	float readerRatio = size.x * 1.f / size.y;
	float scrollerRatio = scrollerWindow.getSize().x * 1.f / scrollerWindow.getSize().y;
	factor = scrollerRatio < readerRatio ?
		scrollerWindow.getSize().x * 1.f / size.x
		: scrollerWindow.getSize().y * 1.f / size.y;
	scrollerSprite.setScale({ factor, factor });

	updateContentTexture();
}

/**
*
* update the current displaying content of the texture on both scroller and reader, get the content from content.txt file
*
* this function update the member contentLines to a vector, each element is a line get from content file
* AND update the texture acoording to the content
*
* line is store by sf::String which can have utf-8 encode, convenient for displaying
*/
void Application::loadContent()
{
	try {
		contentTexts.clear();
		contentOffset.clear();

		std::wifstream contentFile{ "content.txt" };
		std::wstring line;

		if (contentTexts.capacity() < 500)
			contentTexts.reserve(500);
		if (contentOffset.capacity() < 500)
			contentOffset.reserve(500);

		while (std::getline(contentFile, line))
		{
			contentTexts.push_back(WrapText{ font, sf::String::fromUtf8(line.begin(), line.end()) + "\n",static_cast<unsigned int>(fontSize) });

			contentTexts.back().setMaxLength(contentTexture.getSize().x);
			if (contentOffset.empty())
			{
				contentOffset.push_back(0);
			}
			else
			{
				contentOffset.push_back(contentOffset.back() + contentTexts[contentTexts.size() - 2].getLocalBounds().size.y);
			}
		}
			
		for (int i = 0; i < 3; i++)
		{
			if(i==0 || i==2)
				contentTexts.push_back(WrapText{ font, "\n\n\n", static_cast<unsigned int>(fontSize) });
			else
				contentTexts.push_back(WrapText{ font, "--- The End ---", static_cast<unsigned int>(fontSize) });

			if (contentOffset.empty())
			{
				contentOffset.push_back(0);
			}
			else
			{
				contentOffset.push_back(contentOffset.back() + contentTexts[contentTexts.size() - 2].getLocalBounds().size.y);
			}
		}
		// add all starting index to currentIndexes
		currentIndexes.clear();
		if (currentIndexes.capacity() < contentTexts.size())
			currentIndexes.reserve(contentTexts.size());


		for (int i = 0; i < contentTexts.size(); i++)
		{
			if (contentOffset[i] < contentTexture.getSize().y)
			{
				currentIndexes.push_back(i);
			}
		}
		baseContentCenterPosition.y = 0;
	}
	catch (const std::exception& e)
	{
		std::wcout << L"Các ace vui lòng không đổi tên file content.txt nhé ạ! Nếu có lỡ xóa, chỉ cần tạo lại file đó là được ạ.\n";
	}

	updateContentTexture();
}

/// <summary>
/// What updateContentTexture should only do is base on the new info after resize or content change like font size, etc
/// update maxLength of each contentTexts
/// </summary>
void Application::updateContentTexture()
{
	for (auto& text : contentTexts)
	{
		text.setCharacterSize(fontSize);
		text.setMaxLength(contentTexture.getSize().x);
	}

	// update contentOffset according to the new contentTexts size
	contentOffset.clear();
	if (contentOffset.capacity() < contentTexts.size())
		contentOffset.reserve(contentTexts.size());
	for (int i = 0; i < contentTexts.size(); i++)
	{
		if (contentOffset.empty())
		{
			contentOffset.push_back(0);
		}
		else
		{
			assert(!contentOffset.empty());
			contentOffset.push_back(contentOffset.back() + contentTexts[i - 1].getLocalBounds().size.y);
		}
	}
	if (!currentIndexes.empty())
		baseContentCenterPosition.y = -contentOffset[currentIndexes.front()];
	else
		baseContentCenterPosition.y = 0;
	baseContentCenterPosition.x = contentTexture.getSize().x / 2.f;


	updateCurrentIndexes();
}

/// <summary>
/// This should call when the content is scrolled
/// </summary>
void Application::updateCurrentIndexes()
{
	float upperBound = -baseContentCenterPosition.y;
	float lowerBound = upperBound + contentTexture.getSize().y;
	currentIndexes.clear();

	if (currentIndexes.capacity() < contentTexts.size())
		currentIndexes.reserve(contentTexts.size());

	auto itBegin = std::lower_bound(contentOffset.begin(), contentOffset.end(), upperBound);
	auto itEnd = std::upper_bound(contentOffset.begin(), contentOffset.end(), lowerBound);
	if (itBegin != contentOffset.begin())
		itBegin--;

	for (auto& it = itBegin; it != itEnd; it++)
	{
		currentIndexes.push_back(std::distance(contentOffset.begin(), it));
	}
}

//TODO: implement Goto function, allow user for searching certain content
//TODO: implement rotate the scroll direction to horizontal or vertical