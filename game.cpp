#include "game.h"

#include <QApplication>

#include <filesystem>

using namespace std;



Game::Game():
    mCurrentSettings{Game::defaultSettings},

    mGameState{mCurrentSettings.getInitialGameState()},
    mKeyboardControls{
        {mCurrentSettings.getControls().keyboard.up, Direction::Up},
        {mCurrentSettings.getControls().keyboard.right, Direction::Right},
        {mCurrentSettings.getControls().keyboard.down, Direction::Down},
        {mCurrentSettings.getControls().keyboard.left, Direction::Left}},
    mMainWindow{*this},
    mLoopTrigger{this},
    mTimeSinceLastUpdate{},
    mGameClock{},
    mView{},
    mTextView{},
    mRenderWindow{},
    mVertexBuffer{sf::PrimitiveType::Triangles, sf::VertexBuffer::Usage::Dynamic},
    mRng{random_device{}()},
    mFreeCells{mCurrentSettings.getCellCount()},
    mSnake{mCurrentSettings.getCellsPerSideCount(), mCurrentSettings.getStartSnakeDirection()},
    mFoodPosition{},
    mChosenDirection{Direction::None},
    mFont{mCurrentSettings.fontPath},
    mText{mFont},
    mCountdownValue{}
{
    mFont.setSmooth(true);

    mMainWindow.show();
    mText.setFont(mFont);

    connect(&mLoopTrigger, &QTimer::timeout, this, &Game::run);
    connect(&mMainWindow, &MainWindow::startRequested, this, &Game::startNewGame);
}


Game & Game::getInstance()
{
    static Game instance = Game();

    return instance;
}

GameSettings<unsigned short> const & Game::getGameSettings()
{
    return mCurrentSettings;
}

void Game::startNewGame()
{
    mMainWindow.hide();

    this->initGame();
    this->initCountdownState();
}


void Game::setCellPerSizeCount(unsigned short const cellPerSizeCount)
{
    mCurrentSettings.setCellsPerSideCount(cellPerSizeCount);
}

void Game::setLineThickness(unsigned short const lineThickness)
{
    mCurrentSettings.setLineThickness(lineThickness);
}

void Game::setGameInterval(sf::Time const interval)
{
    mCurrentSettings.setUpdateInterval(interval);
}

void Game::setResolution(sf::Vector2u const &resolution)
{
    mCurrentSettings.setWindowSize(resolution);
    mRenderWindow.setSize(resolution);

    this->advanceViewport(resolution, mView, mCurrentSettings.getWorldBounds().size);
    this->advanceViewport(resolution, mTextView, mTextView.getSize());
}


void Game::initGame()
{
    if(!mVertexBuffer.create(mCurrentSettings.getGridVertexBufferSize() + mCurrentSettings.getCellVertexBufferSize()))
    {
        throw runtime_error(mCurrentSettings.vertexBufferCreateError);
    }

    this->initGrid();
    this->initCells();

    mSnake = {mCurrentSettings.getCellsPerSideCount(), mCurrentSettings.getStartSnakeDirection()};
    mFreeCells = {mCurrentSettings.getCellCount()};

    for(unsigned short const cellIndex : mCurrentSettings.getStartSnakeSegments())
    {
        mSnake.addSegment(cellIndex, mFreeCells);
        this->setCellColor(cellIndex, mCurrentSettings.getSnakeColor());
    }

    mChosenDirection = Direction::None;

    mView = sf::View{mCurrentSettings.getWorldBounds()};
    mTextView = sf::View{mCurrentSettings.getTextBounds()};
    mRenderWindow.create(sf::VideoMode(mCurrentSettings.getWindowSize()), Game::gameName);
    mRenderWindow.requestFocus();
    mRenderWindow.setView(mView);
    mRenderWindow.setFramerateLimit(mCurrentSettings.getFramerateLimit());

    this->advanceViewport(mRenderWindow.getSize(), mView, mCurrentSettings.getWorldBounds().size);
    this->advanceViewport(mRenderWindow.getSize(), mTextView, mTextView.getSize());

    this->spawnFood();

    mLoopTrigger.start(0);
}

void Game::initCountdownState()
{
    mGameState = GameState::Countdown;

    mCountdownValue = mCurrentSettings.getGameWaitPeriod();

    this->setText(to_string(static_cast<int>(mCountdownValue.asSeconds())).c_str());

    mTimeSinceLastUpdate = sf::Time::Zero;
    mGameClock.restart();
}

void Game::initInGameState()
{
    mGameState = GameState::InGame;

    mTimeSinceLastUpdate = sf::Time::Zero;
    mGameClock.restart();
}

