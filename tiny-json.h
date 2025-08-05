

enum TJ_ERR {
    TJ_OK = 0,
    TJ_MISSING_KEY = 1
};


enum TjValueType {
    TJ_STRING = 0,
    TJ_FLOAT = 1,
    TJ_DOUBLE = 2,
    TJ_INT32 = 3,
    TJ_UINT32 = 4
};


struct tj_parsed {
    char * _keys;
    size_t _keys_length;
    void * _values;
};


// #define TJ_CALLBACK int (*ptr)(const char *, const size_t, TjParamType, void *)


enum TJ_ERR tj_parse(const char * data, const size_t size, struct tj_parsed * output);
enum TJ_ERR tj_get_value(const struct tj_parsed * parsed, const char * key, const TjValueType expectedType, void * output);
enum TJ_ERR tj_clean(const struct tj_parsed * parsed);
