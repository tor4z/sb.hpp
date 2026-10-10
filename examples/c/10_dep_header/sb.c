#define SB_IMPLEMENTATION
#include "sb.h"

int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_Target exe = sb_create_elf("exe");
    sb_set_compiler(&exe, "cc");
    sb_add_src(&exe, "main.c");
    sb_add_src(&exe, "f.c");
    sb_build(&exe);

    return 0;
}
