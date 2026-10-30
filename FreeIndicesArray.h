#pragma once

#include <cstddef>
#include <type_traits>
#include <vector>



template <typename T>
class FreeIndicesArray
{
static_assert(std::is_unsigned_v<T>,
              "FreeIndicesArray: Type T must be an unsigned integer!");


public:
    FreeIndicesArray(std::size_t const size);

    bool tryOccupy(T const cell);
    bool tryRelease(T const cell);

    [[nodiscard]] T at(std::size_t const index) const;
    [[nodiscard]] T getFreeCount() const;
    [[nodiscard]] std::size_t getSize() const;
    [[nodiscard]] bool isFree(T const cell) const;
    void reset();
    void resize(std::size_t const size); //Resizes the array and resets its state


private:
    std::size_t mFreeCount;
    std::vector<T> mFreeIndices;
    std::vector<T> mPos;
};



using namespace std;


template <typename T>
FreeIndicesArray<T>::FreeIndicesArray(std::size_t const size)
{
    this->resize(size);
}


template <typename T>
bool FreeIndicesArray<T>::tryOccupy(T const cell)
{
    size_t const pos = mPos[cell];

    if(pos >= mFreeCount)
    {
        return false;
    }

    --mFreeCount;

    size_t const lastIdx = mFreeCount;
    size_t const lastCell = mFreeIndices[lastIdx];

    mFreeIndices[pos] = lastCell;
    mPos[lastCell] = pos;

    mFreeIndices[lastIdx] = cell;
    mPos[cell] = lastIdx;

    return true;
}

template <typename T>
bool FreeIndicesArray<T>::tryRelease(T const cell)
{
    size_t const pos = mPos[cell];

    if(pos < mFreeCount)
    {
        return false;
    }

    size_t const firstIdx = mFreeCount;
    size_t const firstCell = mFreeIndices[firstIdx];

    mFreeIndices[pos] = firstCell;
    mPos[firstCell] = pos;

    mFreeIndices[firstIdx] = cell;
    mPos[static_cast<size_t>(cell)] = firstIdx;

    ++mFreeCount;

    return true;
}


template <typename T>
[[nodiscard]] T FreeIndicesArray<T>::at(std::size_t const index) const
{
    return mFreeIndices[index];
}

template <typename T>
[[nodiscard]] T FreeIndicesArray<T>::getFreeCount() const
{
    return mFreeCount;
}

template <typename T>
[[nodiscard]] std::size_t FreeIndicesArray<T>::getSize() const
{
    return mFreeIndices.size();
}

template <typename T>
[[nodiscard]] bool FreeIndicesArray<T>::isFree(T const cell) const
{
    return mPos[static_cast<size_t>(cell)] < mFreeCount;
}

template <typename T>
void FreeIndicesArray<T>::reset()
{
    mFreeCount = mPos.size();

    for(T i = 0; i < static_cast<T>(mFreeCount); ++i)
    {
        mFreeIndices[i] = i;
        mPos[i] = i;
    }
}

template <typename T>
void FreeIndicesArray<T>::resize(std::size_t const size)
{
    mFreeIndices.resize(size);
    mPos.resize(size);

    this->reset();
}