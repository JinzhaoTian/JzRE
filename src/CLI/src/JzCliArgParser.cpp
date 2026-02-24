/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include "JzRE/CLI/JzCliArgParser.h"

namespace JzRE {

Bool JzCliParsedArgs::HasOption(const String &key) const
{
    return options.find(key) != options.end();
}

const std::vector<String> *JzCliParsedArgs::GetOptionValues(const String &key) const
{
    auto iter = options.find(key);
    if (iter == options.end()) {
        return nullptr;
    }
    return &iter->second;
}

const String *JzCliParsedArgs::GetFirstValue(const String &key) const
{
    const auto *values = GetOptionValues(key);
    if (values == nullptr || values->empty()) {
        return nullptr;
    }
    return &values->front();
}

JzCliParsedArgs JzCliArgParser::Parse(const std::vector<String>        &tokens,
                                      const std::unordered_set<String> &flagOptions)
{
    JzCliParsedArgs parsed;

    for (Size i = 0; i < tokens.size(); ++i) {
        const auto &token = tokens[i];

        if (token.rfind("--", 0) == 0) {
            auto equalPos = token.find('=');
            if (equalPos != String::npos) {
                String key   = token.substr(0, equalPos);
                String value = token.substr(equalPos + 1);
                parsed.options[key].push_back(value);
                continue;
            }

            if (flagOptions.find(token) != flagOptions.end()) {
                parsed.options[token];
                continue;
            }

            std::vector<String> values;
            for (Size j = i + 1; j < tokens.size(); ++j) {
                if (tokens[j].rfind("-", 0) == 0) {
                    break;
                }
                values.push_back(tokens[j]);
                i = j;
            }

            parsed.options[token].insert(parsed.options[token].end(), values.begin(), values.end());
            continue;
        }

        if (token.rfind("-", 0) == 0) {
            parsed.options[token];
            continue;
        }

        parsed.positionals.push_back(token);
    }

    return parsed;
}

} // namespace JzRE
