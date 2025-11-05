/**
 * @file main.c
 * @brief BEJ parser demonstration program
 * 
 * This program demonstrates the BEJ parser by:
 * 1. Creating a sample BEJ binary file
 * 2. Loading and parsing the BEJ data
 * 3. Converting the parsed data to JSON format
 */

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "../include/bej_parse.h"

/**
 * @brief Sample BEJ data representing a memory module structure
 * 
 * This data encodes the following JSON-like structure:
 * {
 *   "CapacityMiB": 65536,
 *   "DataWidthBits": 64,
 *   "ErrorCorrection": "NoECC",
 *   "MemoryLocation": {
 *     "Channel": 0,
 *     "Slot": 0
 *   }
 * }
 */
uint8_t bej_data[] = {
  
  /* Root SET */
  0x00,                 // ID=0 (root)
  0x00,                 // TYPE=BEJ_SET
  0x0B,                 // Length of SET in bytes
  
  /* CapacityMiB (id=1, integer) */
  0x01,                 // ID=1
  0x03,                 // TYPE=BEJ_INTEGER
  0x04,                 // Length = 4 bytes
  0x00, 0x00, 0x01, 0x00, // 65536 in little-endian
  
  /* DataWidthBits (id=2, integer) */
  0x02,                 // ID=2
  0x03,                 // TYPE=BEJ_INTEGER
  0x01,                 // Length = 1 byte
  0x40,                 // 64
  
  /* ErrorCorrection (id=3, string) */
  0x03,                 // ID=3
  0x05,                 // TYPE=BEJ_STRING
  0x05,                 // Length = 5 bytes
  0x4E, 0x6F, 0x45, 0x43, 0x43, // "NoECC"
  
  /* MemoryLocation (id=4, set) */
  0x04,                 // ID=4
  0x00,                 // TYPE=BEJ_SET
  0x02,                 // SET length in bytes
  
  /* Channel (id=1, integer) */
  0x01,               // ID=1
  0x03,               // TYPE=BEJ_INTEGER
  0x01,               // Length = 1 byte
  0x00,               // Value = 0
    
  /* Slot (id=2, integer) */
  0x02,               // ID=2
  0x03,               // TYPE=BEJ_INTEGER
  0x01,               // Length = 1 byte
  0x00                // Value = 0
};

/**
 * @brief Main program entry point
 * 
 * Demonstrates the complete BEJ parsing workflow:
 * - Writes sample BEJ data to a binary file
 * - Loads the binary file
 * - Parses the BEJ data structure
 * - Converts to JSON format
 * - Cleans up allocated memory
 * 
 * @return 0 on success, 1 on error
 */
int main()
{ 
  /* Write data to bej.bin */
  FILE *f = fopen("../bin/bej.bin", "wb");
  fwrite(bej_data, 1, sizeof(bej_data), f);
  fclose(f);
  
  /* Load data from bej.bin */
  uint8_t *bin_data = bej_load_file("../bin/bej.bin");
  if(!bin_data) 
  {
    printf("File not found\n");
    return 1;
  }
  
  /* Skip first three BEJ bytes [index][type][len] */
  const uint8_t *data_ptr = bin_data;
  
  /* Parse BEJ data */
  BejSet *root = bej_read_value(&data_ptr, main_dictionary);
  
  /* Convert to JSON */
  bej_to_json_file(root, "../json/result.json");
  
  /* Free allocated memory */
  free(bin_data);
  bej_free(root);
  
  return 0;
}
