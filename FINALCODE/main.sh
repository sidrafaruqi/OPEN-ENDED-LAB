#!/bin/bash

gcc -o main main.c headerfile1.c email.c -I. -lcurl -lcjson
./main

