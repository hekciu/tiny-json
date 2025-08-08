#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>


enum TJ_ERR {
    TJ_OK = 0,
    TJ_MISSING_KEY = 1,
    TJ_PARSE_MISSING_BRACKET = 10,
    TJ_PARSE_UNEXPECTED_BRACKET = 11,
    TJ_PARSE_UNEXPECTED_TOKEN = 12,
    TJ_PARSE_EMPTY_KEY = 13
};


enum TjValueType {
    TJ_STRING = 0,
    TJ_FLOAT = 1,
    TJ_DOUBLE = 2,
    TJ_INT32 = 3,
    TJ_UINT32 = 4
};


struct tj_parsed {
    struct {
        char * key;
        void * value;
        enum TjValueType type;
    } * _keys;
    size_t _elements_length;
};


// #define TJ_CALLBACK int (*ptr)(const char *, const size_t, TjParamType, void *)


enum TJ_ERR tj_parse(const char * data, const size_t size, struct tj_parsed * output);
enum TJ_ERR tj_get_value(const struct tj_parsed * parsed, const char * key, const enum TjValueType type, void * output);
enum TJ_ERR tj_clean(const struct tj_parsed * parsed);


static enum TJ_ERR _tj_parse_block (
    const char * data,
    size_t max_size,
    const char * current_path,
    struct tj_parsed * parsed,
    size_t * char_num_ptr
);



static enum TJ_ERR _tj_parse_string (
    const char * data,
    size_t max_size,
    char ** output_ptr,
    size_t * char_num_ptr
) {
    char * start = NULL;
    size_t output_size = 0;

    for (size_t char_num = 0; char_num < max_size; char_num++) {
        char current_char = *(data + char_num);

        if (start == NULL && current_char == ' ') continue;

        if (start == NULL && current_char == '"') {
            start = data + char_num + 1;
            continue;
        }

        if (start == NULL) return TJ_PARSE_UNEXPECTED_TOKEN;

        if (current_char == '"') {
            *output_ptr = malloc(output_size + 1);
            memcpy(*output_ptr, start, output_size);
            *(*output_ptr + output_size) = '\0';

            *char_num_ptr = char_num;

            return TJ_OK;
        }

        output_size++;
    }

    return TJ_PARSE_UNEXPECTED_TOKEN;
};


static enum TJ_ERR _tj_get_key (
    const char * data,
    size_t max_size,
    const char * current_path,
    char ** output_path_ptr,
    size_t * key_size_ptr
) {
    char * start = NULL;
    size_t key_size = 0;

    for (size_t char_num = 0; char_num < max_size; char_num++) {
        char current_char = *(data + char_num);

        if (start == NULL && current_char == ' ') continue;

        if (start == NULL && current_char == '"') {
            start = data + char_num + 1;
            continue;
        }

        if (start == NULL) return TJ_PARSE_UNEXPECTED_TOKEN;

        if (current_char == '"' && key_size == 0) return TJ_PARSE_EMPTY_KEY;

        if (current_char == '"') {
            size_t current_path_size = strlen(current_path);
            size_t full_key_size = current_path_size + key_size + 1;
            char * full_key = malloc(full_key_size);

            memcpy(full_key, current_path, current_path_size);
            memcpy(full_key + current_path_size, start, key_size);
            *(full_key + full_key_size) = '\0';

            *output_path_ptr = full_key;
            *key_size_ptr = key_size;

            return TJ_OK;
        }

        key_size++;
    }

    return TJ_PARSE_UNEXPECTED_TOKEN; // TODO: maybe better error message?
};


static enum TJ_ERR _tj_parse_value (
    const char * data,
    size_t max_size,
    const char * current_path,
    struct tj_parsed * parsed,
    size_t * char_num_ptr
) {
    bool started = false;
    enum TJ_ERR err;

    for (size_t char_num = 0; char_num < max_size; char_num++) {
        char current_char = *(data + char_num);

        if (current_char == ' ') continue;

        if (!started && current_char == ':') {
            started = true;
            continue;
        }

        if (!started) return TJ_PARSE_UNEXPECTED_TOKEN;

        if (current_char == '"') {
            char * output = NULL;

            size_t parsed_char_num = 0;

            err = _tj_parse_string(data + char_num, max_size - char_num, &output, &parsed_char_num);

            if (err != TJ_OK) return err;

            printf("got key: %s   value: %s\n", current_path, output);
            free(output);

            char_num += parsed_char_num;

            *char_num_ptr = char_num;

            return TJ_OK;
        }

        if (current_char == '{') {
            char * output = NULL;
            size_t parsed_char_num = 0;

            err = _tj_parse_block(
                data + char_num,
                max_size - char_num,
                current_path,
                parsed,
                &parsed_char_num
            );

            if (err != TJ_OK) return err;

            char_num += parsed_char_num;

            *char_num_ptr = char_num;

            return TJ_OK;
        }
    }
};


static enum TJ_ERR _tj_parse_block (
    const char * data,
    size_t max_size,
    const char * current_path,
    struct tj_parsed * parsed, 
    size_t * char_num_ptr
) {
    bool started = false;
    size_t start_position = 0;
    enum TJ_ERR err;
    bool expects_key = false;

    for (size_t char_num = 0; char_num < max_size; char_num++) {
        char current_char = *(data + char_num);

        if (current_char == ' ') continue;

        if (!started && current_char != '{') continue;

        if (!started && current_char == '{') {
            started = true;
            expects_key = true;
            start_position = char_num;
            continue;
        }

        if (current_char == '}' && !expects_key) {
            *char_num_ptr = char_num;
            return TJ_OK;
        }

        if (current_char == '}' && expects_key) {
            return TJ_PARSE_UNEXPECTED_BRACKET;
        }

        if (!expects_key && current_char == ',') {
            expects_key = true;
            continue;
        }

        if (current_char == '"' && expects_key) {
            char * output_path = NULL;
            expects_key = false;

            size_t key_size = 0;

            err = _tj_get_key(
                data + char_num,
                max_size - char_num,
                current_path,
                &output_path,
                &key_size 
            ); 

            // TODO: add key to parsed

            if (err != TJ_OK) return err;

            char_num += key_size + 2;

            size_t value_char_num = 0;

            err = _tj_parse_value(
                data + char_num,
                max_size - char_num,
                output_path,
                parsed,
                &value_char_num
            );

            if (err != TJ_OK) return err;

            char_num += value_char_num;

            continue;
        } 

        return TJ_PARSE_UNEXPECTED_TOKEN;
    }

    return TJ_PARSE_MISSING_BRACKET;
};


enum TJ_ERR tj_parse(const char * data, size_t size, struct tj_parsed * output) {
    if (size <= 0) size = strlen(data);

    size_t current_size = 0;

    return _tj_parse_block(data, size, "", output, &current_size);
};

