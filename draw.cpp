#include <SFML/Graphics.hpp>
#include <windows.system.h>



void draw(sf::CircleShape shape1, sf::RenderWindow window1, float posX, float posY)
{
	if (GetKeyState('D') == true){ posX++; }
	if (GetKeyState('A') == true){ posX--; }
	shape1.setPosition(posX, posY);
	window1.clear();
	window1.draw(shape1);
	window1.display();
}