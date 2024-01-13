#include "headerfile1.h"
#include <email.h>
#define DEFAULT_TO_EMAIL "sidrafaruqi17@gmail.com"
size_t write_callback(void *contents, size_t size, size_t nmemb, void *userp) {
     size_t realsize = size * nmemb;
    FILE *fp = (FILE *)userp;
    if (fp == NULL) {
        fprintf(stderr, "Error: File pointer is NULL.\n");
        return 0;
    }
    return fwrite(contents, size, nmemb, fp);
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


int calculated_data(const char* dt_txt, double *average_temp, double lowest_temp, double highest_temp, double *average_humidity) {
     cJSON *calculated = cJSON_CreateObject();
    cJSON_AddStringToObject(calculated, "dt_txt", dt_txt);
    cJSON_AddNumberToObject(calculated, "average_temp", *average_temp);
    cJSON_AddNumberToObject(calculated, "average_humidity", *average_humidity);
    cJSON_AddNumberToObject(calculated, "lowest_temp", (double)(lowest_temp));
    cJSON_AddNumberToObject(calculated, "highest_temp", (double)(highest_temp));
    char* calculated_str = cJSON_Print(calculated);
    processed_file_opening(calculated_str);
    cJSON_Delete(calculated);
    return 0;
}


int processed_file_opening(const char* newdata) {
     FILE *fp1 = fopen("processed_data.json", "r"); 
    if (fp1 == NULL) { 
        printf("Error: Unable to open the file.\n"); 
        return 1;
    }     
    fseek(fp1, 0, SEEK_END);
    int length = ftell(fp1);
    fseek(fp1, 0, SEEK_SET);
    char *data = (char *)malloc(length + 1); // Allocate memory for data
    if (data == NULL) {
        fclose(fp1);
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }
    size_t len = fread(data, 1, length, fp1);//Reading file content
    if (len != length) {
        free(data); // Free allocated memory
        fclose(fp1);
        fprintf(stderr, "Error reading file content.\n");
        return 1;
    }
    data[length] = '\0'; // Null-terminate the data
    fclose(fp1);
    cJSON *json = cJSON_Parse(data); 
    free(data); // Free allocated memory
    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error parsing JSON: %s\n", error_ptr);
        }
        return 1;
    }
    cJSON *array = cJSON_GetObjectItemCaseSensitive(json, "myArray"); //Accessing "myArray" from processed_data file
    cJSON *newItem = cJSON_Parse(newdata);//converting it into javaScript object
    cJSON_AddItemToArray(array, newItem);//now add it into "myArray"
    FILE *fp2 = fopen("processed_data.json", "w"); 
    if (fp2 == NULL) { 
       printf("Error: Unable to open the file.\n"); 
       return 1; 
   } 
    fprintf(fp2, "%s", cJSON_Print(json));
    fclose(fp2);
    cJSON_Delete(json);
    return 0;  
}


