#pragma once

#include <vector>
#include <optional>



template <typename T>
class RingBuffer
{
public:
    RingBuffer(std::size_t const capacity);

    std::optional<T> tryPushBack(T const value);
    std::optional<T> tryPopFront();
    [[nodiscard]] std::size_t getCapacity() const;
    [[nodiscard]] std::size_t getHeadIndex() const;
    [[nodiscard]] std::size_t getTailIndex() const;
    [[nodiscard]] T getHead() const;
    [[nodiscard]] T getTail() const;
    [[nodiscard]] std::size_t getLength() const;
    void clear(std::optional<std::size_t> const newCapacity);
    void resize(std::size_t const capacity);


private:
    std::vector<T> mData;
    std::size_t mHeadIdx;
    std::size_t mLength;
};



using namespace std;


template <typename T>
RingBuffer<T>::RingBuffer(std::size_t const capacity):
    mData(capacity),
    mHeadIdx{0},
    mLength{0} {}


template <typename T>
std::optional<T> RingBuffer<T>::tryPushBack(T const value)
{
    if(mData.empty())
    {
        return nullopt;
    }

    mData[(mHeadIdx + mLength) % mData.size()] = value;
    ++mLength;

    return value;
}

template <typename T>
std::optional<T> RingBuffer<T>::tryPopFront()
{
    if(mLength == 0)
    {
        return nullopt;
    }

    T const value = mData[mHeadIdx];

    mHeadIdx = (mHeadIdx + 1) % mData.size();
    --mLength;

    return value;
}

template <typename T>
std::size_t RingBuffer<T>::getCapacity() const
{
    return mData.size();
}

template <typename T>
std::size_t RingBuffer<T>::getHeadIndex() const
{
    return mHeadIdx;
}

template <typename T>
std::size_t RingBuffer<T>::getTailIndex() const
{
    return (mHeadIdx + mLength - 1) % mData.size();
}

template <typename T>
T RingBuffer<T>::getHead() const
{
    return mData[mHeadIdx];
}

template <typename T>
T RingBuffer<T>::getTail() const
{
    return mData[(mHeadIdx + mLength - 1) % mData.size()];
}

template <typename T>
std::size_t RingBuffer<T>::getLength() const
{
    return mLength;
}

template <typename T>
void RingBuffer<T>::clear(std::optional<std::size_t> const newCapacity)
{
    if(newCapacity.has_value())
    {
        mData.resize(newCapacity);
    }

    mHeadIdx = 0;
    mLength = 0;
}

template <typename T>
void RingBuffer<T>::resize(std::size_t const capacity)
{
    size_t const newLength = std::min(mLength, capacity);
    vector<T> tempBuffer = {};

    tempBuffer.reserve(newLength);

    for(size_t i = 0; i < newLength; ++i)
    {
        tempBuffer.push_back(move(mData[(mHeadIdx + i) % mData.size()]));
    }

    mData.resize(capacity);

    for(size_t i = 0; i < newLength; ++i)
    {
        mData[i] = move(tempBuffer[i]);
    }

    mHeadIdx = 0;
    mLength = newLength;
}