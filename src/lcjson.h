//
// Created by marvinle on 2018/12/27 4:31 PM.
//

#ifndef LCJSON_LCJSON_H
#define LCJSON_LCJSON_H

typedef enum {LC_NULL, LC_FALSE, LC_TRUE, LC_NUMBER, LC_STRING, LC_OBJECT} lc_type;

typedef struct {
    lc_type type;
    double n;
} lc_value;

int lc_parse(lc_value*, const char*);

lc_type lc_get_type(const lc_value*);

double lc_get_number(const lc_value*);
enum {
    LC_PARSE_OK = 0 ,
    LC_PARSE_EXPECT_VALUE,
    LC_PARSE_INVALID_VALUE,
    LC_PARSE_ROOT_NOT_SINGULAR,
    LC_PARSE_NUMBER_TOO_BIG
};
#endif //LCJSON_LCJSON_H
