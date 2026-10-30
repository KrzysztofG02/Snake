#ifndef DIRECTION_H
#define DIRECTION_H

enum class Direction: char
{
    None, Up, Right, Down, Left
};

constexpr bool isTheSameAxis(Direction const direction1, Direction const direction2)
{
    return (   (direction1 == Direction::Left || direction1 == Direction::Right)
            && (direction2 == Direction::Left || direction2 == Direction::Right)
           )
           ||
           (   (direction1 == Direction::Up || direction1 == Direction::Down)
            && (direction2 == Direction::Up || direction2 == Direction::Down));
}

#endif // DIRECTION_H
