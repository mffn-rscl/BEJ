/**
 * @file bej_parse.c
 * @brief BEJ (Binary Encoded JSON) parser implementation
 * 
 * This file contains functions for parsing BEJ format data and converting
 * it to JSON format. BEJ is a binary encoding format for JSON-like data.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../include/bej_parse.h"

/**
 * @brief Stores the length of the last read field in SET parsing
 * @note This is used to track position while parsing nested SET structures
 */
static uint8_t last_read_field_length = 0; 

/**
 * @brief Loads binary file into memory
 * 
 * @param file_name Path to the file to load
 * @return Pointer to allocated buffer containing file data, or NULL on error
 * @note Caller is responsible for freeing the returned buffer
 */
uint8_t *bej_load_file(const char *file_name)
{
  FILE *f = fopen(file_name, "rb");
  if (!f)
  {
    return NULL;
  }
  
  fseek(f, 0, SEEK_END);
  uint32_t size = ftell(f);
  if(!size) printf("size = NULL\n");
  rewind(f);
  
  uint8_t *data = malloc(size);
  if (!data)
  {
    fclose(f);
    return NULL;
  }
  
  if (fread(data, 1, size, f) != size)
  {
    free(data);
    fclose(f);
    return NULL;
  }
  
  fclose(f);
  return data;
}

/**
 * @brief Reads an integer value from BEJ data stream
 * 
 * Reads a variable-length integer encoded in little-endian format.
 * Updates the data pointer and last_read_field_length.
 * 
 * @param data Pointer to the data pointer (will be advanced)
 * @return Decoded integer value
 */
uint32_t bej_read_integer(const uint8_t **data)
{
  uint8_t length = *(++(*data));
  last_read_field_length = length;
  uint32_t res = 0;
  for (size_t i = 0; i < length; i++)
  {
    res |= ((uint32_t)*(++(*data))) << (8 * i);
  }
  return res;
}

/**
 * @brief Reads a string value from BEJ data stream
 * 
 * Reads a length-prefixed string and null-terminates it.
 * Updates the data pointer and last_read_field_length.
 * 
 * @param data Pointer to the data pointer (will be advanced)
 * @return Pointer to allocated null-terminated string, or NULL on error
 * @note Caller is responsible for freeing the returned string
 */
char *bej_read_string(const uint8_t **data)
{
  uint8_t length = *(++(*data));
  last_read_field_length = length;
  char *res = malloc(length + 1);
  if (!res)
    return NULL;
  
  for (int i = 0; i < length; i++)
  {
    res[i] = *(++(*data));
  }
  res[length] = '\0';
  return res;
}

/**
 * @brief Reads a BEJ SET (object) from data stream
 * 
 * Parses a BEJ SET structure containing multiple key-value pairs.
 * Recursively parses nested objects using the appropriate dictionary.
 * 
 * @param data Pointer to the data pointer (will be advanced)
 * @param parent_id ID of the parent element (used for dictionary lookup)
 * @param dict Dictionary for resolving field names
 * @return Pointer to allocated BejSet structure, or NULL on error
 * @note Caller is responsible for freeing the returned structure using bej_free()
 */
BejSet *bej_read_object(const uint8_t **data, uint8_t parent_id, BejDictionary *dict)
{
  
  BejSet *obj = malloc(sizeof(BejSet));
  if (!obj)
    return NULL;
  
  obj->type = BEJ_SET;
  obj->object_value.count = 0;
  
  obj->object_value.pairs = malloc(sizeof(JsonPair) * PAIR_BUFFER);
  if (!obj->object_value.pairs)
  {
    free(obj);
    return NULL;
  }
  
  uint8_t bytes_len = *(++(*data));
  
  BejDictionary *child_dict = bej_get_child_dictionary(parent_id);

  ++(*data); /* Skip to index */

  /* ID for object_value */
  uint16_t object_val_id = 1;
  while(bytes_len > 0)
  {
    BejSet *value = bej_read_value(data, child_dict);
    ++(*data); /* Skip to index */
    if (value == NULL) break;
    
    obj->object_value.pairs[obj->object_value.count].id = object_val_id++;
    obj->object_value.pairs[obj->object_value.count].value = value;
    obj->object_value.count++;
    

    bytes_len -= last_read_field_length;
  }
  
  return obj;
}

