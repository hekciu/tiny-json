#include <string.h>
#include <stdbool.h>


enum TJ_ERR {
    TJ_OK = 0,
    TJ_MISSING_KEY = 1,
    TJ_PARSE_MISSING_BRACKET = 10,
    TJ_PARSE_UNEXPECTED_BRACKET = 10
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



static enum TJ_ERR _tj_get_key(const char * data, size_t max_size, char ** output_ptr, ) {

};


static enum TJ_ERR _tj_parse_block (
    const char * data,
    size_t max_size,
    size_t * block_size_ptr,
    const char * current_path,
    struct tj_parsed * output
) {
    bool started = false;
    bool currently_processing = false;
    size_t start_position = 0;
    enum TJ_ERR err;

    for (size_t char_num = 0; char_num < max_size; char_num++) {
        char current_char = *(data + char_num);

        if (current_char == ' ') continue;

        if (!started && current_char != '{') continue;

        if (!started && current_char == '{') {
            started = true;
            start_position = char_num;
            continue;
        }

        if (current_char == '}' && !currently_processing) {
            *block_size_ptr = char_num - start_position;
            return TJ_OK;
        }

        if (current_char == '}' && currently_processing) {
            return TJ_PARSE_UNEXPECTED_BRACKET;
        }

        if (current_char == '"') {
            size_t key_start = 0;
            size_t key_end = 0;
            size_t value_start = 0;

            err = _tj_get_key(data + char_num, max_size - char_num, &key, &value_start); // TODO: memory leak

            if (err != TJ_OK) return;
        } else {

        }

        if (current_char == '{') {
            size_t block_size = 0;
            err = _tj_parse_block(data + char_num, max_size - char_num, &block_size);

            if (err != TJ_OK) return err;
        }

        if (current_char == '"') {
            err = _tj_parse_string(data + char_num, max_size - char_num, &block_size);
        }
    }

    return TJ_PARSE_MISSING_BRACKET;
};


enum TJ_ERR tj_parse(const char * data, size_t size, struct tj_parsed * output) {
    if (size <= 0) size = strlen(data);

    size_t current_size = 0;

    return _tj_parse_block(data, size, *current_size);
};

