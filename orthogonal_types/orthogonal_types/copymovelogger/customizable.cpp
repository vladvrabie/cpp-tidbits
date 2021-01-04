#include "customizable.h"

#include <utility>


CopyMoveLogger& CopyMoveLogger::operator=(const CopyMoveLogger& other)
{
    //if (this != &other)
    //{
    //    m_out = other.m_out;
    //    m_isEnabled = other.m_isEnabled;
    //}
    //if (m_isEnabled)
    //{
    //    *m_out << m_defaultMessages.copyAssignMsg;
    //}
    return *this;
}

CopyMoveLogger& CopyMoveLogger::operator=(CopyMoveLogger&& other)
{
    //if (this != &other)
    //{
    //    m_out = std::exchange(other.m_out, nullptr);
    //    m_isEnabled = std::exchange(other.m_isEnabled, false);
    //}
    //if (m_isEnabled)
    //{
    //    *m_out << m_defaultMessages.moveAssignMsg;
    //}
    return *this;
}

void CopyMoveLogger::enableHeap() noexcept
{
    m_usesHeap = true;
    // TODO
}

void CopyMoveLogger::disableHeap() noexcept
{
    m_usesHeap = false;
    // TODO
}

bool CopyMoveLogger::usesHeap() const noexcept
{
    return m_usesHeap;
}
