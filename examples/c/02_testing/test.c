#define SB_IMPLEMENTATION
#include "sb.h"

SB_DEF_CASE(integer_number, add)
{

    SB_ASSERT_EQ(1 + 1, 2);
    SB_ASSERT_EQ(1 + 2, 5);
}

SB_DEF_CASE(floating_number, add)
{
    SB_ASSERT_NEAR(1.0f + 1.0f, 2.0f, 0.00001f);
}

int main()
{
    SB_TEST_CASE(integer_number, add);
    SB_TEST_CASE(floating_number, add);
    return sb_testing_summary();
}
