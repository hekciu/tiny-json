#include "tiny-json.h"



int main(void) {
    struct tj_parsed output = {};

    const char * data = "{ \"chuj\": \"dupa\", \"aaaa\": \"bbbb\" }";

    enum TJ_ERR err = tj_parse(data, strlen(data), &output);
}
