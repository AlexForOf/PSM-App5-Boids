#include "Boid.cpp"
#include <limits>

static const float HUNT_DISTANCE = 300.0f;

class Predator : public Boid
{
public:
 Predator(float x, float y) : Boid(x, y, 5.0f)
 {
  shape.setFillColor(sf::Color::Red);
  shape.setScale(sf::Vector2f(2.0f, 2.0f));
 }

 sf::Vector2f hunt(const std::vector<Boid> &flock)
 {
  float closestDistSqr = std::numeric_limits<float>::max();
  sf::Vector2f targetVector = sf::Vector2f(0, 0);
  bool foundPrey = false;

  for (const auto &boid : flock)
  {
   sf::Vector2f diff = boid.getPosition() - position;
   float distSqr = diff.lengthSquared();

   if (distSqr < HUNT_DISTANCE * HUNT_DISTANCE && distSqr < closestDistSqr)
   {
    closestDistSqr = distSqr;
    targetVector = diff;
    foundPrey = true;
   }
  }

  if (foundPrey)
  {
   return steerTowards(targetVector);
  }
  else
  {
   return getWanderForce();
  }
 }

private:
 sf::Vector2f getWanderForce()
 {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  sf::Vector2f randomDir(dist(gen), dist(gen));

  if (randomDir.lengthSquared() > 0)
   return randomDir.normalized() * (maxSpeed * 0.5f);

  return sf::Vector2f(0, 0);
 }
};