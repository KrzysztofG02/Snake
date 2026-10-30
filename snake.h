#pragma once

#include "Direction.h"
#include "SnakeStepResult.h"
#include "FreeIndicesArray.h"
#include "RingBuffer.h"



template <typename T>
class Snake
{
static_assert(std::is_unsigned_v<T>,
              "Snake: Type T must be an unsigned integer!");


public:
    Snake(T const cellsPerSideCount, Direction const direction = Direction::None);

    Direction getDirection() const;
    std::optional<T> addSegment(T const value, FreeIndicesArray<T> &freeCells);
    void setDirection(Direction const direction);
    SnakeStepResult<T> move(FreeIndicesArray<T> &freeCells, T const foodPosition, Direction const newDirection = Direction::None);

private:
    Direction adjustDirection(Direction const newDirection);
    [[nodiscard]] T getDisplacement(Direction const direction) const;
    [[nodiscard]] bool isHittingWall() const;
    [[nodiscard]] bool isHittingTail(FreeIndicesArray<T> const &freeCells, T const potentialNewHead) const;


private:
    T mCellsPerSideCount;
    Direction mDirection;
    RingBuffer<T> mSnakeBodyQueue;
    T mLastLineIdx;
};



using namespace std;


template <typename T>
Snake<T>::Snake(T const cellsPerSideCount, Direction const direction):
    mCellsPerSideCount{cellsPerSideCount},
    mDirection{direction},
    mSnakeBodyQueue{static_cast<size_t>(mCellsPerSideCount * mCellsPerSideCount)},
    mLastLineIdx{static_cast<T>(cellsPerSideCount - 1)} {}


template <typename T>
std::optional<T> Snake<T>::addSegment(T const value, FreeIndicesArray<T> &freeCells)
{
    optional<T> const result = mSnakeBodyQueue.tryPushBack(value);

    if(result.has_value())
    {
        freeCells.tryOccupy(value);
    }

    return result;
}

template <typename T>
Direction Snake<T>::getDirection() const
{
    return mDirection;
}

template <typename T>
void Snake<T>::setDirection(Direction const direction)
{
    mDirection = direction;
}

template <typename T>
SnakeStepResult<T> Snake<T>::move(FreeIndicesArray<T> &freeCells, T const foodPosition, Direction const newDirection)
{
    SnakeStepResult<T> snakeStepResult = {};

    mDirection = this->adjustDirection(newDirection);

    snakeStepResult.collisionOccurred = this->isHittingWall();
    if(snakeStepResult.collisionOccurred)
    {
        return snakeStepResult;
    }

    T const potentialNewHead = mSnakeBodyQueue.getTail() + this->getDisplacement(mDirection);

    snakeStepResult.foodEaten = potentialNewHead == foodPosition;

    snakeStepResult.collisionOccurred = this->isHittingTail(freeCells, potentialNewHead);
    if(snakeStepResult.collisionOccurred)
    {
        return snakeStepResult;
    }

    snakeStepResult.newHead = *mSnakeBodyQueue.tryPushBack(potentialNewHead);

    if(!snakeStepResult.foodEaten)
    {
        snakeStepResult.oldTail = *mSnakeBodyQueue.tryPopFront();
        freeCells.tryRelease(snakeStepResult.oldTail);
    }

    freeCells.tryOccupy(snakeStepResult.newHead);

    return snakeStepResult;
}


template <typename T>
Direction Snake<T>::adjustDirection(Direction const newDirection)
{
    if((mDirection == Direction::Left || mDirection == Direction::Right))
    {
        if(newDirection == Direction::Up || newDirection == Direction::Down)
        {
            return newDirection;
        }
    }
    else if(mDirection == Direction::Up || mDirection == Direction::Down)
    {
        if(newDirection == Direction::Left || newDirection == Direction::Right)
        {
            return newDirection;
        }
    }

    return mDirection;
}

template <typename T>
[[nodiscard]] T Snake<T>::getDisplacement(Direction const direction) const
{
    switch(direction)
    {
        case Direction::None:  return 0;
        case Direction::Up:    return -mCellsPerSideCount;
        case Direction::Right: return 1;
        case Direction::Down:  return mCellsPerSideCount;
        case Direction::Left:  return -1;
    }

    return 0;
}

template <typename T>
bool Snake<T>::isHittingWall() const
{
    T const &&snakeHeadRowIdx = mSnakeBodyQueue.getTail() / mCellsPerSideCount;
    T const &&snakeHeadColIdx = mSnakeBodyQueue.getTail() % mCellsPerSideCount;

    return    (snakeHeadRowIdx == 0 && mDirection == Direction::Up)
           || (snakeHeadRowIdx == mLastLineIdx && mDirection == Direction::Down)
           || (snakeHeadColIdx == 0 && mDirection == Direction::Left)
           || (snakeHeadColIdx == mLastLineIdx && mDirection == Direction::Right);
}

template <typename T>
bool Snake<T>::isHittingTail(FreeIndicesArray<T> const &freeCells, T const potentialNewHead) const
{
    return !(freeCells.isFree(potentialNewHead) || potentialNewHead == mSnakeBodyQueue.getHead());
}