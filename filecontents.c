#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define APP_PASSWORD "kwjj rnfo duqs ehwd"

struct UploadStatus {
    const char *data;
    size_t size;
    FILE *attachmentFile;
};

static size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp) {
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

static size_t ReadFileCallback(void *ptr, size_t size, size_t nmemb, FILE *stream) {
    return fread(ptr, size, nmemb, stream);
}

void sendEmail(const char *subject, const char *body, const char *attachmentFilePath) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        const char *from = "sidrafaruqi17@gmail.com";  // Set a default 'from' value
        const char *to = "sidrafaruqi17@gmail.com";    // Set a default 'to' value

        // Construct the email payload
        char payload[2048];
        snprintf(payload, sizeof(payload), "From: %s\r\nTo: %s\r\nSubject: %s\r\n\r\n%s", from, to, subject, body);

        struct UploadStatus upload_ctx = {payload, strlen(payload), NULL};

        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);

        struct curl_slist *recipients = NULL;
        recipients = curl_slist_append(recipients, to);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_USERNAME, from);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, APP_PASSWORD);

        curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadCallback);
        curl_easy_setopt(curl, CURLOPT_READDATA, &upload_ctx);
        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        if (attachmentFilePath) {
            upload_ctx.attachmentFile = fopen(attachmentFilePath, "r");
            if (upload_ctx.attachmentFile) {
                curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadFileCallback);
                curl_easy_setopt(curl, CURLOPT_READDATA, upload_ctx.attachmentFile);
            } else {
                fprintf(stderr, "Failed to open attachment file: %s\n", attachmentFilePath);
                curl_slist_free_all(recipients);
                curl_easy_cleanup(curl);
                curl_global_cleanup();
                return;  // Exit the function to avoid further processing
            }
        }

        // Set private data to upload_ctx
        curl_easy_setopt(curl, CURLOPT_PRIVATE, &upload_ctx);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        }

        if (upload_ctx.attachmentFile) {
            fclose(upload_ctx.attachmentFile);
        }

        curl_slist_free_all(recipients);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main(void) {
    // Example usage of the function with specified 'subject' and 'body', and an attachment file
    sendEmail("Test Subject", "This is the body of the email.", "/home/sidra/Desktop/output.txt");

    // Example usage with specified 'subject' and 'body', without an attachment file
    sendEmail("Another Subject", "Another email body.", NULL);

    return 0;
}

