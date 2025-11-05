#ifndef BEJ_PARSE_H
#define BEJ_PARSE_H

#include "objects.h"
#include "dictionary.h"
#define PAIR_BUFFER 32





uint8_t *bej_load_file(const char *file_name);


uint32_t bej_read_integer(const uint8_t **data);

char *bej_read_string(const uint8_t **data);

BejSet *bej_read_object(const uint8_t **data, uint8_t parent_id, BejDictionary *dict);

BejSet *bej_read_value(const uint8_t **data, BejDictionary *dict);


void bej_free(BejSet *val);





void bej_to_json_val(BejSet *val, BejDictionary *dict, FILE *f, int depth);

void bej_to_json_file(BejSet *root, const char *filename);
#endif

