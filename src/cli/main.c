#include "yuji/core/runner.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

YujiRunner* g_yuji_runner;

int run_file(const char* filename) {
  yuji_runner_run_file(g_yuji_runner, filename);
  return 0;
}

int run_repl() {
  printf("Welcome to Yuji REPL! (version %s)\n", YUJI_VERSION_STRING);
  printf("ctrl+D to exit\n");

  bool running = true;

  while (running) {
    char buffer[64];
    printf("> ");

    if (!fgets(buffer, sizeof(buffer), stdin)) {
      printf("\n");
      break;
    }

    yuji_runner_run_string(g_yuji_runner, buffer);
  }

  return 0;
}

void ctrl_c_handler(int signal) {
  if (g_yuji_runner) {
    yuji_print_call_stack(g_yuji_runner);
    yuji_runner_free(g_yuji_runner);
  }

  exit(signal);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, ctrl_c_handler);

  g_yuji_runner = yuji_runner_init();
  int exit_code = 1;

  if (argc == 1) {
    exit_code = run_repl();
  } else if (argc == 2) {
    exit_code = run_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
  }

  yuji_runner_free(g_yuji_runner);
  return exit_code;
}
