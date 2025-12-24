#include "boids/boid.h"

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>

#include <cmath>
#include <random>
#include <iostream>

Boid::Boid(float x, float y, float speed) : maxSpeed(speed), minSpeed(speed * 0.5)
{
  position = sf::Vector2f(x, y);

  static std::random_device rd;
  static std::mt19937 gen(rd());

  std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

  sf::Vector2f randomDir(dist(gen), dist(gen));

  if (randomDir.lengthSquared() > 0.001f)
  {
   velocity = randomDir.normalized() * maxSpeed;
  }
  else
  {
   velocity = sf::Vector2f(1.0f, 0.0f) * maxSpeed;
  }

  shape.setPointCount(3);
  shape.setPoint(0, sf::Vector2f(10, 0));
  shape.setPoint(1, sf::Vector2f(-5, -5));
  shape.setPoint(2, sf::Vector2f(-5, 5));
  shape.setPosition(position);
  shape.setFillColor(sf::Color::Cyan);
 }

sf::Vector2f Boid::separation(const std::vector<Boid> &flock)
{
  sf::Vector2f steering = sf::Vector2f(0.0f, 0.0f);
  int count = 0;

  for (const auto &other : flock)
  {
   if (&other == this)
    continue;

   sf::Vector2f diff = this->position - other.position;
   float distance = diff.length();

   if (distance < SEPARATION_RADIUS)
   {
    if (distance > 0)
    {
     diff = diff.normalized() / distance;
    }
    steering += diff;
    count++;
   }
  }

  if (count > 0)
  {
   steering /= static_cast<float>(count);
  }
  return steerTowards(steering);
}

sf::Vector2f Boid::alignment(const std::vector<Boid> &flock)
{
  sf::Vector2f steering = sf::Vector2f(0.0f, 0.0f);
  int count = 0;

  for (const auto &other : flock)
  {
   if (&other == this)
    continue;

   float distanceSqrt = (position - other.position).lengthSquared();
   if (distanceSqrt < ALIGNMENT_RADIUS * ALIGNMENT_RADIUS && distanceSqrt > 0.001f)
   {
    steering += other.velocity;
    count++;
   }
  }

  if (count > 0)
  {
   steering /= static_cast<float>(count);
  }
  return steerTowards(steering);
}

sf::Vector2f Boid::cohesion(const std::vector<Boid> &flock)
{
  sf::Vector2f centerOfMass = sf::Vector2f(0.0f, 0.0f);
  int count = 0;

  for (const auto &other : flock)
  {
   if (&other == this)
    continue;

   float distanceSqrt = (this->position - other.position).lengthSquared();
   if (distanceSqrt < ALIGNMENT_RADIUS * ALIGNMENT_RADIUS && distanceSqrt > 0.001f)
   {
    centerOfMass += other.position;
    count++;
   }
  }

  sf::Vector2f desiredDir = sf::Vector2f(0, 0);

  if (count > 0)
  {
   centerOfMass /= static_cast<float>(count);

   desiredDir = centerOfMass - position;
  }

  return steerTowards(desiredDir);
 }

 sf::Vector2f Boid::flee(const sf::Vector2f &predatorPos)
 {
  sf::Vector2f diff = position - predatorPos;
  float distanceSqr = diff.lengthSquared();

  if (distanceSqr > FEAR_RADIUS * FEAR_RADIUS || distanceSqr < 0.001f)
  {
   return sf::Vector2f(0, 0);
  }

  diff = diff.normalized() * maxSpeed;

  sf::Vector2f steer = diff - velocity;

  return steer;
}

sf::Vector2f Boid::seek(const sf::Vector2f &targetPos)
{
  sf::Vector2f diff = targetPos - position;
  float diffLengthSq = diff.lengthSquared();
  if (diffLengthSq < 1.0f || diffLengthSq > SEEK_RADIUS * SEEK_RADIUS)
   return sf::Vector2f(0, 0);

  return steerTowards(diff);
}

void Boid::update(float deltaTime, float windowWidth, float windowHeight)
{
  position += velocity * deltaTime;

  if (position.x > windowWidth)
   position.x = position.x - windowWidth;
  if (position.x < 0)
   position.x = windowWidth + (position.x);

  if (position.y > windowHeight)
   position.y = position.y - windowHeight;
  if (position.y < 0)
   position.y = windowHeight + (position.y);

  shape.setPosition(position);
  shape.setRotation(velocity.angle());
}

void Boid::applyForce(sf::Vector2f force)
{
  this->velocity += force;
}

void Boid::draw(sf::RenderWindow &window)
{
  window.draw(shape);
}

void Boid::drawDebug(sf::RenderWindow &window, const sf::Font &font, sf::Vector2f currentForce, std::string stateText)
{
  float lineScale = 150.0f;
  sf::Vector2f lineEnd = position + (currentForce * lineScale);

  sf::Vertex line[] =
      {
          {{position.x, position.y}, sf::Color::Magenta},
          {{lineEnd.x, lineEnd.y}, sf::Color::Magenta}};

  window.draw(line, 2, sf::PrimitiveType::Lines);

  sf::Text text(font);
  text.setString(stateText);
  text.setCharacterSize(12);
  text.setFillColor(sf::Color::Green);

  sf::FloatRect bounds = text.getLocalBounds();
  std::cout << "text bounds" << bounds.position.x << " " << bounds.position.y;
  text.setOrigin(bounds.getCenter());
  text.setPosition(position + sf::Vector2f(0, -15));

  std::cout << "text position" << text.getPosition().x << " " << text.getPosition().y;
  window.draw(text);
}

sf::Vector2f Boid::getPosition() const
{
  return position;
}

sf::Vector2f Boid::steerTowards(sf::Vector2f targetVector)
{
  if (targetVector.lengthSquared() < 0.001f)
    return sf::Vector2f(0, 0);

  sf::Vector2f desired = targetVector.normalized() * maxSpeed;

  sf::Vector2f steer = desired - velocity;

  if (steer.lengthSquared() > MAX_FORCE * MAX_FORCE)
  {
   steer = steer.normalized() * MAX_FORCE;
  }

  return steer;
}
