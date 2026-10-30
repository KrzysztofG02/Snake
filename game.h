#ifndef GAME_H
#define GAME_H

#include "gamesettings.h"
#include "mainwindow.h"
#include "snake.h"

#include <QObject>
#include <QTimer>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/VertexBuffer.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>

#include <random>
#include <unordered_map>



class Game: public QObject
{
Q_OBJECT

public:
    static Game & getInstance();

    GameSettings<unsigned short> const & getGameSettings();
    void startNewGame();

    void setCellPerSizeCount(unsigned short const cellPerSizeCount);
    void setLineThickness(unsigned short const lineThickness);
    void setGameInterval(sf::Time const interval);
    void setResolution(sf::Vector2u const &resolution);

public:
    static constexpr char const *gameName = "Snake";


private:
    Game();

    void initGame();
    void initCountdownState();
    void initInGameState();
    void initLossState();
    void initWinState();
    void setText(char const * const text);

    void run();

    void close();

    void pollEvents();
    void update();
        void updateCountdown();
        void updateInGame();
        void updateEndGame();
    void render();
        void windowClear();
        void windowDraw();
        void windowDisplay();

    void advanceViewport(sf::Vector2u const &size, sf::View &view, sf::Vector2f const &originSize);
    void updateControls(sf::Event::KeyPressed const &keyPressedEvent);
    void initGrid();
    void initCells();
    void spawnFood();
    void randomizeFoodPosition();
    void setCellColor(unsigned short const cellIndex, sf::Color const color);


private:
    static constexpr GameSettings<unsigned short> defaultSettings = {};

    GameSettings<unsigned short> mCurrentSettings;

    GameState mGameState;
    std::unordered_map<sf::Keyboard::Key, Direction> mKeyboardControls;
    MainWindow mMainWindow;
    QTimer mLoopTrigger;
    sf::Time mTimeSinceLastUpdate;
    sf::Clock mGameClock;
    sf::View mView;
    sf::View mTextView;
    sf::RenderWindow mRenderWindow;
    sf::VertexBuffer mVertexBuffer;
    std::mt19937 mRng;

    FreeIndicesArray<unsigned short> mFreeCells;
    Snake<unsigned short> mSnake;
    unsigned short mFoodPosition;
    Direction mChosenDirection;
    sf::Font mFont;
    sf::Text mText;
    sf::Time mCountdownValue;
};

#endif // GAME_H
