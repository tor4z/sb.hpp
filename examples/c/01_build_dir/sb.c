#define SB_IMPLEMENTATION
#include "sb.h"

int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);
    sb_set_build_dir("build");

    SB_Target exe = sb_create_elf("00_exe");
    sb_set_compiler(&exe, "cc");
    sb_add_src(&exe, "main.c");
    sb_build(&exe);
    return exe.status;
}
