#include "../include/unity.h"

// Set up before each test
void setUp(void)
{
    ;
}
// Clean up after each test
void tearDown(void)
{
    // printf("cleaning up after test\n");
}

void test_Addition(void)
{
    TEST_ASSERT_EQUAL_INT(4, 2 + 2);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_Addition);
    return UNITY_END();
}
