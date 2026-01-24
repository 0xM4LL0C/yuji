#include "yuji/core/state.h"
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>

YujiState* G_YUJI_STATE = NULL;

int run_file(const char* filename) {
  yuji_eval_file(G_YUJI_STATE, filename);
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

    yuji_eval_string(G_YUJI_STATE, buffer);
  }

  return 0;
}

void ctrl_c_handler(int signal) {
  if (G_YUJI_STATE) {
    yuji_print_call_stack(G_YUJI_STATE);
    yuji_state_free(G_YUJI_STATE);
  }

  exit(signal);
}

int main(int argc, char* argv[]) {
  signal(SIGINT, ctrl_c_handler);

  G_YUJI_STATE = yuji_state_init();
  int exit_code = 1;

  if (argc == 1) {
    exit_code = run_repl();
  } else if (argc == 2) {
    exit_code = run_file(argv[1]);
  } else {
    fprintf(stderr, "Usage: %s [filename]\n", argv[0]);
  }

  yuji_state_free(G_YUJI_STATE);
  return exit_code;
}
