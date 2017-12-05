#include <stdio.h>
#include <stdlib.h>

#include "mpc.h"

#ifdef _WIN32
#include <string.h>

static char buffer[2048];

char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';

  return cpy;
}

void add_history(char* unused) {}

#else
#include <editline/readline.h>
#endif

int main(void) {
  mpc_parser_t* Noun = mpc_new("noun");
  mpc_parser_t* Adj = mpc_new("adj");
  mpc_parser_t* Phrase = mpc_new("phrase");
  mpc_parser_t* Doge = mpc_new("doge");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                                 \
      noun: \"lisp\" | \"language\" | \"c\" | \"book\" | \"build\" ;                    \
      adj: \"wow\" | \"many\" | \"so\" | \"such\" ;                                     \
      phrase: <adj> <noun> ;                                                            \
      doge: /^/ <phrase>* /$/ ;                                                         \
      ",
      Noun, Adj, Phrase, Doge);

  puts("Can has Doge conversation Version: .0.0.1");
  puts("Press CTRL+c to Exit\n");

  while(1) {
    char* input = readline("doge> ");
    add_history(input);
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Doge, &r)) {
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }

  mpc_cleanup(4, Noun, Adj, Phrase, Doge);

  return EXIT_SUCCESS;
}
