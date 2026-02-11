#ifndef OPERATIONS_H
#define OPERATIONS_H

#include <string>
#include <variant>
#include <optional>

struct EmptyOperation {};
struct InsertOperation { bool isAfter; };
struct DeleteOperation {};
struct WriteOperation { std::string text; };

using OperationRegister = std::variant<EmptyOperation
            , InsertOperation
            , DeleteOperation
            , WriteOperation
>;
using OperationView = std::reference_wrapper<OperationRegister>;
using OptOperationView = std::optional<OperationView>;

#endif
