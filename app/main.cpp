#include <iostream>
#include <stdexcept>
#include <filesystem>
#include "run_service.hpp"

std::filesystem::path resolve_path(const char *path)
{
    if (path == nullptr || path[0] == '\0')
    {
        throw std::invalid_argument("Path is incorrect");
    }

    const std::filesystem::path source_path = std::filesystem::absolute(path);
    
    if (!std::filesystem::exists(source_path))
    {
        throw std::invalid_argument("Path does not exist");
    }

    if (!std::filesystem::is_regular_file(source_path))
    {
        throw std::invalid_argument("Path is not a regular file");
    }

    const auto ext = source_path.extension().string();
    if (ext != ".c" && ext != ".cpp" && ext != ".cc" && ext != ".cxx")
    {
        throw std::invalid_argument("Path is not a C/C++ source file");
    }
    
    return source_path;
}

int main(int argc, char** argv)
{
    std::cout << "### Kernel Performance Lab started ###" << std::endl;

    try
    {
        if (argc < 2)
        {
            throw std::invalid_argument("Usage: kpl <path-to-c-or-cpp-file>");
        }

        const char* path = argv[1];
        const auto source_path = resolve_path(path);
        std::cout << "Source file: " << source_path << std::endl;

        kpl::RunService service(std::filesystem::current_path());

        auto run_id = service.start_run(source_path);

        std::cout << "Run started: " << run_id << std::endl;

        auto result = service.get_run(run_id);

        std::cout << "Run result:\n" << result << std::endl;



    }
    catch (const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}
