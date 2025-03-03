#include "ai.h"

// to change ???
static char ollama_ip[] = "127.0.0.1:11434";
static char* model_name = "mistral";
static double temp = 0.1;

int send_post_request_to_ai(int sd, struct http_url* url, ai_data* ai_prompt) {
  size_t json_length = strlen(ai_prompt->prompt)+strlen(ai_prompt->context)+strlen(ai_prompt->system)+256;
  size_t buf_length = json_length + 500;
  char* json = calloc(json_length,sizeof(char));
  char* buf = calloc(buf_length, sizeof(char));

  snprintf(json, json_length ,
  "\
  {\r\n\
    \"model\": \"%s\",\r\n\
    \"prompt\": \"%s\",\r\n\
    \"context\": %s,\r\n\
    \"system\": \"%s\",\r\n\
    \"stream\": false,\r\n\
    \"options\": {\r\n\
     \"temperature\": %f\r\n\
   }\r\n\
  }\r\n\
  ", model_name, ai_prompt->prompt, ai_prompt->context, ai_prompt->system, temp);

  snprintf(buf, buf_length,
		"\
POST /api/generate HTTP/1.1\r\n\
Host: %s\r\n\
User-Agent: curl/7.68.0\r\n\
Accept: */*\r\n\
Content-Length: %lu\r\n\
Content-Type: application/x-www-form-urlencoded\r\n\
\r\n\
%s\r\n\
\r\n\
", ollama_ip, strlen(json), json);

  puts(buf);

	if (http_send(sd, buf)) {
		perror("http_send");
		return -1;
	}
  free(json);
  free(buf);
	return 0;
}

//
ai_data* speak_to_ollama(ai_data* ai_prompt)
{
  struct http_url *ollama_url;
  struct http_message msg;
  int socket;
  ollama_url = http_parse_url(ollama_ip);
  if(!(socket = http_connect(ollama_url)))
  {
    free(ollama_url);
    perror("http_connect");
    return FALSE;
  }
  memset(&msg, 0, sizeof(msg));
  if(ai_prompt->context)
    free(ai_prompt->context);
  if(ai_prompt->response)
    free(ai_prompt->response);
  if (!send_post_request_to_ai(socket, ollama_url,ai_prompt))
  while (http_response(socket, &msg) > 0)
      if (msg.content)
      {
        char* saved_response = NULL;
        char* new_context = strstr(msg.content, "\"context\":");
        new_context += 10;
        int new_context_length = strcspn(new_context,"]") + 1;
        char* new_response = strstr(msg.content, "\"response\":");
        new_response += 12;
        int new_response_length = strstr(new_response, "\"done\":") - new_response - 2;
        ai_prompt->context = calloc(new_context_length+1,sizeof(char));
        ai_prompt->response = calloc(new_response_length+1,sizeof(char));
        memcpy(ai_prompt->context,new_context,new_context_length);
        memcpy(ai_prompt->response,new_response,new_response_length);
        break; // ugly hack to avoid http_response clogging up
      }

  free(ollama_url);
  close(socket);

if (msg.header.code != 200)
  {
    fprintf(
      stderr,
      "error: returned HTTP code %d\n",
      msg.header.code);
  }
  return ai_prompt;
}

bool check_if_ollama_exists()
{
  struct http_message msg;
  int socket = http_request(ollama_ip);
  memset(&msg, 0, sizeof(msg));
	while (http_response(socket, &msg));
  close(socket);
  if(msg.header.code != 200)
  {
    fprintf(stderr, "Ollama not installed or broken!!!");
    return FALSE;
  }
  return TRUE;
}

ai_data* create_ai_data()
{
  ai_data* out = malloc(sizeof(ai_data));
  out->response = NULL;
  out->system = "You are command writer from graph generating software. You translate user input into strings of graph manipulation commands. Your answers are ONLY made out of valid commands. Below is list of valid commands with their descriptions:\\nexit - exits the program\\nhelp - displays help information\\ncls - clears the screen\\ntell - prints out information about graph.\\nYour output can ONLY contain these commands, do NOT include ANTYHING else. Multiple commands should be semicolon separated.";
  out->context = calloc(4,sizeof(char));
  out->context[0] = '[';
  out->context[1] = '1';
  out->context[2] = ']';
  out->prompt = NULL;
  return out;
}

void destroy_ai_data(ai_data* data)
{
  if(data->context)
    free(data->context);
  if(data->response)
    free(data->response);
  free(data);
}

void* _command_ai_test(char** argv, int argc)
{
  if(!check_if_ollama_exists())
    return NULL;
  puts("Testing AI:");
  ai_data* test_data = create_ai_data();
  test_data->prompt = "Translate this string to valid list of commands:\\nPlease display graph information and then exit the program.";
  test_data = speak_to_ollama(test_data);
  puts(test_data->response);
  destroy_ai_data(test_data);
  return NULL;
}

void* _command_ai(char** argv, int argc)
{
  puts("Nothing yet");
  return NULL;
}

//TheNeverMan 2025