void Game::initLossState()
{
    mGameState = GameState::Loss;

    mCountdownValue = mCurrentSettings.getGameWaitPeriod();

    this->setText(mCurrentSettings.lossText);

    mTimeSinceLastUpdate = sf::Time::Zero;
    mGameClock.restart();
}

void Game::initWinState()
{
    mGameState = GameState::Win;

    mCountdownValue = mCurrentSettings.getGameWaitPeriod();

    this->setText(mCurrentSettings.winText);

    mTimeSinceLastUpdate = sf::Time::Zero;
    mGameClock.restart();
}

void Game::setText(char const * const text)
{
    mText.setString(text);
    mText.setCharacterSize(mCurrentSettings.getCharacterSize());
    mText.setScale(mCurrentSettings.getTextScale());

    sf::FloatRect const textBounds = mText.getLocalBounds();
    mText.setOrigin({
        textBounds.position.x + textBounds.size.x / 2.f,
        textBounds.position.y + textBounds.size.y / 2.f});

    mText.setPosition(mTextView.getCenter());
}


void Game::run()
{
    this->pollEvents();
    this->update();
    this->render();
}


void Game::close()
{
    mLoopTrigger.stop();
    mRenderWindow.close();

    mMainWindow.show();
}


void Game::pollEvents()
{
    while(optional const event = mRenderWindow.pollEvent())
    {
        if(event->is<sf::Event::Closed>())
        {
            this->close();
        }
        else if(sf::Event::Resized const * const resizedEventPtr = event->getIf<sf::Event::Resized>())
        {
            this->advanceViewport(resizedEventPtr->size, mView, mCurrentSettings.getWorldBounds().size);
            this->advanceViewport(resizedEventPtr->size, mTextView, mTextView.getSize());
        }
        else if(sf::Event::KeyPressed const * const keyPressedEventPtr = event->getIf<sf::Event::KeyPressed>())
        {
            switch(mGameState)
            {
                case GameState::InGame:
                    this->updateControls(*keyPressedEventPtr);;
                    break;

                default: break;
            }
        }
    }
}

void Game::update()
{
    switch(mGameState)
    {
        case GameState::Countdown:
            this->updateCountdown();
            break;

        case GameState::InGame:
            this->updateInGame();
            break;

        case GameState::Win:
        case GameState::Loss:
            this->updateEndGame();
            break;

        default: break;
    }
}

void Game::updateCountdown()
{
    mTimeSinceLastUpdate += mGameClock.restart();

    if(mTimeSinceLastUpdate >= sf::seconds(1))
    {
        mTimeSinceLastUpdate -= sf::seconds(1);
        mCountdownValue -= sf::seconds(1);

        if(mCountdownValue > sf::seconds(0))
        {
            mText.setString(to_string(static_cast<int>(mCountdownValue.asSeconds())));
        }
        else
        {
            this->initInGameState();
        }
    }
}

void Game::updateInGame()
{
    mTimeSinceLastUpdate += mGameClock.restart();

    while(mTimeSinceLastUpdate >= mCurrentSettings.getUpdateInterval())
    {
        mTimeSinceLastUpdate -= mCurrentSettings.getUpdateInterval();

        SnakeStepResult<unsigned short> snakeStepResult = mSnake.move(mFreeCells, mFoodPosition, mChosenDirection);

        if(snakeStepResult.collisionOccurred)
        {
            this->initLossState();
        }
        else
        {
            if(snakeStepResult.foodEaten)
            {
                if(mFreeCells.getFreeCount() == 0)
                {
                    this->initWinState();
                }
                else
                {
                    this->spawnFood();
                    this->setCellColor(mFoodPosition, mCurrentSettings.getFoodColor());
                }
            }
            else
            {
                this->setCellColor(snakeStepResult.oldTail, mCurrentSettings.getCellColor());
            }

            this->setCellColor(snakeStepResult.newHead, mCurrentSettings.getSnakeColor());
        }
    }
}

void Game::updateEndGame()
{
    if(mTimeSinceLastUpdate += mGameClock.restart(); mTimeSinceLastUpdate > mCountdownValue)
    {
        this->close();
    }
}

void Game::render()
{
    this->windowClear();
    this->windowDraw();
    this->windowDisplay();
}

void Game::windowClear()
{
    mRenderWindow.clear(mCurrentSettings.getBackgroundColor());
}

