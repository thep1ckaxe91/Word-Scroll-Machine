#pragma once
#ifndef WRAPTEXT_HPP
#define WRAPTEXT_HPP

#include <SFML/Graphics.hpp>
/// <summary>
/// Wrapable text that auto insert \n character when reach the maximum length
/// Behave much like the normal sf::Text, but with the ability to wrap text	
/// </summary>
class WrapText : public sf::Text {
public:
	WrapText(const sf::Font& font = sf::Font(), const sf::String& string = "", unsigned int characterSize = 30);

	void setMaxLength(float length);

	void setFont(const sf::Font& font);

	void setCharacterSize(unsigned int size);

	void setString(const sf::String& string);

	sf::String getWrappedString() const;

	sf::String getOriginalString() const;
private:
	sf::String originalString;
	float maxLength;

	void wrapText();
};


#endif