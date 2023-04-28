#include <SFML/Graphics.hpp>
#include "math.h"

#ifndef HELPERS_H
#define HELPERS_H

namespace helpers
{
  void draw_grid(sf::RenderWindow &window, int spacing, float line_width, sf::Color color)
  {
    sf::RectangleShape line(sf::Vector2f(window.getSize().x, line_width));
    line.setFillColor(color);

    for (int i = 1; i < window.getSize().y / spacing; ++i)
    {
      line.setPosition(0, i * spacing);
      window.draw(line);
    }

    line.setSize(sf::Vector2f(line_width, window.getSize().y));

    for (int i = 1; i < window.getSize().x / spacing; ++i)
    {
      line.setPosition(i * spacing, 0);
      window.draw(line);
    }
  }

  void draw_line_at_angle_from_point(sf::RenderWindow &window, sf::Vector2f origin, float theta, float length, sf::Color color)
  {
    float to_x = origin.x + cos(math::deg_to_rad(theta)) * length;
    float to_y = origin.y + sin(math::deg_to_rad(theta)) * length;
    sf::Vertex line[] = {
      sf::Vertex(origin),
      sf::Vertex(sf::Vector2f(to_x, to_y))};
    line[0].color = color;
    line[1].color = color;
    window.draw(line, 2, sf::Lines);
  }

  void log_vec(std::string memo, sf::Vector2f vec)
  {
    std::cout << memo << " [" << vec.x << "," << vec.y << "]" << std::endl;
  }
  std::pair<int, int> index_to_xy(int i, int w)
  {
    int x = i % w;
    int y = i / w;
    return std::make_pair(x, y);
  }

  int xy_to_index(int x, int y, int w)
  {
    return y * w + x;
  }
}
#endif
