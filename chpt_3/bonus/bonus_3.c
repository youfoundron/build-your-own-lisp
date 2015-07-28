// Declare a function that outputs Hello World! n number of times. Call this from main.
#include <stdio.h>

void puts_n_times(int n) {
  for (int i = 0; i < n; i++) {
    puts("Hello World!");
  }
  return;
}

int main(int argc, char** argv) {
  puts_n_times(5);
  return 0;
}
