

enum TJ_ERR {
    TJ_OK = 0,
    TJ_ERR = 1
};


enum TjParamType {
    TJ_STRING = 0,
    TJ_FLOAT = 1,
    TJ_DOUBLE = 2,
    TJ_INT32 = 3,
    TJ_UINT32 = 4
};



#define TJ_CALLBACK int (*ptr)(const char *, const size_t, TjParamType, void *)


enum TJ_ERR tj_parse(const char * data, const size_t size, TJ_CALLBACK);
