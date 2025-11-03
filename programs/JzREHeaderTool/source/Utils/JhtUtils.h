/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#pragma once

#include <filesystem>
#include <string>
#include <vector>
#include <fstream>

namespace JhtUtils {

inline std::vector<std::string> Split(std::string input, std::string pat)
{
    std::vector<std::string> ret_list;
    while (true) {
        size_t      index    = input.find(pat);
        std::string sub_list = input.substr(0, index);
        if (!sub_list.empty()) {
            ret_list.push_back(sub_list);
        }
        input.erase(0, index + pat.size());
        if (index == -1) {
            break;
        }
    }
    return ret_list;
}

inline std::string Trim(std::string &source_string, const std::string trim_chars)
{
    size_t left_pos = source_string.find_first_not_of(trim_chars);
    if (left_pos == std::string::npos) {
        source_string = std::string();
    } else {
        size_t right_pos = source_string.find_last_not_of(trim_chars);
        source_string    = source_string.substr(left_pos, right_pos - left_pos + 1);
    }
    return source_string;
}

inline std::string ToUpper(std::string &source_string)
{
    transform(source_string.begin(), source_string.end(), source_string.begin(), ::toupper);
    return source_string;
}

inline std::string Replace(std::string &source_string, std::string sub_string, const std::string new_string)
{
    std::string::size_type pos = 0;
    while ((pos = source_string.find(sub_string)) != std::string::npos) {
        source_string.replace(pos, sub_string.length(), new_string);
    }
    return source_string;
}

inline std::string Replace(std::string &source_string, char taget_char, const char new_char)
{
    std::replace(source_string.begin(), source_string.end(), taget_char, new_char);
    return source_string;
}

inline std::string GetFileName(std::string path)
{
    if (path.size() < 1) {
        return std::string();
    }
    std::vector<std::string> result = Split(path, "/");
    if (result.size() < 1) {
        return std::string();
    }
    return result[result.size() - 1];
}

inline std::string FormatQualifiedName(std::string &source_string)
{
    Replace(source_string, '<', 'L');
    Replace(source_string, ':', 'S');
    Replace(source_string, '>', 'R');
    Replace(source_string, '*', 'P');
    return source_string;
}

inline std::string GetNameWithoutContainer(std::string name)
{
    size_t left  = name.find_first_of('<') + 1;
    size_t right = name.find_last_of('>');
    if (left > 0 && right < name.size() && left < right) {
        return name.substr(left, right - left);
    } else {
        return nullptr;
    }
}

inline std::string ConvertNameToUpperCamelCase(const std::string &name, std::string pat)
{
    std::string ret_string;
    auto      &&name_spilts = Split(name, pat);
    for (auto &split_string : name_spilts) {
        split_string[0] = toupper(split_string[0]);
        ret_string.append(split_string);
    }
    return ret_string;
}

inline void ReplaceAll(std::string &resource_str, std::string sub_str, std::string new_str)
{
    std::string::size_type pos = 0;
    while ((pos = resource_str.find(sub_str)) != std::string::npos) {
        resource_str = resource_str.replace(pos, sub_str.length(), new_str);
    }
    return;
}

inline void FormatPathString(const std::string &path_string, std::string &out_string)
{
    unsigned int          ulRet             = 0;
    auto                  local_path_string = path_string;
    std::filesystem::path local_path;

    local_path = local_path_string;
    if (local_path.is_relative()) {
        local_path_string = std::filesystem::current_path().string() + "/" + local_path_string;
    }

    ReplaceAll(local_path_string, "\\", "/");
    std::vector<std::string> subString = Split(local_path_string, "/");
    std::vector<std::string> out_sub_string;
    for (auto p : subString) {
        if (p == "..") {
            out_sub_string.pop_back();
        } else if (p != ".") {
            out_sub_string.push_back(p);
        }
    }
    for (int i = 0; i < out_sub_string.size() - 1; i++) {
        out_string.append(out_sub_string[i] + "/");
    }
    out_string.append(out_sub_string[out_sub_string.size() - 1]);
    return;
}

inline std::filesystem::path MakeRelativePath(const std::filesystem::path &from, const std::filesystem::path &to)
{
    // Start at the root path and while they are the same then do nothing then when they first
    // diverge take the remainder of the two path and replace the entire from path with ".."
    // segments.
    std::string form_complete_string;
    std::string to_complete_string;

    /*remove ".." and "."*/
    FormatPathString(from.string(), form_complete_string);
    FormatPathString(to.string(), to_complete_string);

    std::filesystem::path form_complete = form_complete_string;
    std::filesystem::path to_complete   = to_complete_string;

    auto iter_from = form_complete.begin();
    auto iter_to   = to_complete.begin();

    // Loop through both
    while (iter_from != form_complete.end() && iter_to != to_complete.end() && (*iter_to) == (*iter_from)) {
        ++iter_to;
        ++iter_from;
    }

    std::filesystem::path final_path;
    while (iter_from != form_complete.end()) {
        final_path /= "..";

        ++iter_from;
    }

    while (iter_to != to_complete.end()) {
        final_path /= *iter_to;

        ++iter_to;
    }

    return final_path;
}

inline void SaveFile(const std::string &outpu_string, const std::string &output_file)
{
    std::filesystem::path out_path(output_file);

    if (!std::filesystem::exists(out_path.parent_path())) {
        std::filesystem::create_directories(out_path.parent_path());
    }
    std::fstream output_file_stream(output_file, std::ios_base::out);

    output_file_stream << outpu_string << std::endl;
    output_file_stream.flush();
    output_file_stream.close();
    return;
}

} // namespace JhtUtils