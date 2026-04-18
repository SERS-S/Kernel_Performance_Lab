#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace kpl
{
    class RunService
    {
        public:
            explicit RunService(std::filesystem::path source_path);

            std::string start_run(const std::filesystem::path &source_path);
            std::string get_run(const std::string &run_id) const;
            std::string list_runs() const;
            std::string compare_runs(const std::string &left_run_id, const std::string &right_run_id) const;

        private:
            std::filesystem::path source_path_;
    };
}