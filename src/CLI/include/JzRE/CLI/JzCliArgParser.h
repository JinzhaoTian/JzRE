/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "JzRE/Runtime/Core/JzRETypes.h"

namespace JzRE {

/**
 * @brief Parsed command line token representation.
 */
struct JzCliParsedArgs {
    std::unordered_map<String, std::vector<String>> options;
    std::vector<String>                              positionals;

    /**
     * @brief Check whether an option key exists.
     *
     * @param key Option key (e.g. "--project").
     *
     * @return Bool True when present.
     */
    [[nodiscard]] Bool HasOption(const String &key) const;

    /**
     * @brief Get option values by key.
     *
     * @param key Option key.
     *
     * @return const std::vector<String>* Pointer to values or nullptr.
     */
    [[nodiscard]] const std::vector<String> *GetOptionValues(const String &key) const;

    /**
     * @brief Get the first option value by key.
     *
     * @param key Option key.
     *
     * @return const String* Pointer to first value or nullptr.
     */
    [[nodiscard]] const String *GetFirstValue(const String &key) const;
};

/**
 * @brief Minimal argument parser for `--key value` style CLI arguments.
 */
class JzCliArgParser {
public:
    /**
     * @brief Parse a token list.
     *
     * @param tokens Tokens excluding executable path.
     * @param flagOptions Option keys that should be treated as value-less flags.
     *
     * @return JzCliParsedArgs Parsed result.
     */
    static JzCliParsedArgs Parse(const std::vector<String>       &tokens,
                                 const std::unordered_set<String> &flagOptions = {});
};

} // namespace JzRE
