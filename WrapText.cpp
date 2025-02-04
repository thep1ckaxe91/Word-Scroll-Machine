#include "WrapText.hpp"
#include <vector>

WrapText::WrapText(const sf::Font& font, const sf::String& string, unsigned int characterSize)
	: sf::Text(font, string, characterSize), maxLength(0) {
	originalString = string;
	wrapText();
}

void WrapText::setMaxLength(float length) {
	maxLength = length;
	wrapText();
}

void WrapText::setFont(const sf::Font& font) {
	Text::setFont(font);
	wrapText();
}

void WrapText::setCharacterSize(unsigned int size) {
	Text::setCharacterSize(size);
	wrapText();
}

void WrapText::setString(const sf::String& string) {
	originalString = string;
	wrapText();
}
void WrapText::wrapText() {
	if (maxLength <= 0) {
		sf::Text::setString(originalString);
		return;
	}

	sf::String wrappedString;

	sf::String currentLine;

	for (int i = 0; i < originalString.getSize(); i++)
	{
		if (originalString[i] == '\n')
		{
			wrappedString += currentLine + '\n';
			currentLine = "";
		}
		else
		{
			//get next word
			sf::String nextWord;
			int j = i-1;
			do
			{
				nextWord += originalString[++j];
			} while (j < originalString.getSize() && originalString[j] != ' ' && originalString[j] != '\n');

			sf::Text::setString(currentLine + nextWord);
	
			if (sf::Text::getLocalBounds().size.x > maxLength)
			{
				wrappedString += currentLine + '\n';
				currentLine = nextWord;
				i = j;
			}
			else
			{
				currentLine += nextWord;
				i = j;
			}
		}
	}
	wrappedString += currentLine;
	sf::Text::setString(wrappedString);
}

sf::String WrapText::getWrappedString() const
{
	return sf::Text::getString();
}

sf::String WrapText::getOriginalString() const
{
	return originalString;
}