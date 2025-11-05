/* * * * * * * * * * * * * * * * * * * * * * * *
 * This file contains functions that 
 * can be used to parse a JSON file into 
 * a C structure. In the future, functionality 
 * will be added to parse data directly from 
 * JSON to BEJ.
 * * * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../include/json_parse.h"
#define PAIR_BUFFER 20


char* json_load_file(const char *file_name)
{
    FILE* f = fopen(file_name, "r");
    if (!f) return NULL;
    
    fseek(f, 0, SEEK_END);
    uint32_t size = ftell(f);
    rewind(f);

    char* data = malloc(size + 1);
    
    if(!data) 
    {
      fclose(f);
      return NULL;
    }
    

    fread(data, 1, size, f);
    data[size] = '\0';
    fclose(f);
    return data;
}

void json_skip_spaces(const char** text) 
{
    while (**text == ' ' || **text == '\n' || **text == '\t')(*text)++;
}


char *json_read_string(const char **text)
{
  if(**text != '"')return NULL;
  (*text)++;

  const char *start = *text;
  while (**text != '"' && **text != '\0') (*text)++;
  
  if (**text != '"') return NULL; 
  
  int32_t length = *text - start;
  char* result = (char*)malloc(length + 1);
  strncpy(result, start, length);
  result[length] = '\0';

  (*text)++; /*skip '"'*/
  
  return result;
}

int32_t json_read_integer(const char** text) 
{
    char* end;
    int32_t num =(int32_t) strtol(*text, &end, 10);
    if(end == *text) return 0; /*parsing error*/
    *text = end;
    return num;
}


BejSet* json_read_value(const char **text) 
{
    json_skip_spaces(text);
    BejSet *val = malloc(sizeof(BejSet));
    if(!val) return NULL;  


    if (**text == '"')
    {
        val->type = BEJ_STRING;
        val->string_value = json_read_string(text);
        
        if(!val->string_value)
        {
          free(val);
          return NULL;
        }

    } 
    else if (isdigit(**text)) 
    {
        val->type = BEJ_INTEGER;
        val->integer_value = json_read_integer(text);
    }
    else if (**text == '{') 
    {
        free(val);
        val = json_read_object(text);
    }
    else 
    {
        free(val);
        return NULL;
    }

    return val;
}


BejSet* json_read_object(const char **text) 
{
    BejSet *obj = malloc(sizeof(BejSet));
    if(!obj) return NULL;

    obj->type = BEJ_SET;
    obj->object_value.count = 0;
    

    obj->object_value.pairs = malloc(sizeof(JsonPair) * PAIR_BUFFER);
    if (!obj->object_value.pairs) 
    {
        free(obj);
        return NULL;
    }

    

    
    (*text)++; /*skip '{'*/
    json_skip_spaces(text);

    while (**text != '}' && **text != '\0') 
    {

        json_skip_spaces(text);

        char *key = json_read_string(text);
        if(!key)
        {
          json_free(obj);
          return NULL;
        }
        
        json_skip_spaces(text);
        
        if (**text == ':') (*text)++;
        json_skip_spaces(text);

        BejSet *value = json_read_value(text);
        if (!value) 
        {
            free(key);
            json_free(obj);
            return NULL;
        }

        obj->object_value.pairs[obj->object_value.count].key = key;
        obj->object_value.pairs[obj->object_value.count].value = value;
        obj->object_value.count++;

        json_skip_spaces(text);
        if (**text == ',') 
        {
            (*text)++;
            json_skip_spaces(text);
        }
    }

    if (**text == '}') (*text)++;
    return obj;
}



void json_free(BejSet *obj) 
{
    if (!obj) return;

    if (obj->type == BEJ_SET) 
    {
        for (int i = 0; i < obj->object_value.count; i++)
        {
            free(obj->object_value.pairs[i].key);
            json_free(obj->object_value.pairs[i].value);
        }
        free(obj->object_value.pairs);
    } 
    else if (obj->type == BEJ_STRING) 
    {
        free(obj->string_value);
    }

    free(obj);
}