void Game::windowDraw()
{
    mRenderWindow.setView(mView);
    mRenderWindow.draw(mVertexBuffer, mCurrentSettings.getFirstIndexOfGridBuffer(), mCurrentSettings.getGridVertexBufferSize());
    mRenderWindow.draw(mVertexBuffer, mCurrentSettings.getFirstIndexOfCellBuffer(), mCurrentSettings.getCellVertexBufferSize());

    switch(mGameState)
    {
        case GameState::Countdown:
        case GameState::Win:
        case GameState::Loss:
        {
            mRenderWindow.setView(mTextView);
            mRenderWindow.draw(mText);

            break;
        }

        default: break;
    }
}

void Game::windowDisplay()
{
    mRenderWindow.display();
}


void Game::advanceViewport(sf::Vector2u const &size, sf::View &view, sf::Vector2f const &originSize)
{
    if(size.x > 0 && size.y > 0)
    {
        float const &&windowRatio = static_cast<float>(size.x) / static_cast<float>(size.y);
        float const &&worldSizeRatio = originSize.x / originSize.y;
        sf::FloatRect viewport = {};

        if(windowRatio > worldSizeRatio)
        {
            float const &&viewWidth = worldSizeRatio / windowRatio;

            viewport.position = {(1.f - viewWidth) / 2.f, 0.f};
            viewport.size = {viewWidth, 1.f};
        }
        else
        {
            float const &&viewHeight = windowRatio / worldSizeRatio;

            viewport.position = {0.f, (1.f - viewHeight) / 2.f };
            viewport.size = {1.f, viewHeight };
        }

        view.setViewport(viewport);
    }
}

void Game::updateControls(sf::Event::KeyPressed const &keyPressedEvent)
{
    if(mChosenDirection == Direction::None || isTheSameAxis(mSnake.getDirection(), mChosenDirection))
    {
        unordered_map<sf::Keyboard::Key, Direction>::const_iterator const cIt = mKeyboardControls.find(keyPressedEvent.code);
        if(cIt != mKeyboardControls.cend())
        {
            mChosenDirection = cIt->second;
        }
    }
}

void Game::initGrid()
{
    sf::FloatRect const worldBounds = mCurrentSettings.getWorldBounds();
    sf::Color const gridColor = mCurrentSettings.getGridColor();
    unsigned short const lineThickness = mCurrentSettings.getLineThickness();
    unsigned short const step = mCurrentSettings.getCellSize() + mCurrentSettings.getLineThickness();
    size_t vertexIndex = 0;
    vector<sf::Vertex> mGridVertices(mCurrentSettings.getGridVertexBufferSize());

    for(unsigned short i = 0; i <= mCurrentSettings.getCellsPerSideCount(); ++i)
    {
        float const coord = static_cast<float>(i * step);

        //p0-p1
        //| / |
        //p2-p3

        //4 vertices for a vertical line
        sf::Vector2f p0 = { //top left
                            coord, static_cast<float>(worldBounds.position.y)};
        sf::Vector2f p1 = { //top right
                            coord + lineThickness, static_cast<float>(worldBounds.position.y)};
        sf::Vector2f p2 = { //bottom left
                            coord, static_cast<float>(worldBounds.position.y + worldBounds.size.y)};
        sf::Vector2f p3 = { //bottom right
                            coord + lineThickness, static_cast<float>(worldBounds.position.y + worldBounds.size.y)};

        //triangle 1 (p0, p1, p2)
        mGridVertices[vertexIndex].position = p0;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p1;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p2;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        //triangle 2 (p1, p3, p2)
        mGridVertices[vertexIndex].position = p1;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p3;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p2;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;

        //4 vertices for a horizontal line
        p0 = sf::Vector2f{  //top left
            static_cast<float>(worldBounds.position.x), coord};
        p1 = sf::Vector2f{  //top right
            static_cast<float>(worldBounds.position.x + worldBounds.size.x), coord};
        p2 = sf::Vector2f{  //bottom left
            static_cast<float>(worldBounds.position.x), coord + lineThickness};
        p3 = sf::Vector2f{  //bottom right
            static_cast<float>(worldBounds.position.x + worldBounds.size.x), coord + lineThickness};

        //triangle 1 (p0, p1, p2)
        mGridVertices[vertexIndex].position = p0;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p1;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p2;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        //triangle 2 (p1, p3, p2)
        mGridVertices[vertexIndex].position = p1;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p3;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
        mGridVertices[vertexIndex].position = p2;
        mGridVertices[vertexIndex].color = gridColor;
        ++vertexIndex;
    }

    if(!mVertexBuffer.update(mGridVertices.data(), mCurrentSettings.getGridVertexBufferSize(), mCurrentSettings.getFirstIndexOfGridBuffer()))
    {
        throw runtime_error(mCurrentSettings.vertexBufferUpdateError);
    }
}

