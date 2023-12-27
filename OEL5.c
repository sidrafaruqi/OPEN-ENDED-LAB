#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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


int processed_data(const char* dt_txt, cJSON* temp, cJSON* temp_min, cJSON* temp_max, cJSON* humidity);
int calculated_data(const char* dt_txt, double *average_temp, int *lowest_temp, int *highest_temp, double *average_humidity);
int  processed_file_opening(const char* data);


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
    
    cJSON *dt_txt = NULL;
    cJSON *temp = NULL;
    cJSON *temp_min = NULL;
    cJSON *temp_max = NULL;
    cJSON *humidity = NULL;
    int total_day1 = 0, total_day2 = 0;
    double total_value1 = 0, total_value2 = 0; 
    double average_temp1, average_temp2;
    int *lowest_temp1 = malloc(sizeof(int));
    int *lowest_temp2 = malloc(sizeof(int));
    int *highest_temp1 = malloc(sizeof(int));
    int *highest_temp2 = malloc(sizeof(int));

    double total_humidity1 = 0, total_humidity2 = 0; 
    double average_humidity1, average_humidity2;
        
    time_t current_time;
    struct tm *local_time;
 
    time(&current_time);
     
    local_time = localtime(&current_time);

    int cyear = local_time->tm_year + 1900;
    int cmonth = local_time->tm_mon + 1;
    int cday = local_time->tm_mday;
    
    printf("Current Date: %04d-%02d-%02d\n", cyear, cmonth, cday);

    
    cJSON *cnt = cJSON_GetObjectItemCaseSensitive(json, "cnt"); 
    cJSON *list = cJSON_GetObjectItemCaseSensitive(json, "list"); 
    if (cJSON_IsArray(list) && (list != NULL) && cJSON_IsNumber(cnt) && (cnt != NULL)) { 
    	
    	for (int i = 0; i< cnt->valueint; i++) {
		cJSON *first_dict = cJSON_GetArrayItem(list, i);
		if (first_dict != NULL && cJSON_IsObject(first_dict)) {
		    dt_txt = cJSON_GetObjectItem(first_dict, "dt_txt");
		     }
		     cJSON* main = cJSON_GetObjectItem(first_dict, "main");
		     if (main != NULL && cJSON_IsObject(main)) {
		     	 temp = cJSON_GetObjectItem(main, "temp");
		     	 temp_min = cJSON_GetObjectItem(main, "temp_min");
		     	 temp_max = cJSON_GetObjectItem(main, "temp_max");
		     	 humidity = cJSON_GetObjectItem(main, "humidity");
		     	
	    	    }
		    
		     if (dt_txt != NULL && cJSON_IsString(dt_txt)) {
				char datetimeString[20];  
                    		strcpy(datetimeString, dt_txt->valuestring);
	       			int year, month, day, hour, minute, second;
	       			sscanf(datetimeString, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
			     	 
			     	if (cyear == year && cmonth == month && cday == day) {
              			    total_day1++;
              			    total_value1 = total_value1 + temp->valuedouble; 
              			    total_humidity1 = total_humidity1 + humidity->valuedouble;
              			    
				   if (*lowest_temp1 > cJSON_GetNumberValue(temp_min)) {
				      *lowest_temp1 = cJSON_GetNumberValue(temp_min);
				}

				   if (*highest_temp1 < cJSON_GetNumberValue(temp_max)) {
				       *highest_temp1 = cJSON_GetNumberValue(temp_max);
				}

				    	
				    processed_data(dt_txt->valuestring, temp, temp_min, temp_max, humidity); 
				    
				 }
				 else if (cyear == year && cmonth == month && cday + 1 == day) {
				    total_day2++;
				    total_value2 = total_value2 + temp->valuedouble; 
				    total_humidity2 = total_humidity2 + humidity->valuedouble;
              			    
				     if (*lowest_temp2 > cJSON_GetNumberValue(temp_min)) {
				      *lowest_temp2 = cJSON_GetNumberValue(temp_min);
				}

				   if (*highest_temp2 < cJSON_GetNumberValue(temp_max)) {
				       *highest_temp2 = cJSON_GetNumberValue(temp_max);
				}
				    	
				    processed_data(dt_txt->valuestring, temp, temp_min, temp_max, humidity); 
				    
				    }
		                 else{
		                    break; 
		                 }			     
  		     }
 	}
}
    char date1[20]; // Assuming a maximum length of 20 characters is sufficient
    if (total_day1 != 0) {
	average_temp1 = total_value1 / total_day1;  
	average_humidity1 = total_humidity1 / total_day1;
	 
	// Convert year, month, and day to string and concatenate them
        snprintf(date1, sizeof(date1), "%d-%02d-%02d", cyear, cmonth, cday);

	calculated_data(date1, &average_temp1, lowest_temp1, highest_temp1, &average_humidity1); 
    }
    
    char date2[20]; // Assuming a maximum length of 20 characters is sufficient
    if (total_day2 != 0) {
	average_temp2 = total_value2 / total_day2;  
	average_humidity2 = total_humidity2 / total_day2;
	 
	// Convert year, month, and day to string and concatenate them
        snprintf(date2, sizeof(date2), "%d-%02d-%02d", cyear, cmonth, cday+1);
	calculated_data(date2, &average_temp2, lowest_temp2, highest_temp2, &average_humidity2); 
    }
    
    free(lowest_temp1);
    free(lowest_temp2);
    free(highest_temp1);
    free(highest_temp2);
	     
    cJSON_Delete(json);
 
 }


int processed_data(const char* dt_txt, cJSON* temp, cJSON* temp_min, cJSON* temp_max, cJSON* humidity) {
    cJSON *modified = cJSON_CreateObject();
    cJSON_AddStringToObject(modified, "dt_txt", dt_txt);
    cJSON_AddNumberToObject(modified, "temp", cJSON_GetNumberValue(temp));
    cJSON_AddNumberToObject(modified, "temp_min", cJSON_GetNumberValue(temp_min));
    cJSON_AddNumberToObject(modified, "temp_max", cJSON_GetNumberValue(temp_max));
    cJSON_AddNumberToObject(modified, "humidity", cJSON_GetNumberValue(humidity));

    
    char* modified_str = cJSON_Print(modified);
    
    processed_file_opening(modified_str);
    
    cJSON_Delete(modified);
    return 0;
    }
    
    
int calculated_data(const char* dt_txt, double *average_temp, int *lowest_temp, int *highest_temp, double *average_humidity) {
    cJSON *calculated = cJSON_CreateObject();
    cJSON_AddStringToObject(calculated, "dt_txt", dt_txt);
    cJSON_AddNumberToObject(calculated, "average_temp", *average_temp);
    cJSON_AddNumberToObject(calculated, "average_humidity", *average_humidity);

    // Convert int values to cJSON objects using cJSON_CreateInt
    cJSON_AddNumberToObject(calculated, "lowest_temp", (double)(*lowest_temp));
    cJSON_AddNumberToObject(calculated, "highest_temp", (double)(*highest_temp));




    
    char* calculated_str = cJSON_Print(calculated);
    
    processed_file_opening(calculated_str);
    
    cJSON_Delete(calculated);
    return 0;
    }
    
int processed_file_opening(const char* data) {
    FILE *file = fopen("processed_data.json", "a");
    
    if (file == NULL) { 
    fprintf(stderr, "Error: Unable to open the file 'processed_data.json' for writing.\n");
    return 1; 
	} 
  
    fputs(data, file); 
    fclose(file);

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
