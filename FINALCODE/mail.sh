#!/bin/bash

# Compile the source files
gcc -o mail mail.c email.c -lcurl
./mail
