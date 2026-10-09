#define SB_IMPLEMENTATION
#include "sb.h"

int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_Target a = sb_create_object("a.c.o");
    sb_add_flag(&a, "-c");
    sb_add_src(&a, "a.c");

    SB_Target b = sb_create_object("b.c.o");
    sb_add_flag(&b, "-c");
    sb_add_src(&b, "b.c");

    SB_Target ab = sb_create_archive("libab.a");
    sb_add_flag(&ab, "rcs");
    sb_add_obj(&ab, a);
    sb_add_obj(&ab, b);

    SB_Target exe = sb_create_elf("exe");
    sb_add_dep(&exe, ab);
    sb_add_flag(&exe, "-L.");
    sb_add_flag(&exe, "-lab");
    sb_add_src(&exe, "main.c");
    sb_build(&exe);

    return exe.status;
}
