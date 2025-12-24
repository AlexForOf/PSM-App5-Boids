#pragma once

#include <SFML/Graphics.hpp>

static const float MAX_FORCE = 0.1f;

static const float SEPARATION_RADIUS = 25.0f;
static const float ALIGNMENT_RADIUS = 50.0f;
static const float FEAR_RADIUS = 60.0f;
static const float SEEK_RADIUS = 350.0f;

class Boid {
  public:
    Boid(float x, float y, float speed = 4.0f);
    sf::Vector2f separation(const std::vector<Boid> &flock);
    sf::Vector2f alignment(const std::vector<Boid> &flock);
    sf::Vector2f cohesion(const std::vector<Boid> &flock);
    sf::Vector2f flee(const sf::Vector2f &predatorPos);
    sf::Vector2f seek(const sf::Vector2f &targetPos);
    void applyForce(sf::Vector2f force);
    void draw(sf::RenderWindow &window);
    void update(float deltaTime, float windowWidth, float windowHeight);
    void drawDebug(sf::RenderWindow &window, const sf::Font &font, sf::Vector2f currentForce, std::string stateText);
    sf::Vector2f getPosition() const;
  protected:
    float maxSpeed;
    float minSpeed;
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::ConvexShape shape;
    
    sf::Vector2f steerTowards(sf::Vector2f targetVector);
};
