#ifndef GAMESETTINGS_H
#define GAMESETTINGS_H

#include "Controls.h"
#include "Direction.h"
#include "GameState.h"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>

#include <array>



template<typename T>
struct GameSettings
{
public:
    static constexpr std::size_t rectVertexCount = 6;
    static constexpr std::size_t startSnakeLength = 3;
    static constexpr sf::Time updateIntervalSingleStep = sf::milliseconds(50);
    static constexpr char const *fontPath = "AdwaitaMono-Regular.ttf";
    static constexpr char const *winText = "WIN";
    static constexpr char const *lossText = "LOSS";
    static constexpr char const *vertexBufferCreateError = "Failed to create sf::VertexBuffer (insufficient VRAM or OpenGL context error).";
    static constexpr char const *vertexBufferUpdateError = "Failed to update sf::VertexBuffer data.";

    constexpr GameState getInitialGameState() const;
    constexpr Controls const & getControls() const;

    constexpr sf::Color getGridColor() const;
    constexpr sf::Color getBackgroundColor() const;
    constexpr sf::Color getCellColor() const;
    constexpr sf::Color getSnakeColor() const;
    constexpr sf::Color getFoodColor() const;

    constexpr sf::Time getGameWaitPeriod() const;
    constexpr unsigned getCharacterSize() const;
    constexpr sf::Vector2f getTextScale() const;
    constexpr sf::FloatRect getTextBounds() const;
    constexpr sf::Vector2u getWindowSize() const;
    constexpr unsigned getFramerateLimit() const;
    constexpr sf::Time getUpdateInterval() const;
    constexpr Direction getStartSnakeDirection() const;
    constexpr T getCellsPerSideCount() const;
    constexpr T getCellSize() const;
    constexpr T getLineThickness() const;
    constexpr T getCellCount() const;
    constexpr T getSideLineCount() const;
    constexpr T getMidRowLastIndex() const;
    constexpr sf::FloatRect const & getWorldBounds() const;
    constexpr std::array<T, startSnakeLength> const & getStartSnakeSegments() const;
    constexpr std::size_t getFirstIndexOfGridBuffer() const;
    constexpr std::size_t getFirstIndexOfCellBuffer() const;
    constexpr std::size_t getGridVertexBufferSize() const;
    constexpr std::size_t getCellVertexBufferSize() const;

    constexpr void setInitialGameState(GameState const initialGameState);
    constexpr void setControls(Controls const &controls);
    constexpr void setGridColor(sf::Color const &newGridColor);
    constexpr void setBackgroundColor(sf::Color const &newBackgroundColor);
    constexpr void setCellColor(sf::Color const &newCellColor);
    constexpr void setSnakeColor(sf::Color const &newSnakeColor);
    constexpr void setFoodColor(sf::Color const &newFoodColor);
    constexpr void setGameWaitPeriod(sf::Time const startCountdownValue);
    constexpr void setCharacterSize(unsigned const characterSize);
    constexpr void setTextScale(sf::Vector2f const &textScale);
    constexpr void setTextBounds(sf::FloatRect const &textBounds);
    constexpr void setWindowSize(sf::Vector2u const &startWindowSize);
    constexpr void setFramerateLimit(unsigned const framerateLimit);
    constexpr void setUpdateInterval(sf::Time const updateInterval);
    constexpr void setStartSnakeDirection(Direction const startSnakeDirection);
    constexpr void setCellsPerSideCount(T const cellPerSideCount);
    constexpr void setCellSize(T const cellSize);
    constexpr void setLineThickness(T const lineThickness);


private:
    GameState mInitialGameState = GameState::None;
    Controls mControls = {Controls::Keyboard{
        sf::Keyboard::Key::Up,
        sf::Keyboard::Key::Right,
        sf::Keyboard::Key::Down,
        sf::Keyboard::Key::Left}};

    sf::Color mBackgroundColor = sf::Color::Black;
    sf::Color mGridColor = {100, 100, 105}; //grey
    sf::Color mCellColor = {33, 34, 34};    //dark grey
    sf::Color mSnakeColor = {6, 104, 43};   //dark green
    sf::Color mFoodColor = {255, 165, 0};   //orange

