#pragma once

#include <string>
#include <memory>

class User {
public:
    explicit User(std::string nickname) : nickname_(std::move(nickname)) {}

    const std::string& nickname() const { return nickname_; }

private:
    std::string nickname_;
};

using UserPtr = std::shared_ptr<User>;
using UserWeakPtr = std::weak_ptr<User>;