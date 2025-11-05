#ifndef OBJECTS_H
#define OBJECTS_H

#include <stdint.h>

/*standard*/
typedef enum BejType
{
  BEJ_SET = 0x00, 
  BEJ_ARRAY = 0x01,
  BEJ_NULL = 0x02,
  BEJ_INTEGER = 0x03,
  BEJ_ENUM = 0x04,
  BEJ_STRING = 0x05 

} BejType;


/*Forward declaration*/
struct BejSet;


/*key : value*/
typedef struct JsonPair
{   union
    {
      char *key; /*for json parsing*/
      uint8_t id;  /*for bej parsing*/
    };
    struct BejSet* value;
} JsonPair;



typedef struct BejSet
{
    BejType type;
    union 
    {
        char* string_value;
        int32_t integer_value;
        struct
        {
            JsonPair* pairs;
            uint16_t count;
        } object_value;
    };
} BejSet;


/*dictionary structure*/
typedef struct BejDictionary
{
    uint16_t id;
    const char* name;
    BejType type;
} BejDictionary;


#endif