    sf::Time mGameWaitPeriod = sf::seconds(3);
    unsigned mCharacterSize = 400;
    sf::Vector2f mTextScale = {0.1f, 0.1f};
    sf::FloatRect mTextBounds = {sf::Vector2f{0.f, 0.f}, sf::Vector2f{90.f, 90.f}};
    sf::Vector2u mWindowSize = {800, 600};
    unsigned mFramerateLimit = 30;
    sf::Time mUpdateInterval = sf::milliseconds(350);
    Direction mStartSnakeDirection = Direction::Left;
    T mCellsPerSideCount = 5;
    T mCellSize = 15;
    T mLineThickness = 1;

    T mCellCount = mCellsPerSideCount * mCellsPerSideCount;
    T mSideLineCount = mCellsPerSideCount + 1;
    T mMidRowLastIdx = mCellsPerSideCount / 2 * mCellsPerSideCount + mCellsPerSideCount - 1;
    std::size_t mGridVertexBufferSize = mSideLineCount * GameSettings::rectVertexCount * 2;
    std::size_t mCellVertexBufferSize = mCellCount * GameSettings::rectVertexCount;

    sf::FloatRect mWorldBounds = {
        sf::Vector2f{0.f, 0.f},
        sf::Vector2f{static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness),
                     static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness)}};

    std::array<T, startSnakeLength> mStartSnakeSegments = {
        mMidRowLastIdx, static_cast<T>(mMidRowLastIdx - 1), static_cast<T>(mMidRowLastIdx - 2)};

    static constexpr std::size_t firstIndexOfGridBuffer = 0;
};


template<typename T>
constexpr GameState GameSettings<T>::getInitialGameState() const
{
    return mInitialGameState;
}

template<typename T>
constexpr Controls const & GameSettings<T>::getControls() const
{
    return mControls;
}

template<typename T>
constexpr sf::Color GameSettings<T>::getGridColor() const
{
    return mGridColor;
}

template<typename T>
constexpr sf::Color GameSettings<T>::getBackgroundColor() const
{
    return mBackgroundColor;
}

template<typename T>
constexpr sf::Color GameSettings<T>::getCellColor() const
{
    return mCellColor;
}

template<typename T>
constexpr sf::Color GameSettings<T>::getSnakeColor() const
{
    return mSnakeColor;
}

template<typename T>
constexpr sf::Color GameSettings<T>::getFoodColor() const
{
    return mFoodColor;
}

template<typename T>
constexpr sf::Time GameSettings<T>::getGameWaitPeriod() const
{
    return mGameWaitPeriod;
}

template<typename T>
constexpr unsigned GameSettings<T>::getCharacterSize() const
{
    return mCharacterSize;
}

template<typename T>
constexpr sf::Vector2f GameSettings<T>::getTextScale() const
{
    return mTextScale;
}

template<typename T>
constexpr sf::FloatRect GameSettings<T>::getTextBounds() const
{
    return mTextBounds;
}

template<typename T>
constexpr sf::Vector2u GameSettings<T>::getWindowSize() const
{
    return mWindowSize;
}

template<typename T>
constexpr unsigned GameSettings<T>::getFramerateLimit() const
{
    return mFramerateLimit;
}

template<typename T>
constexpr sf::Time GameSettings<T>::getUpdateInterval() const
{
    return mUpdateInterval;
}

template<typename T>
constexpr Direction GameSettings<T>::getStartSnakeDirection() const
{
    return mStartSnakeDirection;
}

template<typename T>
constexpr T GameSettings<T>::getCellsPerSideCount() const
{
    return mCellsPerSideCount;
}

template<typename T>
constexpr T GameSettings<T>::getCellSize() const
{
    return mCellSize;
}

template<typename T>
constexpr T GameSettings<T>::getLineThickness() const
{
    return mLineThickness;
}

template<typename T>
constexpr T GameSettings<T>::getCellCount() const
{
    return mCellCount;
}

template<typename T>
constexpr T GameSettings<T>::getSideLineCount() const
{
    return mSideLineCount;
}

template<typename T>
constexpr T GameSettings<T>::getMidRowLastIndex() const
{
    return mMidRowLastIdx;
}

template<typename T>
constexpr sf::FloatRect const & GameSettings<T>::getWorldBounds() const
{
    return mWorldBounds;
}

template<typename T>
constexpr std::array<T, GameSettings<T>::startSnakeLength> const & GameSettings<T>::getStartSnakeSegments() const
{
    return mStartSnakeSegments;
}

template<typename T>
constexpr std::size_t GameSettings<T>::getFirstIndexOfGridBuffer() const
{
    return GameSettings<T>::firstIndexOfGridBuffer;
}

