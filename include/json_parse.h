#ifndef JSON_PARSE_H
#define JSON_PARSE_H

#include "objects.h"

char* json_load_file(const char *file_name);

void json_skip_spaces(const char** text);

char *json_read_string(const char **text);
int32_t json_read_integer(const char** text); 
BejSet* json_read_object(const char **text); 

BejSet* json_read_value(const char **text); 

void json_free(BejSet *obj);

#endif
