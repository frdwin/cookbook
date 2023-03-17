/*
 * Recipe to get user input
 */

#include <stdio.h>

int main(void) {

  // Get a few numbers or words as input
  int number;
  printf("Choose a number: ");
  scanf("%d", &number);
  printf("You chose: %d\n", number);

  char letter;
  printf("Choose a number and a letter (format: number <space> letter): ");
  scanf("%d %c", &number, &letter);
  printf("You chose: %d and %c\n", number, letter);

  /* Get multiple words.
   * Need to clear the buffer because scanf
   * does not consume the new line (\n)
   */
  getchar();
  char fullName[20];
  printf("Type your full name (20 char max): ");
  fgets(fullName, sizeof(fullName), stdin);
  printf("Your name is %s", fullName);

  return 0;
}