/**
 * @brief Reads any BEJ value from data stream
 * 
 * Determines the type of the value and calls the appropriate parsing function.
 * Handles INTEGER, STRING, and SET types.
 * 
 * @param data Pointer to the data pointer (will be advanced)
 * @param dict Dictionary for resolving field names in nested SETs
 * @return Pointer to allocated BejSet structure, or NULL on error
 * @note Caller is responsible for freeing the returned structure using bej_free()
 */
BejSet *bej_read_value(const uint8_t **data, BejDictionary *dict)
{
  BejSet *val = malloc(sizeof(BejSet));
  if (!val)
    return NULL;
  
  uint8_t id = **data;
  
  val->type = *(++(*data));
  
  if (val->type == BEJ_INTEGER)
  {
    val->integer_value = bej_read_integer(data);
  }
  else if (val->type == BEJ_STRING)
  {
    val->string_value = bej_read_string(data);
  }
  else if (val->type == BEJ_SET)
  {
    free(val);
    
    val = bej_read_object(data, id, dict);
  }
  else
  {
    free(val);
    return NULL;
  }
  
  return val;
}

/**
 * @brief Recursively frees BejSet structure and all its contents
 * 
 * Handles freeing of strings, nested objects, and the structure itself.
 * Safe to call with NULL pointer.
 * 
 * @param val Pointer to BejSet structure to free
 */
void bej_free(BejSet *val)
{
  
  if (!val) return;
  
  if (val->type == BEJ_STRING)
  {
    if (val->string_value)
      free(val->string_value);
  }
  else if (val->type == BEJ_SET)
  {
    for (size_t i = 0; i < val->object_value.count; i++)
    {
      bej_free(val->object_value.pairs[i].value);
    }
    free(val->object_value.pairs);
  }
  
  free(val);
}

/**
 * @brief Converts BejSet value to JSON format and writes to file
 * 
 * Recursively converts BEJ structure to JSON with proper formatting and indentation.
 * 
 * @param val BejSet structure to convert
 * @param dict Dictionary for resolving field names
 * @param f File handle to write JSON output
 * @param depth Current indentation depth
 */
void bej_to_json_val(BejSet *val, BejDictionary *dict, FILE *f, int depth)
{
  if (!val || !f) return;
  
  if (val->type == BEJ_INTEGER)
  {
    fprintf(f, "%d", val->integer_value);
  }
  else if (val->type == BEJ_STRING)
  {
    fprintf(f, "\"%s\"", val->string_value ? val->string_value : "");
  }
  else if (val->type == BEJ_SET)
  {
    fprintf(f, "{\n");
    for (size_t i = 0; i < val->object_value.count; i++)
    {
      uint8_t id = val->object_value.pairs[i].id;
      const char *name = bej_find_in_dictionary(dict, id, NULL);
      
      if (!name) name = "UNKNOWN";
    
      
      for (int j = 0; j < depth + 1; j++) fprintf(f, "  ");
      fprintf(f, "\"%s\": ", name);
      
      BejDictionary *child_dict = bej_get_child_dictionary(id);
      bej_to_json_val(val->object_value.pairs[i].value, child_dict, f, depth + 1);
      
      if (i < val->object_value.count - 1)
        fprintf(f, ",");
      fprintf(f, "\n");
    }
    for (int j = 0; j < depth; j++) fprintf(f, "  ");
    fprintf(f, "}");
  }
}

/**
 * @brief Converts BEJ root object to JSON and saves to file
 * 
 * @param root Root BejSet structure to convert
 * @param filename Output filename for JSON data
 */
void bej_to_json_file(BejSet *root, const char *filename)
{
  FILE *f = fopen(filename, "w");
  if (!f) return;
  
  bej_to_json_val(root, main_dictionary, f, 0);
  fprintf(f, "\n");
  fflush(f);

  fclose(f);
}











