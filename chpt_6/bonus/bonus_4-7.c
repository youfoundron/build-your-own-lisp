// › Change the grammar to add a new operator such as %.
// › Change the grammar to recognise operators written in textual format add, sub, mul, div.
// › Change the grammar to recognize decimal numbers such as 0.01, 5.21, or 10.2.

// To compile on mac & linux
// On Linux you will also have to link to the maths library: -lm
// cc -std=c99 -Wall <input_filename.c> mpc.c -ledit -lm -o <output_filename>

// System header so reference with <, >
#include <stdio.h>
#include <stdlib.h>
#include <editline/readline.h>

// Local header so reference with quotes
#include "mpc.h"

#ifdef _WIN32

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

// System header so reference with <, >
#elif __APPLE__
#include <editline/readline.h>
#elif __linux
#include <editline/history.h>
#endif

int main(int argc, char** argv) {

  /* Create Some Parsers */
  mpc_parser_t* Number   = mpc_new("number");
  mpc_parser_t* Operator = mpc_new("operator");
  mpc_parser_t* Expr     = mpc_new("expr");
  mpc_parser_t* Nilisp   = mpc_new("nilisp");

  /* Define them with the following Language */
  mpca_lang(MPCA_LANG_DEFAULT,
    "                                                    \
      number   : /-?[0-9]+(\\.[0-9]+)?/;                 \
      operator : '+' | '-' | '*' | '/' | '%' |           \
                \"add\" | \"sub\" | \"mul\" | \"div\";   \
      expr     : <number> | '(' <operator> <expr>+ ')' ; \
      nilisp   : /^/ <operator> <expr>+ /$/ ;            \
    ",
    Number, Operator, Expr, Nilisp);

  /* Print Version and Exit Information */
  puts("Nihilisp Version 0.0.0.0.1");
  puts("Created by @youfoundron");
  puts("Press Ctrl+c to Exit\n");

  while (1) {                           /* In a never ending loop */
    char* input = readline("nilisp> "); /* Output our prompt and get input */
    add_history(input);                 /* Add input to history */

    /* Atempt to Parse the user Input */
    mpc_result_t r;
    if (mpc_parse("<stdin>", input, Nilisp, &r)) {
      /* On Success Print the AST */
      mpc_ast_print(r.output);
      mpc_ast_delete(r.output);
    } else {
      /* Otherwise Print the Error */
      mpc_err_print(r.error);
      mpc_err_delete(r.error);
    }

    free(input);                        /* Free retrieved input */
  }

  /* Undefine and Delete our Parsers */
  mpc_cleanup(4, Number, Operator, Expr, Nilisp);
  return 0;
}
