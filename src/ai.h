#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "http.h"

typedef struct _ai_data
{
  char* context;
  char* response;
  char* prompt;
  char* system;
} ai_data;

typedef int bool;
#define TRUE 255
#define FALSE 0

int send_post_request_to_ai(int sd, struct http_url* url, ai_data* ai_prompt);
ai_data* speak_to_ollama(ai_data* ai_prompt);
bool check_if_ollama_exists();
void* _command_ai(char** argv, int argc);
void* _command_ai_test(char** argv, int argc);
