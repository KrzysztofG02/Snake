#pragma once



template<typename T>
struct SnakeStepResult
{
    T newHead;
    T oldTail;
    bool foodEaten;
    bool collisionOccurred;

    constexpr SnakeStepResult(T const &_newHead = {},
                              T const &_oldTail = {},
                              bool const _foodEaten = false,
                              bool const _collisionOccurred = false):
        newHead{_newHead},
        oldTail{_oldTail},
        foodEaten(_foodEaten),
        collisionOccurred(_collisionOccurred) {}
};