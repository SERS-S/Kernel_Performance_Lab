#pragma once

#include <filesystem>
#include <string>
#include <vector>

namespace kpl
{
    enum class RunStatus
    {
        queued,
        running,
        failed,
        partial_success
    };

    struct RunArtifact
    {
        std::string name;
        std::filesystem::path path;
    };

    struct RunContext
    {
        std::string run_id;

        std::filesystem::path workspace_dir;
        std::filesystem::path source_path;
        std::filesystem::path run_dir;

        RunStatus status = RunStatus::queued;
        std::string error;

        std::string started_at;
        std::string finished_at;

        std::vector<RunArtifact> artifacts;
    };

    const char* to_string(RunStatus status);
}