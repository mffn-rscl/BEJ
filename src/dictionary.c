/**
 * @file dictionary.c
 * @brief BEJ dictionary implementation
 * 
 * Contains dictionary definitions for BEJ field name resolution
 * and helper functions for dictionary operations.
 */

#include "../include/dictionary.h"
#include <stddef.h>

/**
 * @brief Main dictionary for root-level BEJ fields
 * 
 * Maps field IDs to their names and types for the root structure.
 * Terminated with {255, NULL, 0} sentinel.
 */
BejDictionary main_dictionary[] = {
  {0, "root", BEJ_SET},
  {1, "CapacityMiB", BEJ_INTEGER},
  {2, "DataWidthBits", BEJ_INTEGER},
  {3, "ErrorCorrection", BEJ_STRING},
  {4, "MemoryLocation", BEJ_SET},
  {255, NULL, 0}
};

/**
 * @brief Child dictionary for nested MemoryLocation fields
 * 
 * Maps field IDs to their names and types for the MemoryLocation structure.
 * Terminated with {255, NULL, 0} sentinel.
 */
BejDictionary child_dictionary[] = {
  {1, "Channel", BEJ_INTEGER},
  {2, "Slot", BEJ_INTEGER},
  {255, NULL, 0}
};

/**
 * @brief Gets the appropriate child dictionary for a given parent ID
 * 
 * Returns the child dictionary for nested structures based on parent field ID.
 * Falls back to main_dictionary if no specific child dictionary exists.
 * 
 * @param parent_id ID of the parent field
 * @return Pointer to the appropriate BejDictionary
 */
BejDictionary *bej_get_child_dictionary(uint8_t parent_id)
{
  if (parent_id == 4)
    return child_dictionary;
  
  return main_dictionary;
}

/**
 * @brief Finds a field name in the dictionary by ID
 * 
 * Searches the dictionary for a field with the given ID and optionally
 * returns its type.
 * 
 * @param dict Dictionary to search in
 * @param id Field ID to look up
 * @param type Optional pointer to store the field type (can be NULL)
 * @return Field name if found, NULL otherwise
 */
const char *bej_find_in_dictionary(BejDictionary *dict, uint8_t id, BejType *type)
{
  for (int i = 0; dict[i].name != NULL; i++)
  {
    if (dict[i].id == id)
    {
      if (type) {*type = dict[i].type;}
      return dict[i].name;
    }
  }
  return NULL; 
}
