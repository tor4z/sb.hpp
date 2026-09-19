#define SB_IMPLEMEATION
#include "sb.h"

void test_int_add()
{
    SB_CASE("int", "add");

    SB_ASSERT_EQ(1 + 1, 2);
    SB_ASSERT_EQ(1 + 2, 5);
}

void test_float_add()
{
    SB_CASE("float", "add");

    SB_ASSERT_NEAR(1.0f + 1.0f, 2.0f, 0.00001f);
}

int main()
{
    SB_TEST_CASE(test_int_add());
    SB_TEST_CASE(test_float_add());
    sb_testing_report();
    return 0;
}
