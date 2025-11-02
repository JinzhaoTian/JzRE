#pragma once

#include <string>
#include <vector>
#include "language_types/class.h"

struct SchemaMoudle {
    std::string name;

    std::vector<std::shared_ptr<Class>> classes;
};