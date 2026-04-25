#pragma once

#ifdef __cplusplus
extern "C" {
#endif

typedef void *KplContext;

struct KplBackendApi {
  KplContext ctx;

  int (*run_start)(KplContext ctx, const char *scenario_name);

  char *(*run_list)(KplContext ctx);

  void (*string_free)(KplContext ctx, char *str_ptr);
};

int kpl_frontend_run(const struct KplBackendApi *api);
int c_run_start(KplContext ctx, const char *scenario_name);
char *c_run_list(KplContext ctx);
void c_string_free(KplContext ctx, char *str_ptr);

#ifdef __cplusplus
}
#endif