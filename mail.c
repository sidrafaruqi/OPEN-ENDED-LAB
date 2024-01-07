#include "email.h"
#include <stdio.h>

#define DEFAULT_TO_EMAIL "sidrafaruqi17@gmail.com"

int main(void) {

  sendEmail(DEFAULT_TO_EMAIL, "WEEK'S WEATHER REPORT", "         -------------------------------               ", "/home/sidra/Desktop/report_generating.txt");

  return 0;
}




