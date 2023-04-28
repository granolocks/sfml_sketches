#include <iostream>
#include <SFML/Graphics.hpp>
#include "includes/consts.h"
#include "includes/helpers.h"

const int W = 1920;
const int H = 1080;
const int FRAME_LIMIT = 60;

const float GRAVITY = 0.1f;
const float JUMP = 1.5f;
const float RUN = 1.f;
const float RUN_STOP = 0.2f;
const float MAX_RUN = 10.f;

sf::Vector2f DOWN(0.f, 1.f);
sf::Vector2f UP(0.f, -1.f);
sf::Vector2f LEFT(-1.f, 0.f);
sf::Vector2f RIGHT(1.f, 0.f);

class Actor
{
public:
  Actor() : m_spawned(false){};
  ~Actor(){};

  void spawn(float x, float y)
  {
    m_position.x = x;
    m_position.y = y;
    m_spawned = true;
  }

  void jump()
  {
    if (!m_jumping)
    {
      accelerate(UP * JUMP);
      m_jumping = true;
    }
  }

  void run(sf::Vector2f direction)
  {
    m_running = true;
    m_run_dir = direction;
  }

  void stop_run()
  {
    m_running = false;
  }

  void accelerate(sf::Vector2f force)
  {
    m_acceleration += force;
  }

  void move()
  {
    if (m_running)
    {
      m_acceleration.x = m_run_dir.x * RUN;
    }
    else
    {
      if (grounded())
      {
        bool stopped = m_velocity.x == 0;
        if (!stopped && m_run_dir == RIGHT && m_acceleration.x > 0)
        {
          m_acceleration += LEFT * RUN_STOP;
          stopped = m_acceleration.x <= 0;
        }
        else if (!stopped && m_run_dir == LEFT && m_acceleration.x < 0)
        {
          m_acceleration += RIGHT * RUN_STOP;
          stopped = m_acceleration.x >= 0;
        }

        if (stopped)
        {
          m_acceleration.x = 0.f;
          m_velocity.x = 0.f;
        }
      }
    }

    m_velocity += m_acceleration;
    enforce_max_run_speed();
    m_position += m_velocity;

    if (grounded())
    {
      m_velocity.y = 0.f;
      m_acceleration.y = 0.f;
      m_position.y = H - 50.f;
      m_jumping = false;
    }
  }

  void enforce_max_run_speed()
  {
    if (std::abs(m_velocity.x) > MAX_RUN)
    {
      m_velocity.x = m_run_dir.x * MAX_RUN;
    }
  }

  bool grounded()
  {
    return m_position.y + 50.f >= H;
  }

  void draw(sf::RenderWindow &window)
  {
    if (m_spawned)
    {
      sf::RectangleShape rect;
      rect.setSize(sf::Vector2f(100.f, 100.f));
      rect.setOrigin(sf::Vector2f(50.f, 50.f));
      rect.setPosition(m_position);
      rect.setFillColor(consts::colors::pink);
      window.draw(rect);
    }
  }

private:
  bool m_spawned;
  bool m_jumping;
  bool m_running;
  sf::Vector2f m_run_dir;
  sf::Vector2f m_position;
  sf::Vector2f m_velocity;
  sf::Vector2f m_acceleration;
};

int main()
{
  sf::RenderWindow window(sf::VideoMode(W, H), "Template");
  window.setFramerateLimit(FRAME_LIMIT);

  Actor player;
  player.spawn(static_cast<float>(W / 2),
               static_cast<float>(H / 2));

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
      case sf::Event::KeyReleased:
        switch (event.key.code)
        {
        case sf::Keyboard::Key::A:
          player.stop_run();
          break;
        case sf::Keyboard::Key::D:
          player.stop_run();
          break;
        default:
          break;
        }
        break;

      case sf::Event::KeyPressed:
        switch (event.key.code)
        {
        case sf::Keyboard::Key::Space:
          player.jump();
          break;
        case sf::Keyboard::Key::A:
          player.run(LEFT);
          break;
        case sf::Keyboard::Key::D:
          player.run(RIGHT);
          break;
        default:
          break;
        }
        break;
      default:
        break;
      }
    }

    player.accelerate(DOWN * GRAVITY);
    player.move();
    window.clear();
    player.draw(window);
    window.display();
  }

  return 0;
}
