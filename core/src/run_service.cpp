#include "../include/run_service.hpp"
#include <chrono>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

namespace kpl {
static std::string generate_id() {
  auto now = std::chrono::system_clock::now().time_since_epoch().count();
  return std::to_string(now);
}

RunService::RunService(fs::path source_path)
    : source_path_(std::move(source_path)) {}

std::string RunService::start_run(const fs::path &source_path) {
  auto run_id = generate_id();

  auto run_dir = source_path_ / "runs" / run_id;
  fs::create_directories(run_dir);

  std::ofstream out(run_dir / "result.json");

  out << R"({
        "run_id": ")"
      << run_id << R"(",
        "status": "success",
        "source": ")"
      << source_path.string() << R"("
        })";

  out.close();

  return run_id;
}

std::string RunService::get_run(const std::string &run_id) const {
  auto path = source_path_ / "runs" / run_id / "result.json";

  if (!fs::exists(path))
    return R"({"error":"not_found"})";

  std::ifstream in(path);
  return std::string(std::istreambuf_iterator<char>(in),
                     std::istreambuf_iterator<char>());
}

std::string RunService::list_runs() const {
  const auto runs_dir = source_path_ / "runs";
  if (!fs::exists(runs_dir)) {
    fs::create_directories(runs_dir);
  }

  std::string json = "[";

  bool first = true;

  for (auto &entry : fs::directory_iterator(runs_dir)) {
    if (!entry.is_directory())
      continue;

    if (!first)
      json += ",";

    json += "\"" + entry.path().filename().string() + "\"";
    first = false;
  }

  json += "]";
  return json;
}

std::string RunService::compare_runs(const std::string &,
                                     const std::string &) const {
  return R"({"status":"not_implemented"})";
}
} // namespace kpl
