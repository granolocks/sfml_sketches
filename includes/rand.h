#include <random>
#ifndef RAND_H
#define RAND_H

template <class T>
T get_rand(T min, T max)
{
  // Initialize a random device
  std::random_device rd;

  // Initialize a Mersenne Twister pseudo-random number generator
  std::mt19937 gen(rd());

  // Set the range for the random number
  std::uniform_real_distribution<T> dist(min, max);

  return dist(gen);
}

#endif