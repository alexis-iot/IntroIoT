#include <stdio.h>
#include <string.h>
#include <curl/curl.h>

#define OK          0
#define INIT_ERR    1
#define REQ_ERR     2

int StartsWith(const char *a, const char *b) {
    if (strncmp(a, b, strlen(b)) == 0) return 1;
    return OK;
}

int CheckIfNumber(const char *a) {
    int isDigit = 1;
    int j=0;
    while(j<strlen(a) && isDigit == 1){
        if(a[j] >= 48  && a[j] <= 57)
            isDigit = 1;
        else
            isDigit = 0;
        j++;
    }
    return isDigit;
}

int SendPost(const char *url, const char *postthis) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, (long)strlen(postthis));

        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return REQ_ERR;
        }

        curl_easy_cleanup(curl);
    } else {
        return INIT_ERR;
    }

    return OK;
}

int SendPut(const char *url, const char *postthis) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if(curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return REQ_ERR;
        }

        curl_easy_cleanup(curl);
    } else {
        return INIT_ERR;
    }

    return OK;
}

int SendDelete(const char *url, const char *postthis) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postthis);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return REQ_ERR;
        }

        curl_easy_cleanup(curl);
    } else {
        return INIT_ERR;
    }

    return OK;
}

int SendGet(const char *url) {
    CURL *curl;
    CURLcode res;

    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        res = curl_easy_perform(curl);

        if(res != CURLE_OK) {
            fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            return REQ_ERR;
        }

        curl_easy_cleanup(curl);
    } else {
        return INIT_ERR;
    }

    return OK;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Not enough arguments\n");
        printf("Type --help or -h to display online help\n");
        return OK;
    }

    int isHelp = strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0;
    int isPost = strcmp(argv[1], "--post") == 0 || strcmp(argv[1], "-o") == 0;
    int isGet = strcmp(argv[1], "--get") == 0 || strcmp(argv[1], "-g") == 0;
    int isPut = strcmp(argv[1], "--put") == 0 || strcmp(argv[1], "-p") == 0;
    int isDelete = strcmp(argv[1], "--delete") == 0 || strcmp(argv[1], "-d") == 0;

    if (isHelp || isPost || isPut || isGet || isDelete) {

        if (isHelp) { // HELP

            if (argc > 2) {
                printf("Too much arguments after %s\n", argv[1]);
                return OK;
            }

            printf("Usage: assignment [options]\n");
            printf("\t-o, --post <url> <arguments> Send POST request to specified url, with additional arguments\n");
            printf("\t-g, --get <url> <arguments>  Send GET request to specified url\n");
            printf("\t-p, --put <url> <arguments>  Send PUT request to specified url, with additional arguments\n");
            printf("\t-d, --delete <url> <id>      Send DELETE request to specified url, with resource id\n");
            printf("\t-h, --help                   Obtains help information about this command\n");

            return OK;
        }

        if (argc > 2 && (strcmp(argv[2], "--url") == 0 || strcmp(argv[2], "-u") == 0)) {

            if (argc > 3 && StartsWith(argv[3], "http://") == 1 &&
                ((strstr(argv[3], "localhost:") != NULL) || (strstr(argv[3], ".") != NULL))) {

                if (isPost || isPut) {
                    char str[800];

                    for (int i = 4; i < argc; i++) {
                        if (i == 4)
                            strcpy(str, argv[i]);
                        else {
                            strcat(str, " ");
                            strcat(str, argv[i]);
                        }
                    }

                    if (isPost)
                        SendPost(argv[3], str);
                    else
                        SendPut(argv[3], str);
                } else if (isDelete) {
                    if (argc > 5) {
                        printf("Too much arguments after url\n");
                        return OK;
                    }

                    if (CheckIfNumber(argv[4]) == 0) {
                        printf("Incorrect argument after url\n");
                        return OK;
                    }

                    SendDelete(argv[3], argv[4]);
                } else if (isGet) {
                    if (argc > 4) {
                        printf("Too much arguments after url\n");
                        return OK;
                    }

                    SendGet(argv[3]);
                }
            } else {
                if (argc <= 3) {
                    printf("Not enough arguments\n");
                    return OK;
                } else {
                    printf("Incorrect argument after --url\n");
                    return OK;
                }
            }

        } else {
            if (argc <= 2) {
                printf("Not enough arguments\n");
                return OK;
            } else {
                printf("Incorrect argument after %s\n", argv[1]);
                return OK;
            }
        }
    }

    return OK;
}
