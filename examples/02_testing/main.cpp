#define SB_IMPLEMENTATION
#include "sb.hpp"

SB_CASE(Test, hello)
{
    SB_ASSERT_T(1);
    SB_ASSERT_F(1);
    SB_ASSERT_EQ(1, 0);
    SB_ASSERT_NE(1, 1);
    SB_ASSERT_NEAR(1, 1.2, 0.00001);

    const int arr1[] = {1, 4, 3};
    const int arr2[] = {0, 2, 3};
    SB_ASSERT_ARRAY_EQ(arr1, arr2, 3);

    const int arr21[3][3] = {
        {1, 4, 3},
        {1, 4, 3},
    };
    const int arr22[3][3] = {
        {1, 4, 3},
        {1, 9, 3},
    };
    SB_ASSERT_ARRAY2_NEAR(arr21, arr22, 3, 3, 1.0e-5);

    SB_ASSERT_CSTR_EQ("123", "125");
    SB_ASSERT_CSTR_NE("123", "123");
}

SB_CASE(Test, succ)
{
    SB_ASSERT_EQ(1, 1);
}


int main(int argc, char **argv)
{
    for (sb::BaseTestingCase* c : sb::TestingCases::instance()->cases()) {
        c->body();
        c->report(std::cout);
    }
    sb::TestingCases::instance()->report();
    return 0;
}