int report_generating() {
    FILE *fp1 = fopen("processed_data.json", "r");
    if (fp1 == NULL) {
        printf("Error: Unable to open the file.\n");
        return 1;
    }

    fseek(fp1, 0, SEEK_END);
    int length = ftell(fp1);
    fseek(fp1, 0, SEEK_SET);

    char *data = (char *)malloc(length + 1); // Allocate memory for the data 
    if (data == NULL) {
        fclose(fp1);
        printf("Error: Memory allocation failed.\n");
        return 1;
    }

    size_t len = fread(data, 1, length, fp1);
    fclose(fp1);

    if (len != length) {
        free(data); // Release memory in case of read failure
        printf("Error: Reading file failed.\n");
        return 1;
    }

    data[len] = '\0'; // Null-terminate the string

    cJSON *json = cJSON_Parse(data);
    free(data);

    if (json == NULL) {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL) {
            printf("Error: %s\n", error_ptr);
        }
        cJSON_Delete(json);
        return 1;
    }

    FILE *fp2 = fopen("report_generating.txt", "w");
    if (fp2 == NULL) {
        fprintf(stderr, "Error: Unable to open the file.\n");
        return 1;
    }

    fprintf(fp2,"        WEATHER REPORT           \n");
    fprintf(fp2, "\n");

    char dateArray[MAX_SIZE][MAX_STRING_LENGTH];
    int dataSize = 0;
    
    cJSON *myArray = cJSON_GetObjectItemCaseSensitive(json, "myArray");//Accessing "myArray" from processed_data file
    if (!cJSON_IsArray(myArray)) {
        fprintf(stderr, "Error: 'myArray' is not an array.\n");
        fclose(fp2);
        return 1;
    }

    for (int i = 0; i < cJSON_GetArraySize(myArray); i++) {
        cJSON *dict = cJSON_GetArrayItem(myArray, i);//Accessing each item from the list "myArray"
        if (!cJSON_IsObject(dict)) {
            fprintf(stderr, "Error: Invalid object in 'myArray'.\n");
            fclose(fp2);
            return 1;
        }

        cJSON *dt_txt = cJSON_GetObjectItem(dict, "dt_txt");
        cJSON *average_temp = cJSON_GetObjectItem(dict, "average_temp");
        cJSON *lowest_temp = cJSON_GetObjectItem(dict, "lowest_temp");
        cJSON *highest_temp = cJSON_GetObjectItem(dict, "highest_temp");
        cJSON *average_humidity = cJSON_GetObjectItem(dict, "average_humidity");
	
        //in myArray every item doesnot contain my required values, so we will check
    	if (average_temp && cJSON_IsNumber(average_temp)) {
            char date[20];
            strcpy(date, dt_txt->valuestring);

            if (!isDateAlreadyWritten(date, dateArray, dataSize)) {
                strncpy(dateArray[dataSize], date, MAX_STRING_LENGTH - 1);
                dateArray[dataSize][MAX_STRING_LENGTH - 1] = '\0'; // Ensure null-terminated
                dataSize++;

                fprintf(fp2, "DATE: %s\n", dt_txt->valuestring);
                fprintf(fp2, "Average Temp: %.2f\n", average_temp->valuedouble);

                if (lowest_temp && cJSON_IsNumber(lowest_temp)) {
                    fprintf(fp2, "Lowest Temp: %d\n", (int)lowest_temp->valuedouble);
                }

                if (highest_temp && cJSON_IsNumber(highest_temp)) {
                    fprintf(fp2, "Highest Temp: %d\n", (int)highest_temp->valuedouble);
                }

                if (average_humidity && cJSON_IsNumber(average_humidity)) {
                    fprintf(fp2, "Average Humidity: %.2f\n", average_humidity->valuedouble);
                }

                fprintf(fp2, "\n");
            } 
        } else {
            continue; // Skip to the next iteration if average_temp is not present
        }
    }

    fclose(fp2);
    return 0;
}

bool isDateAlreadyWritten(const char* date, const char dateArray[MAX_SIZE][MAX_STRING_LENGTH], int dataSize) {
    for (int i = 0; i < dataSize; ++i) {
        if (strcmp(dateArray[i], date) == 0) {
            return true; // Date already exists
        }
    }
    return false; // Date not found
}

