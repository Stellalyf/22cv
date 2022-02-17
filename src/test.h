#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#define EPS .002

#ifdef UWIMGDLL_EXPORTS
#define UWIMGDLL_API __declspec(dllexport)
#else
#define UWIMGDLL_API __declspec(dllimport)
#endif

extern int tests_total;
extern int tests_fail;
#define TEST(EX) do { ++tests_total; if(!(EX)) {\
    fprintf(stderr, "failed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__); \
    ++tests_fail; }else{fprintf(stderr, "passed: [%s] testing [%s] in %s, line %d\n", __FUNCTION__, #EX, __FILE__, __LINE__);}} while (0)

void UWIMGDLL_API run_tests();
void UWIMGDLL_API test_hw0();
void UWIMGDLL_API test_hw1();
void UWIMGDLL_API test_hw2();
void UWIMGDLL_API test_hw3();
void UWIMGDLL_API test_hw4();
void UWIMGDLL_API test_hw5();
#endif
