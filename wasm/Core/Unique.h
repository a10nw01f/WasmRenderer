#pragma once

struct DefaultDeleter
{
    template<class T>
    void operator()(T* ptr)
    {
        delete ptr;
    }
};

template<class T, class Deleter>
class Unique
{
public:
    Unique(){}
    explicit Unique(const T& value) : m_Value(value) {}

    Unique(const Unique&) = delete;
    Unique& operator=(const Unique&) = delete;

    Unique(Unique&& other) : m_Value(other.m_Value)
    {
        other.m_Value = T{};
    }

    Unique& operator=(Unique&& other)
    {
        Reset(other.m_Value);
        other.m_Value = T{};
        return *this;
    }    

    void Reset(T value = T{})
    {
        Destroy();
        m_Value = value;
    }

    bool IsEmpty() const
    {
        return m_Value == T{};
    }

    T Get() const { return m_Value; }
private:
    void Destroy()
    {
        if(!IsEmpty())
        {
            Deleter{}(m_Value);
        }
    }

    T m_Value = T{};
};