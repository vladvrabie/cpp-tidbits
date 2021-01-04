#include "base.h"

#include <utility>


const BaseCopyMoveLogger::_Messages BaseCopyMoveLogger::m_defaultMessages = BaseCopyMoveLogger::_Messages{};

BaseCopyMoveLogger::BaseCopyMoveLogger(std::ostream& out, bool isEnabled) noexcept
    : m_out{ &out }
    , m_isEnabled{ isEnabled }
{
}

BaseCopyMoveLogger::BaseCopyMoveLogger(const BaseCopyMoveLogger& other) noexcept
    : m_out{ other.m_out }
    , m_isEnabled{ other.m_isEnabled }
{
    if (m_isEnabled)
    {
        *m_out << GetCopyConstructorMsg();
    }
}

BaseCopyMoveLogger::BaseCopyMoveLogger(BaseCopyMoveLogger&& other) noexcept
    : m_out{ std::exchange(other.m_out, nullptr) }
    , m_isEnabled{ std::exchange(other.m_isEnabled, false) }
{
    if (m_isEnabled)
    {
        *m_out << GetMoveConstructorMsg();
    }
}

BaseCopyMoveLogger& BaseCopyMoveLogger::operator=(const BaseCopyMoveLogger& other) noexcept
{
    if (this != &other)
    {
        m_out = other.m_out;
        m_isEnabled = other.m_isEnabled;
    }
    if (m_isEnabled)
    {
        *m_out << GetCopyAssignMsg();
    }
    return *this;
}

BaseCopyMoveLogger& BaseCopyMoveLogger::operator=(BaseCopyMoveLogger&& other) noexcept
{
    if (this != &other)
    {
        m_out = std::exchange(other.m_out, nullptr);
        m_isEnabled = std::exchange(other.m_isEnabled, false);
    }
    if (m_isEnabled)
    {
        *m_out << GetMoveAssignMsg();
    }
    return *this;
}

void BaseCopyMoveLogger::enable() noexcept
{
    m_isEnabled = true;
}

void BaseCopyMoveLogger::disable() noexcept
{
    m_isEnabled = false;
}

bool BaseCopyMoveLogger::isEnabled() const noexcept
{
    return m_isEnabled;
}

const std::string& BaseCopyMoveLogger::GetCopyConstructorMsg() const noexcept
{
    return m_defaultMessages.copyConstructorMsg;
}

const std::string& BaseCopyMoveLogger::GetMoveConstructorMsg() const noexcept
{
    return m_defaultMessages.moveConstructorMsg;
}

const std::string& BaseCopyMoveLogger::GetCopyAssignMsg() const noexcept
{
    return m_defaultMessages.copyAssignMsg;
}

const std::string& BaseCopyMoveLogger::GetMoveAssignMsg() const noexcept
{
    return m_defaultMessages.moveAssignMsg;
}
