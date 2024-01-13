#include "email.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

// Definition of ReadCallback
size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp) {
    // Implementation of ReadCallback function
    struct UploadStatus *upload_ctx = (struct UploadStatus *)userp;

    size_t to_copy = size * nmemb;
    if (to_copy > upload_ctx->size)
        to_copy = upload_ctx->size;

    if (to_copy) {
        memcpy(ptr, upload_ctx->data, to_copy);
        upload_ctx->data += to_copy;
        upload_ctx->size -= to_copy;
        return to_copy;
    }

    return 0;
}

// Definition of sendEmail
void sendEmail(const char *to_email, const char *subject, const char *body, const char *attachmentPath) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        const char *from = "sidrafaruqi17@gmail.com";

        // Construct the email payload
        char payload[4096];
        snprintf(payload, sizeof(payload), "From: %s\r\nTo: %s\r\nSubject: %s\r\n\r\n%s", from, to_email, subject, body);

        if (attachmentPath) {
            FILE *file = fopen(attachmentPath, "r");
            if (file) {
                char *file_content = NULL;
                size_t file_size;

                fseek(file, 0, SEEK_END);
                file_size = ftell(file);
                fseek(file, 0, SEEK_SET);

                file_content = (char *)malloc(file_size + 1);
                fread(file_content, 1, file_size, file);
                fclose(file);

                file_content[file_size] = '\0';
                strcat(payload, "\n");
                strcat(payload, file_content);

                free(file_content);
            } else {
                fprintf(stderr, "Unable to open file: %s\n", attachmentPath);
            }
        }

        struct UploadStatus upload_ctx = {payload, strlen(payload)};

        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);

        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, to_email);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_USERNAME, from);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, "kwjj rnfo duqs ehwd");

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}



