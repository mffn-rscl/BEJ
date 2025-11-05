#ifndef DICTIONARY_H
#define DICTIONARY_H

#include <stdint.h>

#include "objects.h"

extern BejDictionary main_dictionary[];

extern BejDictionary child_dictionary[];

BejDictionary * bej_get_child_dictionary(uint8_t parent_id);
const char * bej_find_in_dictionary(BejDictionary *dict, uint8_t id, BejType *type);


#endif
