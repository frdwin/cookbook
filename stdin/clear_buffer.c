/*
 * Clear the buffer recipe
 */

#include <stdio.h>

void clearBuff(void);

int main(void) {
  char c;
  printf("Type a char: ");
  scanf("%c", &c);

  clearBuff();

  return 0;
}

void clearBuff(void) {
  char c;
  int n = 1;
  do {
    c = getchar();
    printf("Cleared the buffer %d time(s)\n", n);
  } while (c != '\n' && c != EOF);
}
