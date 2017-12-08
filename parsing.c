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

double eval(mpc_ast_t* t);
double eval_op(double x, char* op, double y);

int main(void) {
  mpc_parser_t* Number = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr = mpc_new("expr");
  mpc_parser_t* Lispy = mpc_new("lispy");

  mpca_lang(MPCA_LANG_DEFAULT,
      "                                                                                               \
      number: /-?[0-9]+(\\.[0-9]+)?/ ;                                                                \
      operator: '+' | '-' | '*' | '/' | '%' | \"add\" | \"sub\" | \"mul\" | \"div\" | \"mod\" ;       \
      expr: <number> | '(' <operator> <expr>+ ')' ;                                                   \
      lispy: /^/ <operator> <expr>+ /$/ ;                                                             \
      ",
      Number, Operator, Expr, Lispy);

  puts("Lispy Version 0.0.1");
  puts ("Press CTRL+c to Exit\n");

  while(1) {
    char* input = readline("lispy> ");
    add_history(input);
    mpc_result_t r;
    if(mpc_parse("<stdin>", input, Lispy, &r)) {
      printf("%.2f\n", eval(r.output));
      mpc_ast_delete(r.output);
    } else {
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }
    free(input);
  }


  mpc_cleanup(4, Number, Operator, Expr, Lispy);
  return EXIT_SUCCESS;
}

double eval(mpc_ast_t* t) {
  if(strstr(t->tag, "number")) {
    return atof(t->contents);
  }

  char* op = t->children[1]->contents;
  double x = eval(t->children[2]);

  size_t i = 3;
  while(strstr(t->children[i]->tag, "expr")) {
    x = eval_op(x, op, eval(t->children[i]));
    ++i;
  }
  return x;
}

double eval_op(double x, char* op, double y) {
  if(strcmp(op, "+") == 0 || strcmp(op, "add") == 0) { return x + y; }
  if(strcmp(op, "-") == 0 || strcmp(op, "sub") == 0) { return x - y; }
  if(strcmp(op, "*") == 0 || strcmp(op, "mul") == 0) { return x * y; }
  if(strcmp(op, "/") == 0 || strcmp(op, "div") == 0) { return x / y; }
  if(strcmp(op, "%") == 0 || strcmp(op, "mod") == 0) { return (int)x % (int)y; }
  return 0;
}

