#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

/**
  CC=arm-linux-gnueabihf-gcc ./configure --host=arm-linux-gnueabihf --prefix=/opt/workspace/opensource/arm-linux-gnueabihf/curl-8.5.0 --enable-static --enable-shared --with-openssl
*/

/* curl static memory */
#define CURL_MEMORY_POOL_SIZE 1024 * 1024
static char curl_memory_pool[CURL_MEMORY_POOL_SIZE];
static size_t memory_pool_index = 0;

/* curl static memory func */
void *static_malloc(size_t size)
{
	if ((memory_pool_index + size) > CURL_MEMORY_POOL_SIZE) {
		printf("There is no extra memory!");
		return NULL;
	}

	void *ptr = &curl_memory_pool[memory_pool_index];
	memory_pool_index += size;

	return ptr;
}

void static_free(void *ptr)
{
	/* printf("The static memory no need to free!\n"); */
}

void *static_realloc(void *ptr, size_t size)
{
	void *newptr = static_malloc(size);

	if (newptr && ptr) {
		memcpy(newptr, ptr, size);
	}

	return newptr;
}

char *static_strup(const char *str)
{
	size_t len = strlen(str) + 1;

	char *new_str = static_malloc(len);
	if (new_str) {
		memcpy(new_str, str, len);
	}

	return new_str;
}

void *static_calloc(size_t nmemb, size_t size)
{
	size_t total_size = nmemb * size;
 
	void *mem = static_malloc(total_size);
	if (mem) {
		memset(mem, 0, total_size);
	}

	return mem;
}

void reset_memory_pool(void)
{
	memory_pool_index = 0;
	memset(curl_memory_pool, 0, sizeof(curl_memory_pool));
}

static size_t write_callback(void *data, size_t size, size_t nmemb, void *userp) {
	size_t real_size = size * nmemb;
	memcpy(userp, data, real_size);
	return real_size;
}

int main(void) {
	CURL *curl;
	CURLcode res;
	char response[1024] = {0};
	long http_code;

	//curl_global_init(CURL_GLOBAL_ALL);
	reset_memory_pool();
	curl_global_init_mem(CURL_GLOBAL_ALL, static_malloc, static_free, static_realloc, static_strup, static_calloc);

	curl = curl_easy_init();

	if(curl) {
		struct curl_slist *headers = NULL;
		headers = curl_slist_append(headers, "Content-Type: application/json");
		curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
		curl_easy_setopt(curl, CURLOPT_URL, "http://myexample.com/api/v1/security/login");
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "{\"username\":\"root\",\"password\":\"admin\",\"provider\":\"db\"}");

		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)response);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK) {
			fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
		} else {
			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			printf("Http_code: %ld, Received: %s\n", http_code, response);
		}

		curl_easy_cleanup(curl);
		curl_slist_free_all(headers);
	}

	curl_global_cleanup();
	reset_memory_pool();
	return 0;
}
