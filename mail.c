#include "email.h"
#include <stdio.h>

#define DEFAULT_TO_EMAIL "sidrafaruqi17@gmail.com"

int main(void) {

  sendEmail(DEFAULT_TO_EMAIL, "Test Subject", "This is the body of the email.", "/home/sidra/Desktop/output.txt");

  return 0;
}