void Game::initCells()
{
    sf::Color const cellColor = mCurrentSettings.getCellColor();
    unsigned short const lineThickness = mCurrentSettings.getLineThickness();
    unsigned short const cellsPerSideCount = mCurrentSettings.getCellsPerSideCount();
    unsigned short const cellSize = mCurrentSettings.getCellSize();
    unsigned short const step = cellSize + lineThickness;
    size_t vertexIndex = 0;
    vector<sf::Vertex> snakeVertices(mCurrentSettings.getCellVertexBufferSize());

    for(unsigned short y = 0; y < cellsPerSideCount; ++y)
    {
        for(unsigned short x = 0; x < cellsPerSideCount; ++x)
        {
            float const left = static_cast<float>(x * step + lineThickness);
            float const top = static_cast<float>(y * step + lineThickness);
            float const right = left + static_cast<float>(cellSize);
            float const bottom = top + static_cast<float>(cellSize);

            //p0-p1
            //| / |
            //p2-p3

            sf::Vector2f const p0 = {left, top};       //top left
            sf::Vector2f const p1 = {right, top};      //top right
            sf::Vector2f const p2 = {left, bottom};    //bottom left
            sf::Vector2f const p3 = {right, bottom};   //bottom right

            //triangle 1 (p0, p1, p2)
            snakeVertices[vertexIndex].position = p0;
            snakeVertices[vertexIndex].color = cellColor;
            ++vertexIndex;
            snakeVertices[vertexIndex].position = p1;
            snakeVertices[vertexIndex].color = cellColor;
            ++vertexIndex;
            snakeVertices[vertexIndex].position = p2;
            snakeVertices[vertexIndex].color = cellColor;
            ++vertexIndex;

            //triangle 2 (p1, p3, p2)
            snakeVertices[vertexIndex].position = p1;
            snakeVertices[vertexIndex].color = cellColor;
            ++vertexIndex;
            snakeVertices[vertexIndex].position = p3;
            snakeVertices[vertexIndex].color = cellColor;
            ++vertexIndex;
            snakeVertices[vertexIndex].position = p2;
            snakeVertices[vertexIndex].color = cellColor;
            ++vertexIndex;
        }
    }

    if(!mVertexBuffer.update(snakeVertices.data(), mCurrentSettings.getCellVertexBufferSize(), mCurrentSettings.getFirstIndexOfCellBuffer()))
    {
        throw runtime_error(mCurrentSettings.vertexBufferUpdateError);
    }
}

void Game::spawnFood()
{
    this->randomizeFoodPosition();
    this->setCellColor(mFoodPosition, mCurrentSettings.getFoodColor());
}

void Game::randomizeFoodPosition()
{
    uniform_int_distribution<unsigned short> dist{0, static_cast<unsigned short>(mFreeCells.getFreeCount() - 1)};
    unsigned short const randomIndex = dist(mRng);

    mFoodPosition = mFreeCells.at(randomIndex);
}

void Game::setCellColor(unsigned short const cellIndex, sf::Color const color)
{
    unsigned short const cellsPerSideCount = mCurrentSettings.getCellsPerSideCount();
    unsigned short const cellSize = mCurrentSettings.getCellSize();
    unsigned short const lineThickness = mCurrentSettings.getLineThickness();
    unsigned short const x = cellIndex % cellsPerSideCount;
    unsigned short const y = cellIndex / cellsPerSideCount;
    unsigned short const step = cellSize + lineThickness;

    float const left = static_cast<float>(x * step + lineThickness);
    float const top = static_cast<float>(y * step + lineThickness);
    float const right = left + static_cast<float>(cellSize);
    float const bottom = top + static_cast<float>(cellSize);

    vector<sf::Vertex> vertices(mCurrentSettings.rectVertexCount);
    vertices[0] = sf::Vertex{{left, top}, color};
    vertices[1] = sf::Vertex{{right, top}, color};
    vertices[2] = sf::Vertex{{left, bottom}, color};
    vertices[3] = sf::Vertex{{right, top}, color};
    vertices[4] = sf::Vertex{{right, bottom}, color};
    vertices[5] = sf::Vertex{{left, bottom}, color};

    if(!mVertexBuffer.update(vertices.data(), mCurrentSettings.rectVertexCount, mCurrentSettings.getFirstIndexOfCellBuffer() + cellIndex * mCurrentSettings.rectVertexCount))
    {
        throw runtime_error(mCurrentSettings.vertexBufferUpdateError);
    }
}