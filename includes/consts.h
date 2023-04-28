#include <iostream>

#include <SFML/Graphics.hpp>

#ifndef CONSTS_H
#define CONSTS_h

namespace consts
{
  int FRAME_RATE = 60;

  namespace window
  {
    int width(1920);
    int height(1080);
    std::string name("Procedural Animation");
  }

  namespace colors
  {
    sf::Color bright_green(67, 223, 93);
    sf::Color dark_gray(66, 66, 66);
    sf::Color dark_yellow(135, 132, 100);
    sf::Color light_blue(176, 227, 237);
    sf::Color light_green(176, 237, 200);
    sf::Color light_orange(237, 217, 176);
    sf::Color light_red(175, 116, 135);
    sf::Color pink(230, 176, 237);
    sf::Color purple(88, 77, 92);
  }
}

#endif
