//
// Created by marvinle on 2018/12/27 4:39 PM.
//

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <errno.h>
#include "lcjson.h"

#define MATCH(context, c)   do { assert(*context->json == (c)); context->json++; } while(0)

#define v2

typedef struct {
    const char *json;
} lc_context;

/**
 * 匹配空字符
 */
/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void lc_parse_whitespace(lc_context* c) {
    const char* p = c->json;
    while(*p == ' ' || *p == '\n' || *p == '\t' || *p == '\r')
        p++;
    c->json = p;
}


/* 版本1 解析字面量*/
#ifdef v1
static inline int strncmp(const char* s1, const char* s2, unsigned int n) {
    assert(n >= 0);
    for(int i = 0; i < n ;i++) {
        if(s1[i] != s2[i])
            return -1;
    }
    return 0;
}

/* null = "null" */
static int lc_parse_null(lc_context *c, lc_value *v) {
    MATCH(c, 'n');
    if(strncmp(c->json, "ull", 3) != 0)
        return LC_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LC_NULL;
    return LC_PARSE_OK;
}


/* true = "true" */
static int lc_parse_true(lc_context *c, lc_value *v) {
    MATCH(c, 't');
    if(strncmp(c->json, "rue", 3) != 0)
        return LC_PARSE_INVALID_VALUE;
    c->json += 3;
    v->type = LC_TRUE;
    return LC_PARSE_OK;
}

/* false = 'false' */
static int lc_parse_false(lc_context *c, lc_value *v){
    MATCH(c, 'f');
    if(strncmp(c->json, "alse", 4) != 0)
        return LC_PARSE_INVALID_VALUE;
    c->json += 4;
    v->type = LC_FALSE;
    return LC_PARSE_OK;
}
#else

static int lc_parse_literal(lc_context *c, lc_value *v, const char* literal, lc_type type){
    MATCH(c, literal[0]);
    size_t i;
    for(i; literal[1+i]; i++){
        if(c->json[i] != literal[i+1])
            return LC_PARSE_INVALID_VALUE;
    }
    c->json += i;
    v->type = type;
    return LC_PARSE_OK;
}
#endif


#define ISDIGIT(ch)         ((ch) >= '0' && (ch) <= '9')
#define ISDIGIT1TO9(ch)     ((ch) >= '1' && (ch) <= '9')
/*
 *  json中数字的EBNF
    number = [ "-" ] <int> [ <frac> ] [ <exp> ]
    int = "0" | digit1-9 {digit}
    frac = "." <digit> {digit}       #  小数点后至少一个数字
    exp = ("e" | "E") ["-" | "+"] <digit> {digit}
 *  这里不再递归下降解析数字，而是直接使用标准库 strtod()
 */
static int lc_parse_number(lc_context *c, lc_value *v){
    const char *p = c->json;
    if(*p == '-') p++;
    if(*p == '0') {
        p++;
        if(ISDIGIT(*p))
            return LC_PARSE_INVALID_VALUE;
    } else {
        if(!ISDIGIT1TO9(*p)) return LC_PARSE_INVALID_VALUE;
        for(p++; ISDIGIT(*p); p++);
    }

    if(*p == '.'){
        p++;
        if(!ISDIGIT(*p))    return LC_PARSE_INVALID_VALUE;
        for(p++; ISDIGIT(*p); p++);
    }
    if(*p == 'e' || *p == 'E'){
        p++;
        if(*p == '+' || *p == '-')  p++;
        if(!ISDIGIT(*p)) return LC_PARSE_INVALID_VALUE;
        for(p++; ISDIGIT(*p); p++);
    }
    errno = 0;
    v->n = strtod(c->json, NULL);

    if(errno == ERANGE && (v->n == HUGE_VAL || v->n == -HUGE_VAL)){
        return LC_PARSE_NUMBER_TOO_BIG;
    }
    c->json = p;
    v->type = LC_NUMBER;
    return LC_PARSE_OK;
}

static int lc_parse_value(lc_context *c, lc_value *v){
    switch (*c->json){
        case 'n':  return lc_parse_literal(c, v, "null", LC_NULL);
        case 't':  return lc_parse_literal(c, v, "true", LC_TRUE);
        case 'f':  return lc_parse_literal(c, v, "false", LC_FALSE);
        case '\0': return LC_PARSE_EXPECT_VALUE;
        default: return lc_parse_number(c, v);
    }
}


int lc_parse(lc_value* v, const char* json){
    int ret;
    lc_context context;
    assert(v != NULL);

    context.json = json;
    v->type = LC_NULL;
    lc_parse_whitespace(&context);

    if((ret = lc_parse_value(&context, v)) == LC_PARSE_OK){
        lc_parse_whitespace(&context);
        if(*context.json != '\0')
            ret = LC_PARSE_ROOT_NOT_SINGULAR;
    }
    return ret;
}


/**
 *
 * @param v
 * @return 值类型 lc_type枚举
 */
lc_type lc_get_type(const lc_value* v){
    assert(v != NULL);
    return v->type;
}

double lc_get_number(const lc_value* v){
    assert(v != NULL || v->type == LC_NUMBER);
    return v->n;
}



