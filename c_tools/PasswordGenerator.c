#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PASSWORD_LENGTH 12 // You can change the length of the password here
#define ASCII_MIN 33       // Start of printable ASCII characters ('!')
#define ASCII_MAX 126      // End of printable ASCII characters ('~')

void generatePassword(int length) {
  char password[length + 1]; // +1 for the null terminator
  srand(time(NULL));         // Initialize random number generator

  for (int i = 0; i < length; i++) {
    // Generate a random ASCII character in the printable range
    password[i] = (rand() % (ASCII_MAX - ASCII_MIN + 1)) + ASCII_MIN;
  }

  password[length] = '\0'; // Null terminator for the string

  printf("Generated Password: %s\n", password);
}

int main(int argc, char *argv[]) {
  generatePassword(atoi(argv[1]));
  return 0;
}
