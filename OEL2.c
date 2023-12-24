#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>

size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t realsize = size * nmemb;
    FILE *fp = (FILE *)userp;

    if (fp == NULL) {
        fprintf(stderr, "Error: File pointer is NULL.\n");
        return 0;
    }

    return fwrite(contents, size, nmemb, fp);
}

int retrieve_values() {
    FILE *fp = fopen("api_response.json","r");
    if (fp==NULL){
	fprintf(stderr, "Error: Unable to open the file./n");
	return 1;
	}
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    
    char content[length];
    int file_content = fread(content, 1, sizeof(content), fp);
    fclose(fp);
    
    cJSON *json = cJSON_Parse(content); 
    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        return 1; 
    } 
    
    cJSON *list = cJSON_GetObjectItemCaseSensitive(json, "list"); 
    if (cJSON_IsArray(list) && (list != NULL)) { 
        cJSON *first_dict = cJSON_GetArrayItem(list, 0);
        if (first_dict != NULL && cJSON_IsObject(first_dict)) {
            cJSON* dt_txt = cJSON_GetObjectItem(first_dict, "dt_txt");
             if (dt_txt != NULL && cJSON_IsString(dt_txt)) {
             	printf("DATE AND TIME %s\n", dt_txt->valuestring);
             }
    } 
  }
  
    cJSON_Delete(json);
    return 0;
 
}
 
	 
 

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
        curl_easy_setopt(curl, CURLOPT_URL, "https://api.openweathermap.org/data/2.5/forecast?lat=24.8606&lon=67.0104&appid=064c667df3a9de95ee0ebeb1bc8dc23f");

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

    return 0;
}
