//
// Created by marvinle on 2018/12/27 4:41 PM.
//

#include <stdio.h>
#include <stdio.h>
#include <string.h>
#include "lcjson.h"



static int test_pass = 0;
static int test_count = 0;
static int main_ret = 0;

#define EXPECT_EQ_BASE(equality, expect, actual, format)  \
    do {  \
        test_count++; \
        if(equality)  \
            test_pass++; \
        else {          \
            fprintf(stderr, "%s:%d: expect: " format " actual: " format "\n", __FILE__, __LINE__, expect, actual); \
            main_ret = 1;   \
        }\
    } while(0)


#define EXPECT_EQ_INT(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%d")
#define EXPECT_EQ_DOUBLE(expect, actual) EXPECT_EQ_BASE((expect) == (actual), expect, actual, "%.17g")


#define TEST_NUMBER(expect, json) \
    do{ \
        lc_value v; \
        v.type = LC_FALSE;  \
        EXPECT_EQ_INT(LC_PARSE_OK, lc_parse(&v, json)); \
        EXPECT_EQ_INT(LC_NUMBER, lc_get_type(&v));  \
        EXPECT_EQ_DOUBLE(expect, lc_get_number(&v));    \
    } while(0)


#define TEST_CASE(result, json, typevalue) \
    do{         \
        lc_value v; \
        v.type = LC_FALSE;  \
        EXPECT_EQ_INT(result, lc_parse(&v, json));      \
        EXPECT_EQ_INT(typevalue, lc_get_type(&v));    \
    } while(0)

static void test_parse_null(){
    TEST_CASE(LC_PARSE_OK, "null", LC_NULL);
}


static void test_parse_root_not_singulare(){
     TEST_CASE(LC_PARSE_ROOT_NOT_SINGULAR, "null lcjson", LC_NULL);
}

static void test_parse_true(){
      TEST_CASE(LC_PARSE_OK, "true", LC_TRUE);
}

static void test_parse_invalid_value(){
        // 无效的null false等
        TEST_CASE(LC_PARSE_INVALID_VALUE, "nul", LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "?", LC_NULL);

        //无效数字
        TEST_CASE(LC_PARSE_INVALID_VALUE, "+0", LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "+1",LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "00", LC_NULL);   // 不能以0开头后接数字
        TEST_CASE(LC_PARSE_INVALID_VALUE, "00", LC_NULL);   // 不能以0开头后接数字
        TEST_CASE(LC_PARSE_INVALID_VALUE, "00.11", LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, ".123",LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "1.",LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "INF",LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "inf",LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "NAN",LC_NULL);
        TEST_CASE(LC_PARSE_INVALID_VALUE, "nan",LC_NULL);
}

static void test_parse_expect_value() {
        TEST_CASE(LC_PARSE_EXPECT_VALUE, "", LC_NULL);
        TEST_CASE(LC_PARSE_EXPECT_VALUE, " ", LC_NULL);
}

static void test_parse_false(){
    TEST_CASE(LC_PARSE_OK, "false", LC_FALSE);
}


#define TEST_NUMBER(expect, json) \
    do{ \
        lc_value v; \
        v.type = LC_FALSE;  \
        EXPECT_EQ_INT(LC_PARSE_OK, lc_parse(&v, json)); \
        EXPECT_EQ_INT(LC_NUMBER, lc_get_type(&v));  \
        EXPECT_EQ_DOUBLE(expect, lc_get_number(&v));    \
    } while(0)

static void test_parse_number() {
        TEST_NUMBER(0.0, "0");
        TEST_NUMBER(0.0, "-0");
        TEST_NUMBER(0.0, "-0.0");
        TEST_NUMBER(1.0, "1");
        TEST_NUMBER(-1.0, "-1");
        TEST_NUMBER(1.5, "1.5");
        TEST_NUMBER(-1.5, "-1.5");
        TEST_NUMBER(3.1416, "3.1416");
        TEST_NUMBER(1E10, "1E10");
        TEST_NUMBER(1e10, "1e10");
        TEST_NUMBER(1E+10, "1E+10");
        TEST_NUMBER(1E-10, "1E-10");
        TEST_NUMBER(-1E10, "-1E10");
        TEST_NUMBER(-1e10, "-1e10");
        TEST_NUMBER(-1E+10, "-1E+10");
        TEST_NUMBER(-1E-10, "-1E-10");
        TEST_NUMBER(1.234E+10, "1.234E+10");
        TEST_NUMBER(1.234E-10, "1.234E-10");
        TEST_NUMBER(0.0, "1e-10000");
}

static void test_parse() {
    test_parse_expect_value();
    test_parse_invalid_value();
    test_parse_null();
    test_parse_root_not_singulare();
    test_parse_false();
    test_parse_true();
    test_parse_number();
}

int main() {
        test_parse();
        printf("%d/%d (%3.2f%%) passed\n", test_pass, test_count, test_pass * 100.0 / test_count);
        return main_ret;
}