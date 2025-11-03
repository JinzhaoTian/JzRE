/**
 * @author    Jinzhao Tian
 * @copyright Copyright (c) 2025 JzRE
 */

#include <chrono>

#include "Parsers/JhtMetaParser.h"

int main(int argc, char *argv[])
{
    auto start_time = std::chrono::system_clock::now();
    int  result     = 0;

    if (argv[1] != nullptr && argv[2] != nullptr && argv[3] != nullptr && argv[4] != nullptr && argv[5] != nullptr && argv[6] != nullptr) {
        std::cout << std::endl;
        std::cout << "Parsing meta data for target \"" << argv[5] << "\"" << std::endl;
        std::cout << "Parsing in " << argv[3] << std::endl;

        JhtMetaParser parser(argv[1],
                             argv[2],
                             argv[3],
                             argv[4],
                             argv[5]);

        int result = parser.parse();
        if (0 != result) {
            return result;
        }

        parser.generateFiles();

        parser.finish();

        auto duration_time = std::chrono::system_clock::now() - start_time;
        std::cout << "Completed in "
                  << std::chrono::duration_cast<std::chrono::milliseconds>(duration_time).count()
                  << "ms"
                  << std::endl;

        return result;
    } else {
        std::cerr << "Arguments parse error!" << std::endl
                  << "Please call the tool like this:" << std::endl
                  << "meta_parser  project_file_name  include_file_name_to_generate  project_base_directory "
                     "sys_include_directory module_name showErrors(0 or 1)"
                  << std::endl
                  << std::endl;
        return -1;
    }

    return 0;
}
