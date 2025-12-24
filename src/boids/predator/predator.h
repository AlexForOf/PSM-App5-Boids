#pragma once

#include <SFML/Graphics.hpp>
#include "boids/boid.h"

static const float HUNT_DISTANCE = 300.0f;

class Predator : public Boid 
{
  public:
    Predator(float x, float y);
    sf::Vector2f hunt(const std::vector<Boid> &flock);
  private:
    sf::Vector2f getWanderForce();
};
