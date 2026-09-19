#include <stdio.h>
#define SB_IMPLEMENTATION
#include "sb.h"


int main(int argc, char **argv)
{
    sb_auto_rebuild_self(argc, argv);

    SB_IntList def_vi = {};
    SB_FloatList def_vf = {};
    SB_StringList def_vstr = {};

    bool *b = sb_flag_bool("-f", false, "bool argument for test");
    int *i = sb_flag_int("-int", 42, "Int argument for test");
    float *f = sb_flag_float("-float", 42.7f, "Float argument for test");
    const char *str = sb_flag_string("-str", "whoisudady", "String argument for test");
    SB_IntList *vi = sb_flag_int_v("-vi", &def_vi, "Int list argument for test");
    SB_FloatList *vf = sb_flag_float_v("-vf", &def_vf, "Float list argument for test");
    SB_StringList *vstr = sb_flag_string_v("-vstr", &def_vstr, "String list argument for test");
    bool *help = sb_flag_bool("-h", false, "Show this help info");

    if (!sb_flag_parse(argc, argv) || *help) {
        sb_flag_show_usage();
        return 1;
    }

    printf("The bool is %d\n", *b);
    printf("The int number is %d\n", *i);
    printf("The float number is %f\n", *f);
    printf("The string is %s\n", str);

    printf("vi length: %d\n", vi->count);
    for (int i = 0; i < vi->count; ++i) {
        printf("%d ", vi->list[i]);
    }
    printf("\n");

    printf("vf length: %d\n", vf->count);
    for (int i = 0; i < vf->count; ++i) {
        printf("%f ", vf->list[i]);
    }
    printf("\n");

    printf("vstr length: %d\n", vstr->count);
    for (int i = 0; i < vstr->count; ++i) {
        printf("%s ", vstr->list[i].str);
    }
    printf("\n");

    SB_Target exe = sb_create_elf("03_exe");
    sb_add_src(&exe, "main.c");
    sb_build(&exe);

    return exe.status;
}
