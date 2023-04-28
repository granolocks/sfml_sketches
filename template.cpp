#include <iostream>
#include <SFML/Graphics.hpp>
const int W = 1920;
const int H = 1080;
const int FRAME_LIMIT = 60;
int main()
{
  sf::RenderWindow window(sf::VideoMode(W, H), "Template");
  window.setFramerateLimit(FRAME_LIMIT);


  while (window.isOpen())
  {
    // Process events
    sf::Event event;
    while (window.pollEvent(event))
    {
      switch (event.type)
      {
        case sf::Event::Closed:
          window.close();
        default:
          break;
      }
    }

    // Clear the window
    window.clear();

    window.display();
  }

  return 0;
}
