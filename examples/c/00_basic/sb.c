#define SB_IMPLEMENTATION
#include "sb.h"

int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_Target exe = sb_create_elf("00_exe");
    sb_set_compiler(&exe, "cc");
    sb_add_src(&exe, "main.c");

    SB_Target run_exe = sb_create_phony(exe.path.str);
    sb_add_dep(&run_exe, exe);
    sb_build(&run_exe);

    return run_exe.status;
}
