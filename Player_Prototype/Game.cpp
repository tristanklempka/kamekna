#include "Game.h"

int numFootContacts = 0;
const float SCALE = 30.f; // Box2D works in a scale of 30 pixels = 1 meter
const sf::Time TIME_PER_FRAME = sf::seconds(1.f/60.f);

Game::Game():
    m_window(sf::VideoMode(1280, 720), "Prototype player"),
    m_world(b2Vec2(0, 15.0f)),
    m_gameView(m_window.getView()),
    m_miniMapView(m_window.getView()),
    ladral(nullptr)
{
    m_window.setView(m_gameView);
    m_miniMapView.zoom(4.0f);
    m_miniMapView.setViewport(sf::FloatRect(0.85f, 0.f, 0.15f, 0.15f));

    numFootContacts = 0;
    m_world.SetContactListener(&m_contactListener);

        //Floor
    std::unique_ptr<Platform> tempPlatform(new Platform(m_world));
    platform = std::move(tempPlatform);

    // Player
    std::unique_ptr<Hero> tempLadral(new Hero(m_world));
    ladral = std::move(tempLadral);



        // DEBUG Text
    if (!m_font.loadFromFile("arial.ttf"))
    {
        std::cout << "Unable to load font..." << std::endl;
    }
    m_statisticsText.setFont(m_font);
    m_statisticsText.setString("DEBUG");
    m_statisticsText.setCharacterSize(24);
    m_statisticsText.setColor(sf::Color::Blue);
}

void Game::processEvents()
{
    sf::Event event;
    while (m_window.pollEvent(event))
    {
        ladral->handleEvent(event);
        if (event.type == sf::Event::Closed)
            m_window.close();
    }
}

void Game::processInputs()
{
    ladral->handleRealTimeInput();
}

void Game::update(sf::Time elapsedTime)
{

    // Update physic
    m_world.Step(elapsedTime.asSeconds(), 8, 4);
    // Aplly physic to objects
    ladral->update();
    platform->update();
}

void Game::render()
{
    m_window.clear(sf::Color::White);

    m_gameView.setCenter(ladral->getPosition());
    m_window.setView(m_gameView);
    m_window.draw(*ladral);
    m_window.draw(*platform);

    m_window.setView(m_window.getDefaultView());
    m_window.draw(m_statisticsText);

    m_window.setView(m_miniMapView);
    m_window.draw(*ladral);
    m_window.draw(*platform);

    m_window.display();
}


void Game::run()
{
    sf::Clock clock;
    sf::Time timeSinceLastUpdate = sf::Time::Zero;
    while(m_window.isOpen())
    {

        timeSinceLastUpdate += clock.restart();
        while(timeSinceLastUpdate > TIME_PER_FRAME)
        {
            timeSinceLastUpdate -= TIME_PER_FRAME;
            processEvents();
            processInputs();
            update(TIME_PER_FRAME);
        }
        updateStatistics(TIME_PER_FRAME);
        render();
    }
}

void Game::updateStatistics(sf::Time elapsedTime)
{
    m_statisticsUpdateTime += elapsedTime;
    m_statisticsNumFrames += 1;

    if(m_statisticsUpdateTime >= sf::seconds(1.0f))
    {
        std::string str = static_cast<std::ostringstream*>(&(std::ostringstream()
        << "Fps: " << m_statisticsNumFrames << "\nTime/update: " << m_statisticsUpdateTime.asMicroseconds()/m_statisticsNumFrames << "us"))
        ->str();
        m_statisticsText.setString(str);

        m_statisticsUpdateTime -= sf::seconds(1.0f);
        m_statisticsNumFrames = 0;
    }
}

