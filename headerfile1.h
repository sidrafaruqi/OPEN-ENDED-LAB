#ifndef HEADERFILE1_H
#define HEADERFILE1_H

#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <stdlib.h>
#include <string.h>


size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp);
int processed_data(const char* dt_txt, cJSON* temp, cJSON* temp_min, cJSON* temp_max, cJSON* humidity);
int calculated_data(const char* dt_txt, double *average_temp, double lowest_temp, double highest_temp, double *average_humidity);
int processed_file_opening(const char* newdata);
int report_generating();
int retrieve_values();

#endif  // HEADERFILE1_H

