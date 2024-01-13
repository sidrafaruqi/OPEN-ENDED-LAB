#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include "headerfile1.h"

int main() {
    CURL *curl;
    CURLcode res;
    FILE *fp;

    fp = fopen("api_response.json", "w");

    if (fp == NULL) {
        fprintf(stderr, "Error opening file for writing.\n");
        return 1;
    }

    curl_global_init(CURL_GLOBAL_DEFAULT);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/forecast?lat=24.8606&lon=67.0104&appid=064c667df3a9de95ee0ebeb1bc8dc23f&units=metric");

        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK)
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));

        curl_easy_cleanup(curl);
        fclose(fp);
    }

    curl_global_cleanup();

    retrieve_values();
    report_generating();

    return 0;
}

