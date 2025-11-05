/**
 * @file test_bej.c
 * @brief Unit tests for BEJ parser
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/bej_parse.h"
#include "../include/dictionary.h"

#define TEST_PASS "\033[0;32m[PASS]\033[0m" /*color green*/
#define TEST_FAIL "\033[0;31m[FAIL]\033[0m" /*color red*/

static int tests_run = 0;
static int tests_passed = 0;

void test_result(const char *test_name, int passed)
{
    tests_run++;
    if (passed) 
    {
        tests_passed++;
        printf("%s %s\n", TEST_PASS, test_name);
    } 
    else 
    {
        printf("%s %s\n", TEST_FAIL, test_name);
    }
}

/* Test bej_read_integer - happy path */
void test_read_integer()
{
    uint8_t data[] = {0x01, 0x03, 0x04, 0x00, 0x00, 0x01, 0x00};
    const uint8_t *ptr = data;
    ptr++;  // skip ID
    
    uint32_t result = bej_read_integer(&ptr);
    test_result("read_integer: standard case", result == 65536);
}

/* Test bej_read_string - happy path */
void test_read_string() 
{
    uint8_t data[] = {0x03, 0x05, 0x05, 'N', 'o', 'E', 'C', 'C'};
    const uint8_t *ptr = data;
    ptr++;  // skip ID
    
    char *result = bej_read_string(&ptr);
    int passed = (result != NULL && strcmp(result, "NoECC") == 0);
    test_result("read_string: standard case", passed);
    free(result);
}

/* Test bej_read_value - happy path */
void test_read_value_integer() 
{
    uint8_t data[] = {0x01, 0x03, 0x01, 0x40};
    const uint8_t *ptr = data;
    
    BejSet *val = bej_read_value(&ptr, main_dictionary);
    int passed = (val != NULL && val->type == BEJ_INTEGER && val->integer_value == 64);
    test_result("read_value: integer", passed);
    bej_free(val);
}

void test_read_value_string() 
{
    uint8_t data[] = {0x03, 0x05, 0x05, 'N', 'o', 'E', 'C', 'C'};
    const uint8_t *ptr = data;
    
    BejSet *val = bej_read_value(&ptr, main_dictionary);
    int passed = (val != NULL && val->type == BEJ_STRING);
    test_result("read_value: string", passed);
    bej_free(val);
}

/* Test bej_read_object - happy path */
void test_parse_complete_structure() 
{
    uint8_t data[] = {
        0x00, 0x00, 0x0B,
        0x01, 0x03, 0x04, 0x00, 0x00, 0x01, 0x00,
        0x02, 0x03, 0x01, 0x40,
        0x03, 0x05, 0x05, 0x4E, 0x6F, 0x45, 0x43, 0x43
    };
    const uint8_t *ptr = data;
    
    BejSet *root = bej_read_value(&ptr, main_dictionary);
    int passed = (root != NULL && root->type == BEJ_SET && 
                  root->object_value.count == 3);
    test_result("parse: complete structure", passed);
    bej_free(root);
}

/* Test dictionary lookup - happy path */
void test_dictionary_lookup() 
{
    const char *name = bej_find_in_dictionary(main_dictionary, 1, NULL);
    test_result("dictionary: lookup by ID", 
                name != NULL && strcmp(name, "CapacityMiB") == 0);
}

int main() 
{
    printf("\n=== BEJ Parser Unit Tests ===\n\n");
    
    test_read_integer();
    test_read_string();
    test_read_value_integer();
    test_read_value_string();
    test_parse_complete_structure();
    test_dictionary_lookup();
    
    printf("\n=== Summary ===\n");
    printf("Passed: %d/%d\n", tests_passed, tests_run);
    
    if (tests_passed == tests_run) 
    {
        printf("\n\033[0;32mAll tests passed!\033[0m\n\n");
        return 0;
    } 
    else
    {
        printf("\n\033[0;31mSome tests failed!\033[0m\n\n");
        return 1;
    }
}
