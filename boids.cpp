#include <iostream>
#include <math.h>

#include <SFML/System.hpp>
#include <SFML/Graphics.hpp>

#include "includes/math.h"
#include "includes/consts.h"
#include "includes/helpers.h"
#include "includes/rand.h"

#include "includes/vec2_helpers.h"

const float AWARENESS_RADIUS = 50.f;
const bool DEBUG = false;
const bool DEBUG_2 = false;
const int NUM_BOIDS = 200;
const float MAX_SPEED = 4.f;
const float BOID_SIZE = 7.f;
const float ALIGN_FORCE = 0.9f;
const float COHESION_FORCE = 1.2f;
const float SEPARATION_BIAS = 1.9f;
const float SEPARATION_FORCE = 1.2f;

class Boid
{
public:
  Boid(sf::Vector2f initial_position,
       sf::Vector2f initial_velocity,
       int id,
       sf::RenderWindow &window)
      : m_position(initial_position),
        m_velocity(initial_velocity),
        m_id(id),
        m_window(window),
        m_acceleration(initial_velocity),
        m_size(BOID_SIZE),
        m_awareness(AWARENESS_RADIUS)
  {
  }

  ~Boid() {}

  int id()
  {
    return m_id;
  }

  void draw()
  {
    int angle_of_travel = math::angle_from_origin(m_velocity.x, m_velocity.y) + 90.f;

    sf::CircleShape tri(m_size, 3);
    tri.setOrigin(m_size, m_size);
    tri.setPosition(m_position);
    tri.setFillColor(sf::Color(0.f + 5.f * m_neighbors_pos.size(), 127.f, 127.f - 5.f * m_neighbors_pos.size()));
    tri.scale(1.0f, 2.0f);
    tri.setRotation(angle_of_travel);
    m_window.draw(tri);

    if (DEBUG)
    {
      draw_debug();
    }
  }

  void draw_debug()
  {
    int angle_of_travel = math::angle_from_origin(m_velocity.x, m_velocity.y) + 90.f;

    for (auto n : m_neighbors_pos)
    {

      sf::Vertex line[] = {
          sf::Vertex(m_position),
          sf::Vertex(n)};
      line[0].color = consts::colors::pink;
      line[1].color = consts::colors::pink;
      m_window.draw(line, 2, sf::Lines);
    }

    sf::CircleShape circ(m_awareness);
    circ.setOrigin(m_awareness, m_awareness);
    circ.setPosition(m_position);
    circ.setFillColor(sf::Color::Transparent);
    circ.setOutlineThickness(1.f);
    circ.setOutlineColor(consts::colors::purple);
    m_window.draw(circ);
    helpers::draw_line_at_angle_from_point(m_window,
                                           m_position,
                                           angle_of_travel - 90.f,
                                           m_awareness,
                                           consts::colors::purple);
  }

  void move()
  {
    m_velocity += m_acceleration;

    vec2f::normalize(m_velocity);

    m_velocity *= MAX_SPEED;
    m_position += m_velocity;

    m_acceleration.x = 0;
    m_acceleration.y = 0;

    edges();
  }

  void flock(std::vector<Boid *> boids, sf::RenderWindow &window)
  {
    m_neighbors_pos.clear();
    m_neighbors_vel.clear();
    for (auto b : boids)
    {
      if (b->id() != m_id && vec2f::dist(m_position, b->position()) < m_awareness)
      {
        m_neighbors_pos.push_back(b->position());
        m_neighbors_vel.push_back(b->velocity());
      }
    }
    align();
    cohesion();
    separation();
  }

  void align()
  {
    sf::Vector2f steering = m_velocity;
    int count_velocities = 0;
    for (auto v : m_neighbors_vel)
    {
      steering += v;
      count_velocities++;
    }

    if (count_velocities > 0)
    {
      steering /= static_cast<float>(count_velocities);

      _debug_align(steering);
      m_acceleration += steering * ALIGN_FORCE;
    }
  }

  void _debug_align(sf::Vector2f steering)
  {
    if (DEBUG_2)
    {
      sf::Vertex line[] = {
          sf::Vertex(m_position),
          sf::Vertex(m_position + (steering * 10.f))};
      line[0].color = sf::Color::Red;
      line[1].color = sf::Color::Red;
      m_window.draw(line, 2, sf::Lines);
    }
  }