int retrieve_values() {
     FILE *fp = fopen("api_response.json","r");
    if (fp==NULL){
	fprintf(stderr, "Error: Unable to open the file.\n");
	return 1;
	}
    fseek(fp, 0, SEEK_END);
    int length = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    char *content = (char *)malloc(length + 1); // Allocating memory innstead of creating an array, as it will handle the memory more carefully
    if (content == NULL) {
        fclose(fp);
        fprintf(stderr, "Error: Memory allocation failed.\n");
        return 1;
    }
    size_t file_content = fread(content, 1, length, fp); // Reading file content
    if (file_content != length) {
        free(content); // Free allocated memory
        fclose(fp);
        fprintf(stderr, "Error reading file content.\n");
        return 1;
    }
    content[length] = '\0'; // Null-terminate the content
    fclose(fp);
    cJSON *json = cJSON_Parse(content); // to convert json string into javascript object
    free(content); //free the allocated memory
    if (json == NULL) { 
        const char *error_ptr = cJSON_GetErrorPtr(); 
        if (error_ptr != NULL) { 
            printf("Error: %s\n", error_ptr); 
        } 
        cJSON_Delete(json); 
        return 1; 
    } 
    //initializing variables
    cJSON *dt_txt = NULL;
    cJSON *temp = NULL;
    cJSON *temp_min = NULL;
    cJSON *temp_max = NULL;
    cJSON *humidity = NULL;
    int total_day1 = 0, total_day2 = 0;
    double total_value1 = 0, total_value2 = 0; 
    double average_temp1, average_temp2;
    int lowest_temp1 = 1000;
    int lowest_temp2 = 1000;
    int highest_temp1 = 0;
    int highest_temp2 = 0;
    double total_humidity1 = 0, total_humidity2 = 0; 
    double average_humidity1, average_humidity2;
    time_t current_time;
    struct tm *local_time;
    time(&current_time); //finding the current time
    local_time = localtime(&current_time);
    int cyear = local_time->tm_year + 1900;
    int cmonth = local_time->tm_mon + 1;
    int cday = local_time->tm_mday;
    cJSON *cnt = cJSON_GetObjectItemCaseSensitive(json, "cnt"); //cnt tells the length of "list" key from api_response.json file
    cJSON *list = cJSON_GetObjectItemCaseSensitive(json, "list");//all the data related to weather is stored in "list"
    if (cJSON_IsArray(list) && (list != NULL) && cJSON_IsNumber(cnt) && (cnt != NULL)) { 
    	for (int i = 0; i< cnt->valueint; i++) {
		cJSON *first_dict = cJSON_GetArrayItem(list, i);//accessing the dictionaries from "list"
		if (first_dict != NULL && cJSON_IsObject(first_dict)) {
		    dt_txt = cJSON_GetObjectItem(first_dict, "dt_txt");//accessing the key value pairs
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
	       			//spliting the string and initializing them into the diff variables
	       			sscanf(datetimeString, "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
			     	//As we want to access data for 2 days. so we will check before accessing it
			     	if (cyear == year && cmonth == month && cday == day) {
              			    total_day1++; //checks how many 3 hours data is present for a day
              			    total_value1 = total_value1 + temp->valuedouble;//sum temperature values
              			    total_humidity1 = total_humidity1 + humidity->valuedouble;//sum humidity values
              			    //checking for the lowest temperature
				    if (lowest_temp1 > cJSON_GetNumberValue(temp_min)) {
				        lowest_temp1 = cJSON_GetNumberValue(temp_min);
				    }
            				
				    //checking for the highest temperature
				    if (highest_temp1 < cJSON_GetNumberValue(temp_max)) {
				        highest_temp1 = cJSON_GetNumberValue(temp_max);
				    }

				    //Checking for anomalies
				    if (highest_temp1 > 50) {
            				printf("ANOMALY DETECTED!");
            				}


				    //calling function to create objects for data
				    processed_data(dt_txt->valuestring, temp, temp_min, temp_max, humidity); 
				}
				else if (cyear == year && cmonth == month && cday + 1 == day) {
				    total_day2++;
				    total_value2 = total_value2 + temp->valuedouble; 
				    total_humidity2 = total_humidity2 + humidity->valuedouble;
				    if (lowest_temp2 > cJSON_GetNumberValue(temp_min)) {
				        lowest_temp2 = cJSON_GetNumberValue(temp_min);
				    }

				    //GENERATES ALERT MAIL
				    if (lowest_temp2 < 19) {
            				sendEmail(DEFAULT_TO_EMAIL, "RS WEATHER ALERT", "The temperature will fall below 19 C. DRESS WARM FOR TOMORROW :) ", NULL);
            				}


				    if (highest_temp2 < cJSON_GetNumberValue(temp_max)) {
				        highest_temp2 = cJSON_GetNumberValue(temp_max);
				    }	
				    processed_data(dt_txt->valuestring, temp, temp_min, temp_max, humidity); 
				}


		                else{
		                    break; 
		                }			     
  		     }

 	}
 	//GENERATES ALERT MAIL
 	if (lowest_temp2 < 19) {
            		sendEmail(DEFAULT_TO_EMAIL, "RS WEATHER ALERT", "The temperature will fall below 19 C. DRESS WARM FOR TOMORROW :) ", NULL);
        }
        //Checking for anomolies
	if (highest_temp1 > 50) {
            		show_notification("WARNING", "ANOMOLY DETECTED!");
       }

    }
    //Calculations for accessed data
    char date1[20]; 
    if (total_day1 != 0) {
	average_temp1 = total_value1 / total_day1;//average temperatue of a day
	average_humidity1 = total_humidity1 / total_day1;//average humidity of a day
	// Convert year, month, and day to string and concatenate them
        snprintf(date1, sizeof(date1), "%d-%02d-%02d", cyear, cmonth, cday);
	calculated_data(date1, &average_temp1, lowest_temp1, highest_temp1, &average_humidity1); 
    }
    char date2[20];
    if (total_day2 != 0) {
	average_temp2 = total_value2 / total_day2;  
	average_humidity2 = total_humidity2 / total_day2; 
        snprintf(date2, sizeof(date2), "%d-%02d-%02d", cyear, cmonth, cday+1);
	calculated_data(date2, &average_temp2, lowest_temp2, highest_temp2, &average_humidity2); 
    }
     
    cJSON_Delete(json);
}


void show_notification(const char *title, const char *message) {
    // Calculate the required size for the command string
    size_t command_size = snprintf(NULL, 0, "notify-send '%s' '%s'", title, message) + 1;

    // Allocate memory for the command string
    char *command = malloc(command_size);

    // Build the command using snprintf
    snprintf(command, command_size, "notify-send '%s' '%s'", title, message);

    // Use system() to execute the command
    int result = system(command);

    // Free the dynamically allocated memory
    free(command);
}
