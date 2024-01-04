#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#define APP_PASSWORD "kwjj rnfo duqs ehwd"

struct UploadStatus {
    FILE *attachmentFile;
};

void sendEmail(const char *subject, const char *body, const char *attachmentFilePath) {
    CURL *curl;
    CURLcode res;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        const char *from = "sidrafaruqi17@gmail.com";  // Set a default 'from' value
        const char *to = "sidrafaruqi17@gmail.com";    // Set a default 'to' value

        struct UploadStatus upload_ctx = {NULL};

        struct curl_slist *recipients = NULL;

        // Construct the MIME headers
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: multipart/mixed;");
        headers = curl_slist_append(headers, "boundary=frontier");

        // Set email parameters
        const char *emailFrom = "From: sidrafaruqi17@gmail.com";
        const char *emailTo = "To: sidrafaruqi17@gmail.com";
        const char *emailSubject = "Subject: Test Subject";

        curl_easy_setopt(curl, CURLOPT_URL, "smtps://smtp.gmail.com:465");
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long)CURLUSESSL_ALL);

        // Set mail parameters
        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, from);
        recipients = curl_slist_append(recipients, to);
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        curl_easy_setopt(curl, CURLOPT_USERNAME, from);
        curl_easy_setopt(curl, CURLOPT_PASSWORD, APP_PASSWORD);

        // Set MIME headers
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);

        // Create a mime structure for the email
        curl_mime *mime = curl_mime_init(curl);

        // Set the body part of the email
        curl_mimepart *part = curl_mime_addpart(mime);
        curl_mime_data(part, body, CURL_ZERO_TERMINATED);

        // Set the file attachment part of the email
        if (attachmentFilePath) {
            part = curl_mime_addpart(mime);
            curl_mime_filedata(part, attachmentFilePath);
        }

        // Set the mime structure to the request
        curl_easy_setopt(curl, CURLOPT_MIMEPOST, mime);

        // Perform the email sending
        res = curl_easy_perform(curl);

        // Cleanup
        curl_slist_free_all(headers);
        curl_slist_free_all(recipients);
        if (upload_ctx.attachmentFile) {
            fclose(upload_ctx.attachmentFile);
        }
        curl_mime_free(mime);
        curl_easy_cleanup(curl);
    }

    curl_global_cleanup();
}

int main(void) {
    // Example usage of the function with specified 'subject', 'body', and an attachment file
    sendEmail("Test Subject", "This is the body of the email.", "/home/sidra/Desktop/output.txt");

    // Example usage with specified 'subject' and 'body', without an attachment file
    //sendEmail("Another Subject", "Another email body.", NULL);

    return 0;
}

