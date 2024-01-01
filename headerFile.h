#ifndef HEADERFILE_H
#define HEADERFILE_H

// Function declarations
int retrieve_values();
int processed_data(const char* dt_txt, cJSON* temp, cJSON* temp_min, cJSON* temp_max, cJSON* humidity);
int calculated_data(const char* dt_txt, double *average_temp, double lowest_temp, double highest_temp, double *average_humidity);
int processed_file_opening(const char* newdata);
int report_generating();

#endif   
