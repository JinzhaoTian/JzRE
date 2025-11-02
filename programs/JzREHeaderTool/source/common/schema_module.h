/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <string>
#include <vector>

#include "Types/JhtClass.h"

struct SchemaModule {
    std::string                            name;
    std::vector<std::shared_ptr<JhtClass>> classes;
};