  void cohesion()
  {
    sf::Vector2f avg_pos(0.f, 0.f);
    sf::Vector2f steering(0.f, 0.f);

    int count_pos = 0;
    for (auto p : m_neighbors_pos)
    {
      avg_pos += p;
      count_pos++;
    }

    if (count_pos > 0)
    {
      avg_pos /= static_cast<float>(count_pos);
      float delta_x = avg_pos.x - m_position.x;
      float delta_y = avg_pos.y - m_position.y;
      float theta_rad = atan2(delta_y, delta_x);
      steering = sf::Vector2f(cos(theta_rad), sin(theta_rad));

      _debug_cohesion(steering, avg_pos);

      m_acceleration += steering * COHESION_FORCE;
    }
  }

  void _debug_cohesion(sf::Vector2f steering, sf::Vector2f avg_pos)
  {
    if (DEBUG_2)
    {
      sf::Vertex line[] = {
          sf::Vertex(m_position),
          sf::Vertex(m_position + (steering * 20.f))};
      line[0].color = sf::Color::Green;
      line[1].color = sf::Color::Green;
      m_window.draw(line, 2, sf::Lines);
      line[1] = avg_pos; // * static_cast<float>(count_pos);
      line[1].color = sf::Color::Yellow;
      m_window.draw(line, 2, sf::Lines);
    }
  }

  void separation()
  {
    sf::Vector2f steering(0.f, 0.f);

    int count_pos = 0;
    for (auto p : m_neighbors_pos)
    {
      float dist = vec2f::dist(m_position, p);
      if (dist > 0)
      {
        steering += (SEPARATION_BIAS / dist) * (m_position - p);
        count_pos++;
      }
    }
    if (count_pos > 0)
    {
      steering /= static_cast<float>(count_pos);
      _debug_separation(steering);
      m_acceleration += steering * SEPARATION_FORCE;
    }
  }

  void _debug_separation(sf::Vector2f steering)
  {
    if (DEBUG_2)
    {
      sf::Vertex line[] = {
          sf::Vertex(m_position),
          sf::Vertex(m_position + (steering * 100.f))};
      line[0].color = sf::Color::Blue;
      line[1].color = sf::Color::Blue;
      m_window.draw(line, 2, sf::Lines);
    }
  }

  void edges()
  {
    if (m_position.x < 0)
    {
      m_position.x = m_window.getSize().x;
    }
    else if (m_position.x > m_window.getSize().x)
    {
      m_position.x = 0;
    }

    if (m_position.y < 0)
    {
      m_position.y = m_window.getSize().y;
    }
    else if (m_position.y > m_window.getSize().y)
    {
      m_position.y = 0;
    }
  }

  sf::Vector2f position()
  {
    return m_position;
  }

  sf::Vector2f velocity()
  {
    return m_velocity;
  }

private:
  sf::Vector2f m_position;
  sf::Vector2f m_velocity;
  int m_id;
  sf::RenderWindow &m_window;
  sf::Vector2f m_acceleration;
  float m_size;
  float m_awareness;
  std::vector<sf::Vector2f> m_neighbors_pos;
  std::vector<sf::Vector2f> m_neighbors_vel;
};

int main()
{
  // Create a window with a specified size and title
  sf::RenderWindow window(
      sf::VideoMode(consts::window::width,
                    consts::window::height),
      "Boids");

  window.setFramerateLimit(consts::FRAME_RATE);

  std::vector<Boid *> boids;
  for (int i = 0; i < NUM_BOIDS; i++)
  {
    boids.push_back(
        new Boid(sf::Vector2f(get_rand<float>(0.f, window.getSize().x), get_rand<float>(0.f, window.getSize().y)),
                 sf::Vector2f(get_rand<float>(-1.f, 1.f), get_rand<float>(-1.f, 1.f)),
                 i,
                 window));
  }

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

    window.clear();

    for (int i = 0; i < NUM_BOIDS; i++)
    {
      Boid *b = boids.at(i);
      b->flock(boids, window);
      b->draw();
      b->move();
    }

    window.display();
  }

  return 0;
}
