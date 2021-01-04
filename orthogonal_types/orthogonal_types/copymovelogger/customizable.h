#pragma once

#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <variant>

#include "base.h"

// Class which can be set to print messages when copy or move operations happen.
// Printing functionality can be enabled/disabled.
// Custom messages can be set for each instantiation.
// Detail: an instance holds a non-owning observer to the output stream.
class CopyMoveLogger : public BaseCopyMoveLogger
{
public:
    using BaseCopyMoveLogger::BaseCopyMoveLogger;

    virtual ~CopyMoveLogger() = default;

    // virtual assign ??

    CopyMoveLogger& operator= (const CopyMoveLogger& other);
    // The moved-from state of 'other' is disabled and no stream held
    CopyMoveLogger& operator= (CopyMoveLogger&& other);

    void enableHeap() noexcept;
    void disableHeap() noexcept;
    [[nodiscard]] bool usesHeap() const noexcept;

private:
    using Messages = BaseCopyMoveLogger::Messages;
    using MessagesPtr = std::unique_ptr<Messages>;

private:
    bool m_usesDefaultMessages = true;
    bool m_usesHeap = false;
    std::variant<std::monostate, Messages, MessagesPtr> m_customMessages;
};
