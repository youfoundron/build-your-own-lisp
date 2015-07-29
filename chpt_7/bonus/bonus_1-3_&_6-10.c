// › Write a recursive function to compute the number of leaves of a tree.
// › Write a recursive function to compute the number of branches of a tree.
// › Write a recursive function to compute the most number of children spanning from one branch of a tree.

// › Add the operator %, which returns the remainder of division. For example % 10 6 is 4.
// › Add the operator ^, which raises one number to another. For example ^ 4 2 is 16.
// › Add the function min, which returns the smallest number. For example min 1 5 3 is 1.
// › Add the function max, which returns the biggest number. For example max 1 5 3 is 5.
// › Change the minus operator - so that when it receives one argument it negates it.

#include "mpc.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32 //define something for Windows (32-bit and 64-bit, this part is common)
#include <string.h>

static char buffer[2048];

/* Fake readline function */
char* readline(char* prompt) {
  fputs(prompt, stdout);
  fgets(buffer, 2048, stdin);
  char* cpy = malloc(strlen(buffer)+1);
  strcpy(cpy, buffer);
  cpy[strlen(cpy)-1] = '\0';
  return cpy;
}

/* Fake add history function */
void add_history(char* unused) {}

  #ifdef _WIN64 //define something for Windows (64-bit only)
  #endif
#elif __APPLE__ //define something for Mac OS
#include <editline/readline.h>
  #include "TargetConditionals.h"
  #if TARGET_IPHONE_SIMULATOR // iOS Simulator
  #elif TARGET_OS_IPHONE // iOS device
  #elif TARGET_OS_MAC // Other kinds of Mac OS
  #else // Unsupported platform
  #endif
#elif __linux // linux
#include <editline/readline.h>
#include <editline/history.h> // histedit.h on Arch Linux
#elif __unix // Unix - all unices not caught above
#elif __posix // POSIX
#endif

int number_of_nodes(mpc_ast_t* t) {
  if (t->children_num == 0) {  return 1; }
  if (t->children_num >= 1) {
    int total = 1;
    for (int i = 0; i < t->children_num; i++) {
      total += number_of_nodes(t->children[i]);
    }
    return total;
  }
  return 0;
}

/* User operator string to see which operation to perform */
long eval_op(long x, char* op, long y) {
  if (strcmp(op, "+") == 0) { return x + y; }
  if (strcmp(op, "-") == 0) { return x - y; }
  if (strcmp(op, "*") == 0) { return x * y; }
  if (strcmp(op, "/") == 0) { return x / y; }
  if (strcmp(op, "%") == 0) { return x % y; }
  return 0;
}

/* evaluate function */
long eval(mpc_ast_t* t) {
  /* If tagged as number return it directly. */
  if (strstr(t->tag, "number")) {
    return atoi(t->contents);
  }

  /* The operator is always second child. */
  char* op = t->children[1]->contents;

  /* We store the third child in 'x' */
  long x = eval(t->children[2]);

  /* Iterate the remaining children and combining. */
  int i = 3;
  while (strstr(t->children[i]->tag, "expr"))  {
    x = eval_op(x, op, eval(t->children[i]));
    i++;
  }
  return x;
}

/* count the leaves of a tree */
int count_leaves(mpc_ast_t* t) {
  if (t->children_num == 0) { return 1; }
  if (t->children_num >= 0) {
    int total_num_leaves = 0;
    for (int i = 0; i < t->children_num; i++) {
      total_num_leaves += count_leaves(t->children[i]);
    }
    return total_num_leaves;
  }
  return 0;
}

/* count the branches of a tree */
//  seems like the same thing as leaves?
int count_branches(mpc_ast_t* t) { return count_leaves(t) }

/* find the length of the longest branch */
// looking into this one later

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
      operator : '+' | '-' | '*' | '/' | '%' ;           \
      expr     : <number> | '(' <operator> <expr>+ ')';  \
      nilisp   : /^/ <operator> <expr>+ /$/ |            \
                 /^\\(/ <operator> <expr>+ /\\)$/;       \
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
      /* On Success Evaluate the AST and Print the response */
      long result = eval(r.output);
      long num_leaves = count_leaves(r.output);
      printf("%li\n", result);
      printf("num_leaves: %li\n", num_leaves);
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
