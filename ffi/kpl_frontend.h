extern "C" {

typedef void *KplContext;
struct KplBackendApi {
  KplContext ctx;

  int (*run_start)(KplContext ctx, const char *scenario_name);

  char *(*run_list)(KplContext ctx);

  void (*string_free)(KplContext ctx, char *str_ptr);
};
int kpl_frontend_run(const KplBackendApi *api);
}