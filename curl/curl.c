#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <curl/curl.h>

/**
  CC=arm-linux-gnueabihf-gcc ./configure --host=arm-linux-gnueabihf --prefix=/opt/workspace/opensource/arm-linux-gnueabihf/curl-8.5.0 --enable-static --enable-shared --with-openssl
*/

/*
static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    memcpy(userp, data, real_size);
    return real_size;
}
*/
struct MemoryStruct {
    char *memory;
    size_t size;
};

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    struct MemoryStruct *mem = (struct MemoryStruct *)userp;

    printf("real size = %ld\n", real_size);
    char *ptr = realloc(mem->memory, mem->size + real_size + 1);
    if (ptr == NULL) {
        printf("Not enough memory!\n");
        return 0;
    }

    mem->memory = ptr;
    memcpy(&(mem->memory[mem->size]), contents, real_size);
    mem->size += real_size;
    mem->memory[mem->size] = 0;

    return real_size;
}

int main(void) {
    CURL *curl;
    CURLcode res;
    //char response[1024] = {0};
    long http_code;

    struct MemoryStruct chunk;
    chunk.memory = malloc(1);
    chunk.size = 0;

    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, "http://myexample.com/api/v1/security/login");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"username\":\"root\",\"password\":\"admin\",\"provider\":\"db\"}");

        //curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        //curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
            printf("Return code from web: %ld\n", http_code);
            if (http_code == 200) {
                printf("Received: %s\n", chunk.memory);
            }
        }

        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }
    free(chunk.memory);

    curl_global_cleanup();
    return 0;
}
