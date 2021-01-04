#pragma once

#include <iostream>
#include <string>

// Class which can be set to print messages when copy or move operations happen.
// Printing functionality can be enabled/disabled.
// Custom messages can be set through static member.
// Detail: an instance holds a non-owning observer to the output stream.
class BaseCopyMoveLogger
{
public:
    BaseCopyMoveLogger(std::ostream& out = std::cout, bool isEnabled = true) noexcept;

    BaseCopyMoveLogger(const BaseCopyMoveLogger& other) noexcept;
    // The moved-from state of 'other' is disabled and no stream held
    BaseCopyMoveLogger(BaseCopyMoveLogger&& other) noexcept;

    virtual ~BaseCopyMoveLogger() noexcept = default;

    BaseCopyMoveLogger& operator= (const BaseCopyMoveLogger& other) noexcept;
    // The moved-from state of 'other' is disabled and no stream held
    BaseCopyMoveLogger& operator= (BaseCopyMoveLogger&& other) noexcept;

    void enable() noexcept;
    void disable() noexcept;
    [[nodiscard]] bool isEnabled() const noexcept;

    [[nodiscard]] virtual const std::string& GetCopyConstructorMsg() const noexcept;
    [[nodiscard]] virtual const std::string& GetMoveConstructorMsg() const noexcept;
    [[nodiscard]] virtual const std::string& GetCopyAssignMsg() const noexcept;
    [[nodiscard]] virtual const std::string& GetMoveAssignMsg() const noexcept;

private:
    struct _Messages
    {
        const std::string copyConstructorMsg = "CopyC";
        const std::string moveConstructorMsg = "MoveC";
        const std::string copyAssignMsg = "CopyA";
        const std::string moveAssignMsg = "MoveA";
    };

    static const _Messages m_defaultMessages;

protected:
    using Messages = _Messages;

private:
    std::ostream* m_out = &std::cout;
    bool m_isEnabled = true;  // 4/8 bytes (1 used + padding)
};
