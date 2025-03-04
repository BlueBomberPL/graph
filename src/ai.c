#include "ai.h"

// AI settings
static const char* const ollama_ip = "127.0.0.1:11434";
static const char* const model_name = "mistral";
static const char* const prompt_header = "Convert following user input to commands, if it is a question then your list of commands should provide an answer for it:\\n";
static const char* const default_system_prompt = "\
You are command writer from graph generating software. \
You translate user input into strings of graph manipulation commands. \
Your answers are ONLY made out of valid commands.\\n\
Some commands take input arguments - arguments are highlited with <> brackets and are explained \
in command description (always remember about appropriate arguments).\\n\
NEVER try to make up any commands and NEVER respond with command that are not on the list below.\\n\
Below is list of valid commands with their descriptions:\\n\
exit - exits the program. This command takes no arguments.\\n\
help - displays help information. This command takes no arguments.\\n\
cls - clears the screen. This command takes no arguments.\\n\
tell - prints out graph's size and number of arches and vertices. This command takes no arguments.\\n\
list - prints the graph. This command takes no arguments.\\n\
del <A> - deletes <A> vertex, where <A> argument is an index of deleted vertex.\\n\
file <name> - saves graph to file, where <name> argument is file name.\\n\
new - clears graph and ERASES all arches and vertices. This command takes no arguments.\\n\
find <A> <B> - Checks if arch from vertex <A> to vertex <B> exists. <A> and <B> arguments are \
indexes of vertices to search for.\\n\
size <n> - sets graph size to <n>, where <n> argument is positive new number of vertices.\\n\
add <A> <B> <C> - adds NEW VERTEX to the graph with connections to vertices <A> <B> <C>, \
where <A> <B> <C> arguments are indexes of vertices to which new vertex is connected, \
Number of arguments of this command depends on number of connections user wants, \
so it can range from 0 if new vertex should not have any connections to as many as user specifies. Arguments for this command DO NOT contain index of newly added vertex, DO NOT add it to argument list.\\n\
set <A>: <B> <C> <D> - updates and modifies connections of vertex <A>, so it is only connected to vertexes <B> <C> <D>. \
<A> <B> <C> <D> arguments are indexes of vertices which should have connection to vertex <A>. \
Number of arguments of this command depends on user input. \
<A> argument MUST be specified, but <B> <C> <D> depend only on number of specified vertices, \
so it can range from 0 to as much vertices user specifies.\\n\
arch add <A> <B> - ADDS CONNECTION between existing vertices <A> and <B>. \
Arguments <A> and <B> are indexes of existing vertices between which arch is ADDED.\\n\
arch del <A> <B> - DELETES CONNECTION between existing vertices <A> and <B>. \
Arguments <A> and <B> are indexes of existing vertices between which arch is DELETED.\\n\
Indexes of vertices are numbers.\\n\
Your output can ONLY contain these commands, do NOT include ANTYHING else.\\n\
Words surrounded in <> brackets are command arguments. Some commands require them, \
when necessary always deduct VALID command arguments (from user input) and add them to command.\\n\
Command and its arguments are space separated.\\n\
Multiple commands should be semicolon separated.\\n\
If you can't generate any commands at all or user input is invalid, \
respond like you got \\\"Display help information.\\\" input.\\n";
static const double temp = 0.8;

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
     \"num_thread\": 4,\r\n\
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
  out->system = calloc(strlen(default_system_prompt)+2,sizeof(char));
  memcpy(out->system,default_system_prompt,strlen(default_system_prompt));
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
  if(data->system)
    free(data->system);
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
  size_t user_input_pointer = 0;
  size_t user_input_length = 12;
  char* user_input = calloc(user_input_length,sizeof(char));
  puts("Please enter prompt for AI:");
  char input = '0';
  fflush(stdin);
  while(input != '\n')
  {
    input = getc(stdin);
    if((user_input_pointer+1) == user_input_length)
    {
      user_input_length *= 2;
      user_input = realloc(user_input,sizeof(char)*user_input_length);
    }
    if(input != '\n')
      user_input[user_input_pointer] = input;
    // printf("%i %c %i")
    user_input_pointer++;
  }
  ai_data* user_data = create_ai_data();
  user_data->prompt = calloc(strlen(prompt_header)+strlen(user_input)+1,sizeof(char));
  memcpy(user_data->prompt,prompt_header,strlen(prompt_header));
  strcat(user_data->prompt,user_input);
  free(user_input);
  user_data = speak_to_ollama(user_data);
  char* returned_command_list = user_data->response;
  char* command = strtok(returned_command_list,";");
  puts("AI converted prompt to following commands:");
  while(command)
  {
    puts(command);
    command = strtok(NULL,";");
  }
  destroy_ai_data(user_data);
  return NULL;
}

//TheNeverMan 2025
