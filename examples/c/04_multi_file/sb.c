#define SB_IMPLEMENTATION
#include "sb.h"

int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_Target exe = sb_create_elf("exe");
    sb_add_srcs(&exe, "main.c", "f.c");
    sb_build(&exe);

    return exe.status;
}
