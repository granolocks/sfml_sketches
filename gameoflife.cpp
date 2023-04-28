#include <iostream>
#include <math.h>

#include <SFML/Graphics.hpp>

#include "includes/consts.h"
#include "includes/helpers.h"
#include "includes/rand.h"

const int SCALE     = 10;
const int W         = 1920;
const int H         = 1080;
const int NUM_ROWS  = H / SCALE;
const int NUM_COLS  = W / SCALE;
const int NUM_CELLS = NUM_COLS * NUM_ROWS;

// relative to any cell index these are the offsets where we we 
// expect to find a neighbor, the middle is ignored b/c it is the 
// cell in question
const int N_NEIGHBORS = 8;
const int NEIGHBOR_OFFSETS[N_NEIGHBORS] = { 
  -NUM_COLS - 1, -NUM_COLS, -NUM_COLS + 1,
  -1,            /*self*/   1,
  NUM_COLS - 1,  NUM_COLS,  NUM_COLS + 1
};

const int FRAME_LIMIT = 10; // fps

const sf::Color C_ALIVE_AND_WELL = consts::colors::bright_green;
const sf::Color C_DEAD           = consts::colors::dark_gray;
const sf::Color C_DYING          = consts::colors::light_red;
const sf::Color C_ZOMBIE         = consts::colors::dark_yellow;

class Cell
{
  public:
    Cell()
    {
      m_alive = static_cast<int>(get_rand<float>(0, 2)) == 1;
    }

    ~Cell() {}

    bool alive()
    {
      return m_alive;
    }

    void index(int i)
    {
      m_index = i;
      m_xy = helpers::index_to_xy(i, NUM_COLS);
    }

    int index()
    {
      return m_index;
    }

    sf::Vector2f position_v2f()
    {
      return sf::Vector2f(
          static_cast<float>(SCALE * m_xy.first),
          static_cast<float>(SCALE * m_xy.second));
    }

    std::vector<int> neighbor_idx()
    {
      return m_neighbor_idx;
    }

    void calc_neighbor_idx()
    {
      for (int i = 0; i < N_NEIGHBORS; i ++)
      {
        int offset = NEIGHBOR_OFFSETS[i];
        int neighbor_idx = offset + m_index;

        bool inside_cell_bounds = neighbor_idx >= 0 
                                  && neighbor_idx < NUM_CELLS;

        bool not_on_far_side = !(m_xy.first == 0 && offset == -1) 
                               && !(m_xy.first == NUM_COLS - 1 && offset == 1);

        if (inside_cell_bounds && not_on_far_side) 
        {
          m_neighbor_idx.push_back(neighbor_idx);
        }
      }
    }

    bool will_be_alive(int count_live_neighbors)
    {
      int n2 = count_live_neighbors == 2;
      int n3 = count_live_neighbors == 3;

      if (m_alive && (n2 || n3))
      {
        // stay alive with 2 or 3 live neighbors
        return true;
      }
      else if (!m_alive && n3)
      {
        // resurrect with 3 lives neighbors if dead
        return true;
      }
      else 
      {
        // die
        return false;
      }
    }

    sf::Color color(int n_count)
    {
      if (!m_alive)
      {
        return will_be_alive(n_count) ? C_ZOMBIE : C_DEAD;
      }
      else
      {
        return will_be_alive(n_count) ? C_ALIVE_AND_WELL : C_DYING;
      }
    }

    void live_or_die(int count_live_neighbors)
    {
      m_alive = will_be_alive(count_live_neighbors);
    }

  private:
    bool m_alive;
    int m_index;
    std::pair<int, int> m_xy;
    std::vector<int> m_neighbor_idx;
};

int main()
{
  sf::RenderWindow window(sf::VideoMode(W, H), "Game of Life");
  window.setFramerateLimit(FRAME_LIMIT);

  int neighbor_count[NUM_CELLS] = {0};
  Cell cells[NUM_CELLS];

  for (int i = 0; i < NUM_CELLS; i++)
  {
    cells[i].index(i);
    cells[i].calc_neighbor_idx();
  }

  sf::RectangleShape c_rect;
  c_rect.setSize(sf::Vector2f(static_cast<float>(SCALE), 
                              static_cast<float>(SCALE)));

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

    // count all live neighbors in one pass before drawing so we get
    // a snapshot of the whole world at once
    for (int i = 0; i < NUM_CELLS; i++)
    {
      int count_live = 0;
      Cell c = cells[i];
      for (int n_idx : c.neighbor_idx())
      {
        count_live += cells[n_idx].alive();
      }
      neighbor_count[i] = count_live;
    }

    // draw and update
    for (int i = 0; i < NUM_CELLS; i++)
    {
      Cell c = cells[i];
      int n_count = neighbor_count[i];
      c_rect.setFillColor(c.color(n_count));
      c_rect.setPosition(c.position_v2f());
      window.draw(c_rect);

      cells[i].live_or_die(n_count);
    }

    window.display();
  }

  return 0;
}
