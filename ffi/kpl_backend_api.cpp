#include "kpl_backend_api.h"
#include "../core/include/run_service.hpp"
#include <cstdlib>
#include <cstring>
#include <string>

extern "C" {

int c_run_start(KplContext ctx, const char *scenario_name) {
  if (!ctx || !scenario_name)
    return -1;

  auto *service = static_cast<kpl::RunService *>(ctx);

  service->start_run(std::string(scenario_name));
  return -1;
}

char *c_run_list(KplContext ctx) {
  if (!ctx)
    return nullptr;

  auto *service = static_cast<kpl::RunService *>(ctx);

  std::string json =
      service->get_run(0); // нужно будет заменить на какой-нибудь get_runs

#if defined(_WIN32) || defined(_WIN64)
  return _strdup(json.c_str());
#else
  return strdup(json.c_str());
#endif
}

void c_string_free(KplContext ctx, char *str_ptr) {
  (void)ctx;
  if (str_ptr) {
    free(str_ptr);
  }
}
}