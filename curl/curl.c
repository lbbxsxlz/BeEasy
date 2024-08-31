#include <stdio.h>
#include <string.h>
#include <curl/curl.h>
#include "cJSON.h"

/**
  CC=arm-linux-gnueabihf-gcc ./configure --host=arm-linux-gnueabihf --prefix=/opt/workspace/opensource/arm-linux-gnueabihf/curl-8.5.0 --enable-static --enable-shared --with-openssl
*/

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
    size_t real_size = size * nmemb;
    memcpy(userp, data, real_size);
    return real_size;
}

int main(void) {
    CURL *curl;
    CURLcode res;
    char response[1024] = {0};
    cJSON *object = NULL;
    cJSON *token = NULL;

    // 初始化libcurl
    curl_global_init(CURL_GLOBAL_ALL);
    curl = curl_easy_init();

    if(curl) {
        struct curl_slist *headers = NULL;
        headers = curl_slist_append(headers, "Content-Type: application/json");
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, "http://myexample.com/api/v1/security/login");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"username\":\"root\",\"password\":\"admin\"}");

        // 设置接收响应数据的回调函数
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

        // 执行HTTP POST请求
        res = curl_easy_perform(curl);

        // 检查错误
        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
        } else {
            printf("Received: %s\n", response);
            object = cJSON_Parse(response);
            token = cJSON_GetObjectItem(object, "access_token");
            printf("token: %s\n", token->valuestring);
            cJSON_Delete(object);
        }

        // 清理
        curl_easy_cleanup(curl);
        curl_slist_free_all(headers);
    }

    curl_global_cleanup();
    return 0;
}
