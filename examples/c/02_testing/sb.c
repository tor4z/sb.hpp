#define SB_IMPLEMENTATION
#include "sb.h"


int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_Target testing = sb_create_elf("testing");
    sb_add_src(&testing, "test.c");

    SB_Target run_test = sb_create_phony(testing.path.str);
    sb_add_dep(&run_test, testing);
    sb_build(&run_test);

    return run_test.status;
}
