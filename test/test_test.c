#include "unity.h"

void test_test(void)
{
TEST_ASSERT(1 == 1);
}

int main(void)
{
UNITY_BEGIN();
RUN_TEST(test_test);
return UNITY_END();
}