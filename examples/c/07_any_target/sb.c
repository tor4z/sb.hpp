#define SB_IMPLEMENTATION
#include "sb.h"


int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_Target hello = sb_create_any("hello.txt");
    sb_set_cmd(&hello, "touch");
    sb_add_flag(&hello, SB_TARGET);
    sb_build(&hello);

    return hello.status;
}
