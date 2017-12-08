#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"
#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdin);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  cpy[strlen(cpy)-1] = '\0';

  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

int main(void) {
  mpc_parser_t* Ab = mpc_new("ab");
  mpc_parser_t* Abl = mpc_new("abl");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                       \
      ab: 'a'|'b' ;                                           \
      abl: /^/ <ab>+ /$/ ;                                    \
      ",
      Ab, Abl);

  puts("Abl Version 0.0.1");
  puts ("Press CTRL+c to Exit\n");

  while(1) {
    char* input = readline("abl> ");
    add_history(input);
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Abl, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }


  mpc_cleanup(2, Ab, Abl);
  return EXIT_SUCCESS;
}

