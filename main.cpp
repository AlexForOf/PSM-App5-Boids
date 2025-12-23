#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include "Boid.cpp"
#include "Predator.cpp"

const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 720;
const int BOID_COUNT = 500;

static bool debugMode = false;
static sf::Font font;

std::vector<Boid> spawnFlock(int count)
{
 std::vector<Boid> flock;
 flock.reserve(BOID_COUNT);

 std::random_device rd;
 std::mt19937 gen(rd());
 std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(WINDOW_WIDTH));
 std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(WINDOW_HEIGHT));

 sf::Vector2f predatorPos(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);
 float safeRadius = 100.0f;
 float safeRadiusSq = safeRadius * safeRadius;

 for (int i = 0; i < BOID_COUNT; ++i)
 {
  float startX, startY;
  float distSq;

  do
  {
   startX = distX(gen);
   startY = distY(gen);

   float dx = startX - predatorPos.x;
   float dy = startY - predatorPos.y;

   distSq = (dx * dx) + (dy * dy);

  } while (distSq < safeRadiusSq);

  flock.emplace_back(startX, startY);
 }

 return flock;
}

void updateFlock(float dt, std::vector<Boid> &flock, Predator &predator, sf::RenderWindow &window)
{
 std::vector<sf::Vector2f> currentFrameForces;
 currentFrameForces.reserve(flock.size());

 sf::Vector2f predatorForce = predator.hunt(flock);
 sf::Vector2f totalForce;

 struct DebugInfo
 {
  sf::Vector2f force;
  std::string state;
 };

 std::vector<DebugInfo> debugData;
 debugData.reserve(flock.size());

 int index = 0;
 for (auto &boid : flock)
 {
  sf::Vector2f flee = boid.flee(predator.getPosition());
  std::string stateStr = "IDLE";

  if (flee.lengthSquared() > 0.001f)
  {
   sf::Vector2f sep = boid.separation(flock);

   std::cout << " Flee vector(x,y)" << flee.x << ", " << flee.y << " on index boid: " << index << std::endl;
   totalForce = (flee * 5.0f) + (sep * 2.0f);
   stateStr = "FLEE";
  }
  else
  {
   sf::Vector2f sep = boid.separation(flock);
   sf::Vector2f ali = boid.alignment(flock);
   sf::Vector2f coh = boid.cohesion(flock);

   bool isMouseDown = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
   sf::Vector2i mousePointInt = sf::Mouse::getPosition(window);

   if (isMouseDown && mousePointInt.x >= 0 && mousePointInt.x <= WINDOW_WIDTH && mousePointInt.y >= 0 && mousePointInt.y <= WINDOW_HEIGHT)
   {
    sf::Vector2f mousePos = sf::Vector2f(static_cast<float>(mousePointInt.x), static_cast<float>(mousePointInt.y));

    sf::Vector2f mouseForce = boid.seek(mousePos);

    totalForce = (sep * 1.5f) + (ali * 1.0f) + (coh * 0.8f) + (mouseForce * 2.5f);
    stateStr = "SEEK";
   }
   else
   {
    totalForce = (sep * 1.5f) + (ali * 1.0f) + (coh * 0.8f);
    stateStr = "FLOCK";
   }
  }

  currentFrameForces.push_back(totalForce);

  debugData.push_back({totalForce, stateStr});
  index++;
 }

 predator.applyForce(predatorForce);
 predator.update(dt, WINDOW_WIDTH, WINDOW_HEIGHT);

 for (size_t i = 0; i < flock.size(); ++i)
 {
  flock[i].applyForce(currentFrameForces[i]);

  flock[i].update(dt, WINDOW_WIDTH, WINDOW_HEIGHT);

  if (debugMode)
  {
   std::cout << "Debug drawn" << std::endl;
   flock[i].drawDebug(window, font, debugData[i].force, debugData[i].state);
  }
 }
}

void renderFlock(sf::RenderWindow &window, std::vector<Boid> &flock)
{
 for (auto &boid : flock)
 {
  boid.draw(window);
 }
}

int main()
{
 sf::RenderWindow window(sf::VideoMode({WINDOW_WIDTH, WINDOW_HEIGHT}), "Boid Simulation");
 window.setFramerateLimit(60);

 font = sf::Font();

 if (!font.openFromFile("roboto.ttf"))
 {
  return -1;
 }

 std::cout << font.getInfo().family;

 sf::Clock clock;
 std::vector<Boid> flock = spawnFlock(BOID_COUNT);
 Predator predator(WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2);

 while (window.isOpen())
 {
  while (const std::optional event = window.pollEvent())
  {
   if (event->is<sf::Event::Closed>())
   {
    window.close();
   }

   if (event->is<sf::Event::KeyPressed>())
   {
    if (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::D)
    {
     debugMode = !debugMode;
    }
   }
  }

  float dt = clock.restart().asSeconds();
  updateFlock(dt, flock, predator, window);

  window.clear(sf::Color::White);
  renderFlock(window, flock);
  predator.draw(window);

  window.display();
 }

 return 0;
}