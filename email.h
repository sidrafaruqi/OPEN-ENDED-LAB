#ifndef EMAIL_H
#define EMAIL_H

#include <stddef.h>

// Structure definition for UploadStatus
struct UploadStatus {
    const char *data;
    size_t size;
};

// Function prototype for ReadCallback
size_t ReadCallback(void *ptr, size_t size, size_t nmemb, void *userp);

// Function prototype for sendEmail
void sendEmail(const char *to_email, const char *subject, const char *body, const char *attachmentPath);

#endif