template<typename T>
constexpr std::size_t GameSettings<T>::getFirstIndexOfCellBuffer() const
{
    return GameSettings<T>::firstIndexOfGridBuffer + mGridVertexBufferSize;
}

template<typename T>
constexpr std::size_t GameSettings<T>::getGridVertexBufferSize() const
{
    return mGridVertexBufferSize;
}

template<typename T>
constexpr std::size_t GameSettings<T>::getCellVertexBufferSize() const
{
    return mCellVertexBufferSize;
}


template<typename T>
constexpr void GameSettings<T>::setInitialGameState(GameState const initialGameState)
{
    mInitialGameState = initialGameState;
}

template<typename T>
constexpr void GameSettings<T>::setControls(Controls const &controls)
{
    mControls = controls;
}

template<typename T>
constexpr void GameSettings<T>::setGridColor(const sf::Color &newGridColor)
{
    mGridColor = newGridColor;
}

template<typename T>
constexpr void GameSettings<T>::setBackgroundColor(const sf::Color &newBackgroundColor)
{
    mBackgroundColor = newBackgroundColor;
}

template<typename T>
constexpr void GameSettings<T>::setCellColor(const sf::Color &newCellColor)
{
    mCellColor = newCellColor;
}

template<typename T>
constexpr void GameSettings<T>::setSnakeColor(const sf::Color &newSnakeColor)
{
    mSnakeColor = newSnakeColor;
}

template<typename T>
constexpr void GameSettings<T>::setFoodColor(const sf::Color &newFoodColor)
{
    mFoodColor = newFoodColor;
}

template<typename T>
constexpr void GameSettings<T>::setGameWaitPeriod(sf::Time const startCountdownValue)
{
    mGameWaitPeriod = startCountdownValue;
}

template<typename T>
constexpr void GameSettings<T>::setCharacterSize(unsigned const characterSize)
{
    mCharacterSize = characterSize;
}

template<typename T>
constexpr void GameSettings<T>::setTextScale(sf::Vector2f const &textScale)
{
    mTextScale = textScale;
}

template<typename T>
constexpr void GameSettings<T>::setTextBounds(sf::FloatRect const &textBounds)
{
    mTextBounds = textBounds;
}


template<typename T>
constexpr void GameSettings<T>::setWindowSize(sf::Vector2u const &startWindowSize)
{
    mWindowSize = startWindowSize;
}

template<typename T>
constexpr void GameSettings<T>::setFramerateLimit(unsigned const framerateLimit)
{
    mFramerateLimit = framerateLimit;
}

template<typename T>
constexpr void GameSettings<T>::setUpdateInterval(sf::Time const updateInterval)
{
    mUpdateInterval = updateInterval;
}

template<typename T>
constexpr void GameSettings<T>::setStartSnakeDirection(Direction const startSnakeDirection)
{
    mStartSnakeDirection = startSnakeDirection;
}

template<typename T>
constexpr void GameSettings<T>::setCellsPerSideCount(T const cellPerSideCount)
{
    mCellsPerSideCount = cellPerSideCount;

    mCellCount = mCellsPerSideCount * mCellsPerSideCount;
    mSideLineCount = mCellsPerSideCount + 1;
    mMidRowLastIdx = mCellsPerSideCount / 2 * mCellsPerSideCount + mCellsPerSideCount - 1;
    mGridVertexBufferSize = mSideLineCount * GameSettings::rectVertexCount * 2;
    mCellVertexBufferSize = mCellCount * GameSettings::rectVertexCount;

    mWorldBounds = sf::FloatRect{
        mWorldBounds.position,
        sf::Vector2f{static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness),
                     static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness)}};

    mStartSnakeSegments = {
        mMidRowLastIdx, static_cast<T>(mMidRowLastIdx - 1), static_cast<T>(mMidRowLastIdx - 2)};
}

template<typename T>
constexpr void GameSettings<T>::setCellSize(T const cellSize)
{
    mCellSize = cellSize;

    mWorldBounds = sf::FloatRect{
        mWorldBounds.position,
        sf::Vector2f{static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness),
                     static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness)}};
}

template<typename T>
constexpr void GameSettings<T>::setLineThickness(T const lineThickness)
{
    mLineThickness = lineThickness;

    mWorldBounds = sf::FloatRect{
        mWorldBounds.position,
        sf::Vector2f{static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness),
                     static_cast<float>(mCellsPerSideCount * mCellSize + mSideLineCount * mLineThickness)}};
}

#endif // GAMESETTINGS_H