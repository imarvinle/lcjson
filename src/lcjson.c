//
// Created by marvinle on 2018/12/27 4:39 PM.
//

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "lcjson.h"

#define MATCH(context, c)   do { assert(*context->json == (c)); context->json++; } while(0)


typedef struct {
    const char *json;
} lc_context;

/**
 * 匹配空字符
 */

static inline int strncmp(const char* s1, const char* s2, unsigned int n) {
    assert(n >= 0);
    for(int i = 0; i < n ;i++) {
        if(s1[i] != s2[i])
            return -1;
    }
    return 0;
}
/* ws = *(%x20 / %x09 / %x0A / %x0D) */
static void lc_parse_whitespace(lc_context* c) {
    const char* p = c->json;
    while(*p == ' ' || *p == '\n' || *p == '\t' || *p == '\r')
        p++;
    c->json = p;
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

static int lc_parse_value(lc_context *c, lc_value *v){
    switch (*c->json){
        case 'n':  return lc_parse_null(c, v);
        case 't':  return lc_parse_true(c, v);
        case 'f':  return lc_parse_false(c, v);
        case '\0': return LC_PARSE_EXPECT_VALUE;
        default: return LC_PARSE_INVALID_VALUE;
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



