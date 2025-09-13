#pragma once

#include <unordered_map>
#include <memory>
#include <string>
#include <stack>
#include "Stella/Absyn.H"

namespace Stella {
class Context {
public:
    Context() = default;
    Context(const Context&) = delete;

    Context(Context&&) = delete;

    std::shared_ptr<Type> lookup(const std::string&) const;
    void bindNew(const std::string&, std::shared_ptr<Type>);
    void addScope();
    void popScope();
    std::unordered_map<std::string, std::shared_ptr<Type>>& getTop();

private:
    std::stack<std::unordered_map<std::string, std::shared_ptr<Type>>> scopes_;
};
}