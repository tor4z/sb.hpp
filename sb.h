#ifndef SB_H_
#define SB_H_

#include <stdbool.h>


#ifndef SB_MAX_ARGS
#   define SB_MAX_ARGS 128
#endif // SB_MAX_ARGS

#define SB_TARGET "@"

typedef enum SB_TargetType
{
    SB_ANY = 0,
    SB_ELF,
    SB_OBJECT,
    SB_PHONY,
    SB_ARCHIVE,
} SB_TargetType; // enum SB_TargetType

typedef struct SB_Target SB_Target;

typedef struct SB_String {
    char *str;
    int count;
    int capacity;
} SB_String; // struct SB_String

typedef struct SB_StringView {
    char *str;
    int count;
} SB_StringView; // SB_StringView

typedef struct SB_StringList {
    SB_String *list;
    int count;
    int capacity;
} SB_StringList; // struct SB_StringList

typedef struct SB_IntList {
    int *list;
    int count;
    int capacity;
} SB_IntList; // struct SB_IntList

typedef struct SB_BoolList {
    bool *list;
    int count;
    int capacity;
} SB_BoolList; // struct SB_BoolList

typedef struct SB_FloatList {
    float *list;
    int count;
    int capacity;
} SB_FloatList; // struct SB_FloatList

typedef struct SB_TargetList {
    SB_Target *list;
    int count;
    int capacity;
} SB_TargetList; // struct SB_TargetList

typedef struct SB_Target {
    SB_String name;
    SB_String path;
    SB_String cmd;
    SB_String obj_d_path;
    SB_StringList srcs;
    SB_StringList flags;
    SB_TargetList deps;
    SB_StringList dep_files;
    SB_TargetList objs;
    int status;
    bool always_build;
    SB_TargetType target_type;
} SB_Target;

void sb_set_build_dir(const char *dir);
const char *sb_build_dir();

SB_Target sb_create_any(const char *name);
SB_Target sb_create_elf(const char *name);
SB_Target sb_create_object(const char *name);
SB_Target sb_create_phony(const char *name);
SB_Target sb_create_archive(const char *name);

bool sb_set_cmd(SB_Target *target, const char *cmd);
bool sb_set_compiler(SB_Target *target, const char *compiler);
bool sb_set_linker(SB_Target *target, const char *linker);
bool sb_set_ar(SB_Target *target, const char *ar);

bool sb_add_src(SB_Target *target, const char *src);
bool sb_add_src_s(SB_Target *target, SB_String src);
bool sb_add_srcs(SB_Target *target, SB_StringList srcs);
bool sb_add_srcs_v(SB_Target *target, const char **srcs, int count);

bool sb_add_flag(SB_Target *target, const char *flag);
bool sb_add_flag_pair(SB_Target *target, const char *flag1, const char *flag2);
bool sb_add_flag_s(SB_Target *target, SB_String flag);
bool sb_add_flags(SB_Target *target, SB_StringList flags);
bool sb_add_flags_v(SB_Target *target, const char **flags, int count);

bool sb_add_dep_file(SB_Target *target, const char *dep);
bool sb_add_dep_file_s(SB_Target *target, SB_String dep);
bool sb_add_dep_files(SB_Target *target, SB_StringList deps);
bool sb_add_dep_files_v(SB_Target *target, const char **deps, int count);

bool sb_add_dep(SB_Target *target, SB_Target dep);
bool sb_add_deps(SB_Target *target, SB_TargetList deps);
bool sb_add_deps_v(SB_Target *target, const SB_Target *deps, int count);

bool sb_add_obj(SB_Target *target, SB_Target obj);
bool sb_add_objs(SB_Target *target, SB_TargetList objs);
bool sb_add_objs_v(SB_Target *target, const SB_Target *objs, int count);

bool sb_always_build(SB_Target *target, bool sure);
bool sb_build(SB_Target *target);

int sb_command(const char *cmd_str);
int sb_command_v(const char **cmd, int count);
int sb_command_p(const char *first, ...);
SB_String sb_shell(const char *cmd_str);
SB_String sb_shell_v(const char **cmd, int count);
SB_String sb_shell_p(const char *first, ...);

#define sb_auto_rebuild_self(argc, argv) sb_auto_rebuild_self__(argc, argv, __FILE__)
bool sb_auto_rebuild_self__(int argc, char **argv, const char *src);
void sb_self_add_dep(const char *file);

SB_String sb_file_slurp(const char *filename);

int sb_string_append_substr(SB_String *string, const char *start, const char *end);
int sb_string_append_char(SB_String *string, char c);
int sb_string_append_cstr(SB_String *string, const char *cstr);
SB_String sb_string_copy(const SB_String *src);
SB_String sb_string_copy_sv(const SB_StringView *src);
void sb_string_clean(SB_String *string);
void sb_string_free(SB_String *string);
bool sb_string_empty(const SB_String *string);
bool sb_string_reserve(SB_String *string, int count);
bool sb_string_resize(SB_String *string, int count);

#define log_info(...)   printf("[MSG]: "__VA_ARGS__)
#define log_error(...)  fprintf(stderr, "[ERR]: "__VA_ARGS__)

// ----- flags ------

const char *sb_flag_string(const char *key, const char *default_val, const char *help);
int *sb_flag_int(const char *key, int default_val, const char *help);
float *sb_flag_float(const char *key, float default_val, const char *help);
bool *sb_flag_bool(const char *key, bool default_val, const char *help);
SB_StringList *sb_flag_string_v(const char *key, const SB_StringList *default_val, const char *help);
SB_IntList *sb_flag_int_v(const char *key, const SB_IntList *default_val, const char *help);
SB_FloatList *sb_flag_float_v(const char *key, const SB_FloatList *default_val, const char *help);
bool sb_flag_parse(int argc, char **argv);
void sb_flag_show_usage();


// ------ unit test suit ---

#define SB_TESTING_MODULE_NAME_LEN 47

typedef struct SB_TestingCase {
    SB_String module;
    SB_String name;
    int failed_assert;
    int success_assert;
} SB_TestingCase; // struct SB_TestingCase

typedef struct SB_TestingCaseList {
    SB_TestingCase *list;
    int count;
    int capacity;
} SB_TestingCaseList; // struct SB_TestingCaseList

typedef struct SB_Testing {
    SB_TestingCaseList cases;
} SB_Testing; // struct SB_Testing

void sb_testing_assert_success(SB_TestingCase *sb_case);
void sb_testing_assert_failed(SB_TestingCase *sb_case, const char *filename, int line, const char *checking);
void sb_testing_case_report(SB_TestingCase *sb_case);
int sb_testing_summary();

#define SB_ABS(x) ((x) > 0 ? (x) : -(x))
#define SB_FLT_NEAR(x, y, err) (SB_ABS((x) - (y)) < err)

extern SB_Testing __sb_testing;

#define SB_DECL_CASE(module, name)                                                                  \
    void sb_testing_case_##module##_##name(SB_TestingCase *__sb_case)

#define SB_DEF_CASE(module, name)                                                                   \
    void sb_testing_case_##module##_##name(SB_TestingCase *__sb_case)

#define SB_TEST_CASE(m, n)                                                                          \
    do {                                                                                            \
        SB_DA_ADD(__sb_testing.cases);                                                              \
        SB_TestingCase *sb_case = SB_DA_LAST(__sb_testing.cases);                                   \
        sb_string_append_cstr(&sb_case->module, #m);                                                \
        sb_string_append_cstr(&sb_case->name, #n);                                                  \
        sb_testing_case_##m##_##n(sb_case);                                                         \
        sb_testing_case_report(sb_case);                                                            \
    } while (0)

#define SB_ASSERT_T(x)                                                                              \
    do {                                                                                            \
        if (x) {                                                                                    \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_T");                    \
            printf(#x" != true\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_F(x)                                                                              \
    do {                                                                                            \
        if (!(x)) {                                                                                 \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_F");                    \
            printf(#x" != false\n");                                                                \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_EQ(x, y)                                                                          \
    do {                                                                                            \
        if ((x) == (y)) {                                                                           \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_EQ");                   \
            printf(#x" != "#y"\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_NE(x, y)                                                                          \
    do {                                                                                            \
        if ((x) != (y)) {                                                                           \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_NE");                   \
            printf(#x" == "#y"\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_GT(x, y)                                                                          \
    do {                                                                                            \
        if ((x) > (y)) {                                                                            \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_GT");                   \
            printf(#x" <= "#y"\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_GE(x, y)                                                                          \
    do {                                                                                            \
        if ((x) >= (y)) {                                                                           \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_GE");                   \
            printf(#x" < "#y"\n");                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_LT(x, y)                                                                          \
    do {                                                                                            \
        if ((x) < (y)) {                                                                            \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_NT");                   \
            printf(#x" >= "#y"\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_LE(x, y)                                                                          \
    do {                                                                                            \
        if ((x) <= (y)) {                                                                           \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_LE");                   \
            printf(#x" > "#y"\n");                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY_EQ(x, y, n)                                                                 \
    do {                                                                                            \
        bool has_failed_case = false;                                                               \
        for (int i = 0; i < n; ++i) {                                                               \
            if ((x)[i] != (y)[i]) {                                                                 \
                if (!has_failed_case) {                                                             \
                    sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_ARRAY_EQ");     \
                }                                                                                   \
                if (has_failed_case) {                                                              \
                    printf(", ");                                                                   \
                }                                                                                   \
                printf(#x"[%d] != "#y"[%d]", i, i);                                                 \
                has_failed_case = true;                                                             \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            printf("\n");                                                                           \
        } else {                                                                                    \
            sb_testing_assert_success(__sb_case);                                                   \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY_NEAR(x, y, n, abs_err)                                                      \
    do {                                                                                            \
        bool has_failed_case = false;                                                               \
        for (int i = 0; i < n; ++i) {                                                               \
            if (!SB_FLT_NEAR((x)[i], (y)[i], abs_err)) {                                            \
                if (!has_failed_case) {                                                             \
                    sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_ARRAY_NEAR");   \
                }                                                                                   \
                if (has_failed_case) {                                                              \
                    printf(", ");                                                                   \
                }                                                                                   \
                printf(#x "[%d] != " #y "[%d]", i, i);                                              \
                has_failed_case = true;                                                             \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            printf("\n");                                                                           \
        } else {                                                                                    \
            sb_testing_assert_success(__sb_case);                                                   \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY2_EQ(x, y, m, n)                                                             \
    do {                                                                                            \
        bool has_failed_case = false;                                                               \
        for (int i = 0; i < n; ++i) {                                                               \
            for (int j = 0; j < m; ++j) {                                                           \
                if ((x)[i][j] != (y)[i][j]) {                                                       \
                    if (!has_failed_case) {                                                         \
                        sb_testing_assert_failed(__sb_case, __FILE__, __LINE__,                     \
                            "ASSERT_ARRAY2_EQ");                                                    \
                    }                                                                               \
                    if (has_failed_case) {                                                          \
                        printf(", ");                                                               \
                    }                                                                               \
                    printf(#x "[%d][%d] != " #y "[%d][%d]", i, j, i, j);                            \
                    has_failed_case = true;                                                         \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            printf("\n");                                                                           \
        } else {                                                                                    \
            sb_testing_assert_success(__sb_case);                                                   \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY2_NEAR(x, y, m, n, abs_err)                                                  \
    do {                                                                                            \
        bool has_failed_case = false;                                                               \
        for (int i = 0; i < n; ++i) {                                                               \
            for (int j = 0; j < m; ++j) {                                                           \
                if (!SB_FLT_NEAR((x)[i][j], (y)[i][j], abs_err)) {                                  \
                    if (!has_failed_case) {                                                         \
                        sb_testing_assert_failed(__sb_case, __FILE__, __LINE__,                     \
                            "ASSERT_ARRAY2_NEAR");                                                  \
                    }                                                                               \
                    if (has_failed_case) {                                                          \
                        printf(", ");                                                               \
                    }                                                                               \
                    printf(#x "[%d][%d] != " #y "[%d][%d]", i, j, i, j);                            \
                    has_failed_case = true;                                                         \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            printf("\n");                                                                           \
        } else {                                                                                    \
            sb_testing_assert_success(__sb_case);                                                   \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_NEAR(x, y, abs_err)                                                               \
    do {                                                                                            \
        if (SB_FLT_NEAR(x, y, abs_err)) {                                                           \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_NEAR");                 \
            printf(#x" != "#y"\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_NNEAR(x, y, abs_err)                                                              \
    do {                                                                                            \
        if (!SB_FLT_NEAR(x, y, abs_err)) {                                                          \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_NNEAR");                \
            peinrf(#x" == "#y"\n");                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_CSTR_EQ(x, y)                                                                     \
    do {                                                                                            \
        if ((x) == NULL && (y) == NULL) {                                                           \
            sb_testing_assert_success(__sb_case);                                                   \
        } else {                                                                                    \
            if ((x) == NULL) {                                                                      \
                sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_CSTR_EQ");          \
                printf(#x" is NULL while y not\n");                                                 \
            } else if ((y) == NULL) {                                                               \
                sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_CSTR_EQ");          \
                printf(#y" is NULL while x not\n");                                                 \
            } else {                                                                                \
                int i = 0;                                                                          \
                bool failed = false;                                                                \
                while (!failed && (x)[i] && (y)[i]) {                                               \
                    if ((x)[i] != (y)[i]) {                                                         \
                        failed = true;                                                              \
                        sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_CSTR_EQ");  \
                        printf(#x" != "#y", the first not equal occurred at "#x                     \
                            "[%d](%c) != "#y"[%d](%c)\n", i, i, (x)[i], (y)[i]);                    \
                    }                                                                               \
                    ++i;                                                                            \
                }                                                                                   \
                if (!failed) {                                                                      \
                    if ((x)[i] == '\0' && (y)[i] == '\0') {                                         \
                        sb_testing_assert_success(__sb_case);                                       \
                    } else {                                                                        \
                        sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_CSTR_EQ");  \
                        printf(#x" and "#y" has different length\n");                               \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_CSTR_NE(x, y)                                                                     \
    do {                                                                                            \
        if ((x) == NULL && (y) == NULL) {                                                           \
            sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_CSTR_NE");              \
            printf("both "#x" and "#y" are NULL\n");                                                \
        } else {                                                                                    \
            if ((x) == NULL || (y) == NULL) {                                                       \
                sb_testing_assert_success(__sb_case);                                               \
            } else {                                                                                \
                int i = 0;                                                                          \
                bool success = false;                                                               \
                while (!success && (x)[i] && (y)[i]) {                                              \
                    if ((x)[i] != (y)[i]) {                                                         \
                        success = true;                                                             \
                        sb_testing_assert_success(__sb_case);                                       \
                    }                                                                               \
                    ++i;                                                                            \
                }                                                                                   \
                if (!success) {                                                                     \
                    if ((x)[i] == '\0' && (y)[i] == '\0') {                                         \
                        sb_testing_assert_failed(__sb_case, __FILE__, __LINE__, "ASSERT_CSTR_NE");  \
                        printf(#x" == "#y"\n");                                                     \
                    } else {                                                                        \
                        sb_testing_assert_success(__sb_case);                                       \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
    } while (0)

#define SB_DA_ADD(da)                                                                               \
    do {                                                                                            \
        if (!(da).list) {                                                                           \
            /* init da */                                                                           \
            (da).capacity = 8;                                                                      \
            int nbytes = sizeof(*(da).list) * (da).capacity;                                        \
            (da).list = malloc(nbytes);                                                             \
            memset((da).list, 0, nbytes);                                                           \
        } else {                                                                                    \
            if ((da).count == (da).capacity) {                                                      \
                (da).capacity *= 2;                                                                 \
                int nbytes = sizeof(*(da).list) * (da).capacity;                                    \
                int nbytes_used = sizeof(*(da).list) * (da).count;                                  \
                (da).list = realloc((da).list, nbytes);                                             \
                memset((da).list + nbytes_used, 0, nbytes - nbytes_used);                           \
            }                                                                                       \
        }                                                                                           \
        ++(da).count;                                                                               \
    } while(0)

#define SB_DA_LAST(da) (&((da).list[(da).count - 1]))

#define SB_DA_APPEND(da, x)                                                                         \
    do {                                                                                            \
        if (!(da).list) {                                                                           \
            /* init da */                                                                           \
            (da).capacity = 8;                                                                      \
            int nbytes = sizeof(*(da).list) * (da).capacity;                                        \
            (da).list = malloc(nbytes);                                                             \
            memset((da).list, 0, nbytes);                                                           \
        } else {                                                                                    \
            if ((da).count == (da).capacity) {                                                      \
                (da).capacity *= 2;                                                                 \
                int nbytes = sizeof(*(da).list) * (da).capacity;                                    \
                int nbytes_used = sizeof(*(da).list) * (da).count;                                  \
                (da).list = realloc((da).list, nbytes);                                             \
                memset((da).list + nbytes_used, 0, nbytes - nbytes_used);                           \
            }                                                                                       \
        }                                                                                           \
        (da).list[(da).count] = x;                                                                  \
        ++(da).count;                                                                               \
    } while(0)

#define SB_DA_APPEND_MANY(da, xs, n)                                                                \
    do {                                                                                            \
        if (!(da).list) {                                                                           \
            /* init da */                                                                           \
            (da).capacity = (n / 8 + 1) * 8;                                                        \
            int nbytes = sizeof(*(da).list) * (da).capacity;                                        \
            (da).list = malloc(nbytes);                                                             \
            memset((da).list, 0, nbytes);                                                           \
        } else {                                                                                    \
            if ((da).count == (da).capacity) {                                                      \
                (da).capacity *= 2;                                                                 \
                int nbytes = sizeof(*(da).list) * (da).capacity;                                    \
                int nbytes_used = sizeof(*(da).list) * (da).count;                                  \
                (da).list = realloc((da).list, nbytes);                                             \
                memset((da).list + nbytes_used, 0, nbytes - nbytes_used);                           \
            }                                                                                       \
        }                                                                                           \
        for (int i = 0; i < n; ++i) {                                                               \
            (da).list[(da).count] = xs[i];                                                          \
            ++(da).count;                                                                           \
        }                                                                                           \
    } while(0)

#define SB_DA_RESET(da)                                                                             \
    do {                                                                                            \
        (da).count = 0;                                                                             \
    } while(0)

#define SB_DA_FREE(da)                                                                              \
    do {                                                                                            \
        if ((da).list) {                                                                            \
            free((da.list));                                                                        \
        }                                                                                           \
        (da).list = NULL;                                                                           \
        (da).count = 0;                                                                             \
        (da).capacity = 0;                                                                          \
    } while(0)

#define SB_ARRAY_LEN(arr) (sizeof(arr) / sizeof(*(arr)))

#endif // SB_H_


#ifdef SB_IMPLEMENTATION

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <ctype.h>
#include <sys/wait.h>
#include <sys/stat.h>

#ifdef __APPLE__
#   include <mach-o/dyld.h>
#endif

#define SB_SEC_TO_NS        (1000 * 1000 * 1000)
#define TIMESPEC_TO_NS(ts)  ((ts).tv_nsec + (ts).tv_sec * SB_SEC_TO_NS)

#define SB_TERM_COLOR_B_RED_S    "\033[31m"
#define SB_TERM_COLOR_B_GREEN_S  "\033[32m"
#define SB_TERM_COLOR_B_WHITE_S  "\033[1;37m"
#define SB_TERM_COLOR_E          "\033[0m"
#define SB_TERM_COLOR_SUCC_S     SB_TERM_COLOR_B_GREEN_S
#define SB_TERM_COLOR_FAIL_S     SB_TERM_COLOR_B_RED_S
#define SB_TERM_COLOR_HL_S       SB_TERM_COLOR_B_WHITE_S

#ifdef __APPLE__
#   define FILE_MTIME_NS(st)   TIMESPEC_TO_NS(st.st_mtimespec)
#else
#   define FILE_MTIME_NS(st)   TIMESPEC_TO_NS(st.st_mtim)
#endif

static SB_StringList sb_self_dep_files__ = {};
static const char *sb_build_dir__ = "./";
static const char *sb_default_compiler__ = "cc";
static const char *sb_default_ar__ = "ar";
static const char *sb_excluded_flags[] = {
    "-o", "-c"
};

const char *sb_dir();
bool mkdir_if_not_exists(const char* dir);
int copy_file(const char *src, const char *dest);
SB_String *sb_string_trim_left(SB_String *str);
SB_String *sb_string_trim_right(SB_String *str);
SB_String *sb_string_trim(SB_String *str);
SB_String sb_path_join(const char *parent, const char *child);
SB_String sb_path_dirname(const char *path);
SB_StringList sb_split_cstr(const char* str);
bool sb_should_compile(const char *src, const char *target);
void sb_print_command(const char **cmd, int count);

bool sb_auto_rebuild_self__(int argc, char **argv, const char *src)
{
    if (!argv || argc == 0) {
        log_error("Invalid argc argv.\n");
        return false;
    }

    uint64_t bin_mtime;
    uint64_t src_mtime;
    struct stat st;

    const char* tmp_suffix = ".tmp";
    bool is_tmp = strstr(argv[0], tmp_suffix) != NULL;
    char tmp_path[256];
    strcpy(tmp_path, argv[0]);
    strcat(tmp_path, tmp_suffix);
    char sb_name[128];
    bool should_rebuild = false;

    SB_DA_ADD(sb_self_dep_files__);
    sb_string_append_cstr(SB_DA_LAST(sb_self_dep_files__), src);

    strncpy(sb_name, argv[0], 128);
    if (is_tmp) {
        sb_name[strlen(argv[0]) - strlen(tmp_suffix)] = '\0';
    }

    if (stat(sb_name, &st) == 0) {
        bin_mtime = FILE_MTIME_NS(st);
    } else {
        perror("Error bin file stats");
        return false;
    }

    for (int i = 0; i < sb_self_dep_files__.count; ++i) {
        SB_String *dep_file = sb_self_dep_files__.list + i;
        if (stat(dep_file->str, &st) == 0) {
            src_mtime = FILE_MTIME_NS(st);
            should_rebuild = src_mtime > bin_mtime;
            if (should_rebuild) {
                break;
            }
        } else {
            log_error("`self` depends on %s file, but not found\n", dep_file->str);
            return false;
        }
    }

    const char *cmd[SB_MAX_ARGS];
    if (should_rebuild) {
        // rebuild self
        if (is_tmp) {
            log_info("Rebuilding self ..\n");
            const char* tmp[] = {sb_default_compiler__, src, "-o", sb_name};
            if (sb_command_v(tmp, (sizeof(tmp) / sizeof(*tmp))) == 0) {
                cmd[0] = sb_name;
                int i = 1;
                for (; i < argc; ++i) {
                    assert(i < SB_MAX_ARGS && "Command too much arguments");
                    cmd[i] = argv[i];
                }
                exit(sb_command_v(cmd, i));
            } else {
                // restore sb file if compiling new sb failed
                copy_file(tmp_path, sb_name);
                remove(tmp_path);
                exit(-1);
            }
        }
        copy_file(argv[0], tmp_path);
        if (chmod(tmp_path, S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
            log_error("Failed to set executable permissions");
            return false;
        }
        cmd[0] = tmp_path;
        int i = 0;
        for (i = 1; i < argc; ++i) {
            assert(i < SB_MAX_ARGS && "Command too much arguments");
            cmd[i] = argv[i];
        }
        exit(sb_command_v(cmd, i));
    } else if (stat(tmp_path, &st) == 0) {
        remove(tmp_path);
    }
    return true;
}

void sb_self_add_dep(const char *file)
{
    SB_DA_ADD(sb_self_dep_files__);
    SB_String *file_str = SB_DA_LAST(sb_self_dep_files__);
    sb_string_append_cstr(file_str, file);
}

int sb_command_v(const char **cmd, int count)
{
    if (!cmd) {
        return -1;
    }

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }

    int status = 0;
    if (pid == 0) {
        const char *buff[count + 1];
        for (int i = 0; i < count; ++i) buff[i] = cmd[i];
        buff[count] = NULL;

        sb_print_command(cmd, count);
        execvp(cmd[0], (char* const*)buff);
        // only reach on error
        perror("execvp:");
    } else {
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
    }

    return status;
}

int sb_command(const char *cmd_str)
{
    SB_StringList string_list = sb_split_cstr(cmd_str);
    const char *cmd[string_list.count];
    for (int i = 0; i < string_list.count; ++i) {
        cmd[i] = string_list.list[i].str;
    }

    int result = sb_command_v(cmd, string_list.count);
    for (int i = 0; i < string_list.count; ++i) {
        sb_string_free(&string_list.list[i]);
    }
    SB_DA_FREE(string_list);
    return result;
}

int sb_command_p(const char *first, ...)
{
    const char *cmd[SB_MAX_ARGS];
    int cmd_idx = 0;

    va_list ap;
    va_start(ap, first);
        const char *p = first;
        while (p) {
            cmd[cmd_idx++] = p;
            p = va_arg(ap, const char*);
        }
    va_end(ap);

    return sb_command_v(cmd, cmd_idx);
}

SB_String sb_shell_v(const char **cmd, int count)
{
    int pipefd[2];
    pid_t pid;
    char buff[512];
    ssize_t bytes_read;

    SB_String output = {0};
    // 1. Create the pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return output;
    }

    // 2. Fork the process
    pid = fork();
    if (pid < 0) {
        perror("fork");
        return output;
    }

    if (pid == 0) {
        close(pipefd[0]); 
        dup2(pipefd[1], STDOUT_FILENO); 
        dup2(pipefd[1], STDERR_FILENO);
        close(pipefd[1]);

        const char *buff[count + 1];
        for (int i = 0; i < count; ++i) buff[i] = cmd[i];
        buff[count] = NULL;
        execvp(cmd[0], (char* const*)buff);
        perror("execvp");
    } else {
        close(pipefd[1]); 
        while ((bytes_read = read(pipefd[0], buff, sizeof(buff) - 1)) > 0) {
            sb_string_append_substr(&output, buff, buff + bytes_read);
        }
        close(pipefd[0]); 
        int status;
        waitpid(pid, &status, 0); 
    }

    sb_string_trim(&output);
    return output;
}

SB_String sb_shell(const char *cmd_str)
{
    SB_StringList string_list = sb_split_cstr(cmd_str);
    const char *cmd[string_list.count];
    for (int i = 0; i < string_list.count; ++i) {
        cmd[i] = string_list.list[i].str;
    }

    SB_String result = sb_shell_v(cmd, string_list.count);
    for (int i = 0; i < string_list.count; ++i) {
        sb_string_free(&string_list.list[i]);
    }
    SB_DA_FREE(string_list);
    return result;
}

SB_String sb_shell_p(const char *first, ...)
{
    const char *cmd[SB_MAX_ARGS];
    int cmd_idx = 0;

    va_list ap;
    va_start(ap, first);

    const char *p = first;
    while(p) {
        cmd[cmd_idx++] = p;
        p = va_arg(ap, const char*);
    }
    va_end(ap);

    return sb_shell_v(cmd, cmd_idx);
}


void sb_set_build_dir(const char* dir)
{
    static SB_String build_dir_string = {0};
    sb_string_clean(&build_dir_string);
    sb_string_append_cstr(&build_dir_string, dir);
    if (build_dir_string.str[build_dir_string.count - 1] != '/') {
        sb_string_append_char(&build_dir_string, '/');
    }
    sb_build_dir__ = build_dir_string.str;
    if (!mkdir_if_not_exists(sb_build_dir__)) {
        abort();
    }
}

const char *sb_build_dir()
{
    return sb_build_dir__;
}

SB_Target sb_create_any(const char *name)
{
    SB_Target target = {0};
    target.target_type = SB_ANY;
    target.always_build = false;
    if (name) {
        sb_string_append_cstr(&target.name, name);
        target.path = sb_path_join(sb_build_dir__, name);
        SB_String target_dir = sb_path_dirname(target.path.str);
        mkdir_if_not_exists(target_dir.str);
        sb_string_free(&target_dir);
    } else {
        sb_string_append_cstr(&target.name, "unnamed_target");
    }
    return target;
}

SB_Target sb_create_elf(const char *name)
{
    assert(name && strlen(name) > 0 && "Bad elf name");
    SB_Target target = {0};
    target.target_type = SB_ELF;
    target.always_build = false;
    sb_string_append_cstr(&target.name, name);
    sb_string_clean(&target.cmd);
    sb_string_append_cstr(&target.cmd, sb_default_compiler__);
    target.path = sb_path_join(sb_build_dir__, name);
    SB_String target_dir = sb_path_dirname(target.path.str);
    mkdir_if_not_exists(target_dir.str);
    sb_string_free(&target_dir);
    return target;
}

SB_Target sb_create_object(const char *name)
{
    assert(name && strlen(name) > 0 && "Bad object name");

    SB_Target target = {0};
    target.target_type = SB_OBJECT;
    target.always_build = false;
    sb_string_append_cstr(&target.name, name);
    sb_string_append_cstr(&target.cmd, sb_default_compiler__);
    target.path = sb_path_join(sb_build_dir__, name);
    target.obj_d_path = sb_string_copy(&target.path);
    sb_string_append_cstr(&target.obj_d_path, ".d");
    SB_String target_dir = sb_path_dirname(target.path.str);
    mkdir_if_not_exists(target_dir.str);
    sb_string_free(&target_dir);
    return target;
}

SB_Target sb_create_phony(const char *name)
{
    SB_Target target = {0};
    target.target_type = SB_PHONY;
    target.always_build = true;
    if (name) {
        sb_set_cmd(&target, name);
        sb_string_append_cstr(&target.name, name);
    } else {
        sb_string_append_cstr(&target.name, "unnamed_target");
    }
    return target;
}

SB_Target sb_create_archive(const char *name)
{
    assert(name && strlen(name) > 0 && "Bad archive name");

    SB_Target target = {0};
    target.target_type = SB_ARCHIVE;
    target.always_build = false;
    sb_string_append_cstr(&target.name, name);
    sb_string_append_cstr(&target.cmd, sb_default_ar__);
    target.path = sb_path_join(sb_build_dir__, name);
    SB_String target_dir = sb_path_dirname(target.path.str);
    mkdir_if_not_exists(target_dir.str);
    sb_string_free(&target_dir);
    return target;
}

bool sb_set_cmd(SB_Target *target, const char *cmd)
{
    if (!target) {
        return false;
    }

    sb_string_clean(&target->cmd);
    return sb_string_append_cstr(&target->cmd, cmd);
}

bool sb_set_compiler(SB_Target *target, const char *compiler)
{
    return sb_set_cmd(target, compiler);
}

bool sb_set_linker(SB_Target *target, const char *linker)
{
    return sb_set_cmd(target, linker);
}

bool sb_set_ar(SB_Target *target, const char *ar)
{
    return sb_set_cmd(target, ar);
}

bool sb_check_append_object_from_src(SB_Target *target, SB_String src)
{
    if (sb_string_empty(&src)) {
        return false;
    }

    SB_String obj_name = sb_string_copy(&src);
    sb_string_append_cstr(&obj_name, ".o");
    SB_Target obj = sb_create_object(obj_name.str);
    sb_set_compiler(&obj, "");   // set compiler on build time
    sb_add_flag(&obj, "-c");
    sb_add_flag(&obj, "-Wno-unused-command-line-argument");
    sb_add_src_s(&obj, src);
    sb_always_build(&obj, target->always_build);
    sb_string_free(&obj_name);

    sb_add_obj(target, obj);
    return true;
}

bool sb_add_src_s(SB_Target *target, SB_String src)
{
    if (!target) {
        return false;
    }

    if (sb_string_empty(&src)) {
        return false;
    }

    switch (target->target_type) {
    case SB_OBJECT:
        SB_DA_APPEND(target->srcs, src);
        break;
    case SB_ELF:
        if (!sb_check_append_object_from_src(target, src)) {
            return false;
        }
        break;
    default:
        log_error("Ignore add file `%s` for target type: %d\n", src.str, target->target_type);
        break;
    }

    return true;
}

bool sb_add_src(SB_Target *target, const char *src)
{
    if (!target) {
        return false;
    }

    SB_String src_string = {0};
    if (sb_string_append_cstr(&src_string, src) <= 0) {
        return false;
    }

    return sb_add_src_s(target, src_string);
}

bool sb_add_srcs(SB_Target *target, SB_StringList srcs)
{
    if (!target) {
        return false;
    }

    bool result = true;
    for (int i = 0; i < srcs.count; ++i) {
        if (!sb_add_src_s(target, sb_string_copy(&srcs.list[i]))) {
            result = false;
        }
    }

    return result;
}

bool sb_add_srcs_v(SB_Target *target, const char **srcs, int count)
{
    if (!target || !srcs) {
        return false;
    }

    bool result = true;
    for (int i = 0; i < count; ++i) {
        if (!sb_add_src(target, srcs[i])) {
            result = false;
        }
    }

    return result;
}

bool sb_add_flag_s(SB_Target *target, SB_String flag)
{
    if (!target) {
        return false;
    }

    if (sb_string_empty(&flag)) {
        return false;
    }

    SB_DA_APPEND(target->flags, flag);
    return true;
}

bool sb_add_flag(SB_Target *target, const char *flag)
{
    if (!target) {
        return false;
    }

    SB_String flag_string = {0};
    if (!sb_string_append_cstr(&flag_string, flag)) {
        return false;
    }
    return sb_add_flag_s(target, flag_string);
}

bool sb_add_flag_pair(SB_Target *target, const char *flag1, const char *flag2)
{
    return sb_add_flag(target, flag1) && sb_add_flag(target, flag2);
}

bool sb_add_flags(SB_Target *target, SB_StringList flags)
{
    if (!target) {
        return false;
    }

    bool result = false;
    for (int i = 0; i < flags.count; ++i) {
        if (sb_add_flag_s(target, sb_string_copy(&flags.list[i]))) {
            result = true;
        }
    }

    return result;
}

bool sb_add_flags_v(SB_Target *target, const char **flags, int count)
{
    if (!target || !flags) {
        return false;
    }

    bool result = false;
    for (int i = 0; i < count; ++i) {
        if (sb_add_flag(target, flags[i])) {
            result = true;
        }
    }

    return result;
}

bool sb_add_dep_file(SB_Target *target, const char *dep)
{
    if (!target) {
        return false;
    }

    SB_String dep_file = {0};
    if (!sb_string_append_cstr(&dep_file, dep)) {
        return false;
    }
    return sb_add_dep_file_s(target, dep_file);
}

bool sb_add_dep_file_s(SB_Target *target, SB_String dep)
{
    if (!target) {
        return false;
    }

    if (dep.count == 0) {
        return false;
    }
    SB_DA_APPEND(target->dep_files, dep);
    return true;
}

bool sb_add_dep_files(SB_Target *target, SB_StringList deps)
{
    if (!target) {
        return false;
    }

    bool result = true;
    for (int i = 0; i < deps.count; ++i) {
        if (!sb_add_dep_file_s(target, deps.list[i])) {
            result = false;
        }
    }
    return result;
}

bool sb_add_dep_files_v(SB_Target *target, const char **deps, int count)
{
    if (!target) {
        return false;
    }

    bool result = true;
    for (int i = 0; i < count; ++i) {
        if (!sb_add_dep_file(target, deps[i])) {
            result = false;
        }
    }
    return result;
}

bool sb_add_dep(SB_Target *target, SB_Target dep)
{
    if (!target) {
        return false;
    }

    SB_DA_APPEND(target->deps, dep);
    return true;
}

bool sb_add_deps(SB_Target *target, SB_TargetList deps)
{
    if (!target) {
        return false;
    }

    for (int i = 0; i < deps.count; ++i) {
        SB_DA_APPEND(target->deps, deps.list[i]);
    }
    return true;
}

bool sb_add_deps_v(SB_Target *target, const SB_Target* deps, int count)
{
    if (!target) {
        return false;
    }

    for (int i = 0; i < count; ++i) {
        SB_DA_APPEND(target->deps, deps[i]);
    }
    return true;
}

bool sb_add_obj(SB_Target *target, SB_Target obj)
{
    if (!target) {
        return false;
    }

    SB_DA_APPEND(target->objs, obj);
    return true;
}

bool sb_add_objs(SB_Target *target, SB_TargetList objs)
{
    if (!target) {
        return false;
    }

    bool result = true;
    for (int i = 0; i < objs.count; ++i) {
        if (!sb_add_obj(target, objs.list[i])) {
            result = false;
        }
    }

    return result;
}

bool sb_add_objs_v(SB_Target *target, const SB_Target* objs, int count)
{
    if (!target) {
        return false;
    }

    bool result = true;
    for (int i = 0; i < count; ++i) {
        if (!sb_add_obj(target, objs[i])) {
            result = false;
        }
    }

    return false;
}

bool sb_always_build(SB_Target *target, bool sure)
{
    if (!target) {
        return false;
    }

    target->always_build = sure;
    return true;
}

static int sb_build_cmd_fill_flags(SB_Target *target, const char *cmd[SB_MAX_ARGS], int cmd_idx, bool set_output)
{
    for (int flag_i = 0; flag_i < target->flags.count; ++flag_i) {
        SB_String *flag = &(target->flags.list[flag_i]);
        if (set_output && strcmp(flag->str, "-o") == 0) {
            set_output = false;
        }
        if (strcmp(flag->str, SB_TARGET) == 0) {
            if (!sb_string_empty(&target->path)) {
                cmd[cmd_idx++] = target->path.str;
            } else {
                log_error("Try to use `SB_TARGET` magic for unnamed target\n");
            }
        } else {
            cmd[cmd_idx++] = flag->str;
        }
    }

    if (set_output) {
        cmd[cmd_idx++] = "-o";
        cmd[cmd_idx++] = target->path.str;
    }

    return cmd_idx;
}

void sb_parse_make_style_dep_skip_space(const SB_String *content, int *i)
{
    if (!content || !i) {
        return;
    }

    while(*i < content->count) {
        char c = content->str[*i];
        if (c == ' ' || c == '\t') {
            ++(*i);
            continue;
        }
    
        if (c == '\\') {
            (*i) += 2;
            continue;
        }
        break;
    }
}

SB_StringView sb_parse_make_style_dep_read_path(const SB_String *content, int *i)
{
#define IS_NUM(x)   ((x) >= '0' && (x) <= '9')
#define IS_ALPHA(x) ((x) >= 'a' && (x) <= 'z' || (x) >= 'A' && (x) <= 'Z')

    SB_StringView sv = {};
    if (!content || !i) {
        return sv;
    }

    sv.str = content->str + *i;
    while(*i < content->count) {
        char c = content->str[*i];
        if (!IS_NUM(c) && !IS_ALPHA(c) && c != '.' && c != '_' && c != '-' && c != '/') break;
        ++(*i);
        ++sv.count;
    }
    return sv;

#undef IS_NUM
#undef IS_ALPHA
}

bool sb_parse_make_style_dep_exp_path(const SB_String *content, int *i)
{
    if (!content || !i) {
        return false;
    }

    SB_StringView path = sb_parse_make_style_dep_read_path(content, i);
    if (path.count == 0) {
        return false;
    }
    return true;
}

bool sb_parse_make_style_dep_exp_char(const SB_String *content, char c, int *i)
{
    if (!content || !i) {
        return false;
    }

    if (*i >= content->count || content->str[(*i)] != c) {
        log_error("make_style_depend file is ill-formed. expect %c, fount %c at %d\n", c, content->str[(*i)], *i);
        return false;
    }
    ++(*i);
    return true;
}

bool sb_parse_make_style_dep_exp_colon(const SB_String *content, int *i)
{
    return sb_parse_make_style_dep_exp_char(content, ':', i);
}

bool sb_parse_make_style_dep_exp_new_line(const SB_String *content, int *i)
{
    return sb_parse_make_style_dep_exp_char(content, '\n', i);
}

static bool sb_resolve_make_style_depend(SB_Target *target)
{
    if (!target || sb_string_empty(&target->obj_d_path)) {
        return false;
    }

    struct stat st;
    if (stat(target->path.str, &st) != 0) {
        return false;
    }

    SB_String content = sb_file_slurp(target->obj_d_path.str);
    if (sb_string_empty(&content)) {
        return false;
    }

    SB_StringView path = {};
    for (int i = 0; i < content.count;) {
        sb_parse_make_style_dep_skip_space(&content, &i);
        if (!sb_parse_make_style_dep_exp_path(&content, &i)) return false;
        sb_parse_make_style_dep_skip_space(&content, &i);
        if (!sb_parse_make_style_dep_exp_colon(&content, &i)) return false;
        while(true) {
            sb_parse_make_style_dep_skip_space(&content, &i);
            path = sb_parse_make_style_dep_read_path(&content, &i);
            if (path.count == 0) {
                break;
            } else {
                sb_add_dep_file_s(target, sb_string_copy_sv(&path));
            }
        }
        sb_parse_make_style_dep_exp_new_line(&content, &i);
    }

    return true;
}

static bool sb_build_resolve_depend(SB_Target *target)
{
    if (!target) {
        return false;
    }

    bool should_build = target->always_build || sb_string_empty(&target->path);

    for (int i = 0; i < target->deps.count; ++i) {
        SB_Target dep = target->deps.list[i];
        if (!sb_build(&dep) || dep.status != 0) {
            log_error("Failed to build %s\n", dep.name.str);
            target->status = -1;
            return false;
        }
        sb_add_dep_file_s(target, dep.path);
    }

    if (target->target_type == SB_OBJECT) {
        sb_resolve_make_style_depend(target);
    }

    for (int i = 0; i < target->dep_files.count; ++i) {
        if (should_build || sb_should_compile(target->dep_files.list[i].str, target->path.str)) {
            should_build = true;
        }
    }

    return should_build;
}

bool sb_build(SB_Target *target)
{
    if (!target) {
        return false;
    }

    bool should_build = sb_build_resolve_depend(target);
    const char *cmd[SB_MAX_ARGS];
    int cmd_idx = 0;

    cmd[cmd_idx++] = target->cmd.str;
    switch (target->target_type) {
    case SB_ELF: {
        for (int obj_i = 0; obj_i < target->objs.count; ++obj_i) {
            SB_Target *obj = &(target->objs.list[obj_i]);
            if (sb_string_empty(&obj->cmd)) {
                obj->cmd = sb_string_copy(&target->cmd);
                for (int flag_i = 0; flag_i < target->flags.count; ++flag_i) {
                    SB_DA_APPEND(obj->flags, target->flags.list[flag_i]);
                }
            }

            if (!sb_build(obj) || obj->status != 0) {
                log_error("Failed to build %s\n", obj->name.str);
                target->status = -1;
                return false;
            }
            if (should_build || sb_should_compile(obj->path.str, target->path.str)) {
                should_build = true;
            }
            cmd[cmd_idx++] = obj->path.str;
        }
        cmd_idx = sb_build_cmd_fill_flags(target, cmd, cmd_idx, true);
    } break;
    case SB_OBJECT: {
        for (int src_i = 0; src_i < target->srcs.count; ++src_i) {
            SB_String *src = &(target->srcs.list[src_i]);
            if (should_build || sb_should_compile(src->str, target->path.str)) {
                should_build = true;
            }
            cmd[cmd_idx++] = src->str;
        }
        cmd_idx = sb_build_cmd_fill_flags(target, cmd, cmd_idx, true);
        if (!sb_string_empty(&target->obj_d_path)) {
            cmd[cmd_idx++] = "-MMD";
            cmd[cmd_idx++] = "-MP";
            cmd[cmd_idx++] = "-MF";
            cmd[cmd_idx++] = target->obj_d_path.str;
        }
    } break;
    case SB_ARCHIVE: {
        cmd_idx = sb_build_cmd_fill_flags(target, cmd, cmd_idx, false);
        cmd[cmd_idx++] = target->path.str;

        for (int obj_i = 0; obj_i < target->objs.count; ++obj_i) {
            SB_Target *obj = &(target->objs.list[obj_i]);
            if (!sb_build(obj) || obj->status != 0) {
                fprintf(stderr, "Failed to build %s\n", obj->name.str);
                target->status = -1;
                return false;
            }
            if (should_build || sb_should_compile(obj->path.str, target->path.str)) {
                should_build = true;
            }
            cmd[cmd_idx++] = obj->path.str;
        }
    } break;
    case SB_PHONY: {
        should_build = true;
        cmd_idx = sb_build_cmd_fill_flags(target, cmd, cmd_idx, false);
    } break;
    case SB_ANY: {
        should_build = should_build || (target->deps.count == 0 && target->dep_files.count == 0);
        cmd_idx = sb_build_cmd_fill_flags(target, cmd, cmd_idx, false);
    } break;
    default:
        log_error("Unknown target type: %d\n", target->target_type);
        break;
    }

    if (should_build) {
        target->status = sb_command_v(cmd, cmd_idx);
    } else {
        if (target->target_type == SB_ELF || target->target_type == SB_ARCHIVE ||
            target->target_type == SB_ANY) {
            log_info("No update for %s\n", target->name.str);
        }
        target->status = 0;
    }

    return true;
}

bool sb_should_compile(const char *src, const char *target)
{
    if (!target || strlen(target) == 0) {
        return true;
    }

    bool result = true;
    struct stat st;
    if (stat(src, &st) == 0) {
        uint64_t src_mtime = FILE_MTIME_NS(st);
        if (stat(target, &st) == 0) {
            uint64_t obj_mtime = FILE_MTIME_NS(st);
            if (obj_mtime > src_mtime) {
                result = false;
            }
        }
    } else {
        log_error("source file %s not found\n", src);
        return false;
    }

    return result;
}

void sb_print_command(const char **cmd, int count)
{
    printf("[CMD]: ");
    for (size_t i = 0; i < count; ++i) {
        printf("%s", cmd[i]);
        if (i == count - 1)
            printf("\n");
        else
            printf(" ");
    }
}

// ----------- flag --------
enum SB_AP_SchemeType
{
    SB_APST_UNKNOWN = 0,
    SB_APST_STR,
    SB_APST_INT,
    SB_APST_FLOAT,
    SB_APST_BOOL,
    SB_APST_VSTR,
    SB_APST_VINT,
    SB_APST_VFLOAT,
}; // enum SB_AP_SchemeType

typedef struct SB_AP_Scheme {
    union
    {
        SB_String str;
        SB_StringList vstr;
        SB_IntList vi;
        SB_FloatList vf;
        SB_BoolList vb;
        int i;
        float f;
        bool b;
    };
    enum SB_AP_SchemeType type;
    const char* key;
    const char* help;
} SB_AP_Scheme; // struct SB_AP_Scheme

typedef struct SB_ArgParser
{
    const char* self_anme;
    SB_AP_Scheme scheme_list[SB_MAX_ARGS];
    int scheme_idx;
} SB_ArgParser; // struct SB_ArgParser

static SB_ArgParser sb_ap__ = {0};

SB_AP_Scheme *sb_flag_add_scheme(const char *key, const char *help)
{
    if (sb_ap__.scheme_idx >= SB_MAX_ARGS) {
        log_error("Tooo much args to parse\n");
        abort();
    }

    SB_AP_Scheme *scheme = &(sb_ap__.scheme_list[sb_ap__.scheme_idx++]);
    scheme->help = help;
    scheme->key = key;
    scheme->type = SB_APST_UNKNOWN;
    return scheme;
}

SB_StringList sb_str_split_with_comma(const char* str)
{
    SB_StringList output = {0};
    SB_String string = {0};
    sb_string_reserve(&string, 8);

    if (str) {
        while (*str) {
            if (*str != ',') {
                sb_string_append_char(&string, *str);
            } else {
                SB_DA_APPEND(output, string);
                string.str = NULL;
                string.count = 0;
                string.capacity = 0;
                sb_string_reserve(&string, 8);
            }
            ++str;
        }
        if (string.count > 0) {
            SB_DA_APPEND(output, string);
        }
    }
    return output;
}

const char *sb_flag_string(const char *key, const char *default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    sb_string_append_cstr(&scheme->str, default_val);
    scheme->type = SB_APST_STR;
    return scheme->str.str;
}

int *sb_flag_int(const char *key, int default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    scheme->i = default_val;
    scheme->type = SB_APST_INT;
    return &scheme->i;
}

float *sb_flag_float(const char *key, float default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    scheme->f = default_val;
    scheme->type = SB_APST_FLOAT;
    return &scheme->f;
}

bool *sb_flag_bool(const char *key, bool default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    scheme->b = default_val;
    scheme->type = SB_APST_BOOL;
    return &scheme->b;
}

SB_StringList *sb_flag_string_v(const char *key, const SB_StringList *default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    scheme->vstr = *default_val;
    scheme->type = SB_APST_VSTR;
    return &(scheme->vstr);
}

SB_IntList *sb_flag_int_v(const char *key, const SB_IntList *default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    scheme->vi = *default_val;
    scheme->type = SB_APST_VINT;
    return &(scheme->vi);
}

SB_FloatList *sb_flag_float_v(const char *key, const SB_FloatList *default_val, const char *help)
{
    SB_AP_Scheme *scheme = sb_flag_add_scheme(key, help);
    scheme->vf = *default_val;
    scheme->type = SB_APST_VFLOAT;
    return &(scheme->vf);
}

bool sb_flag_parse(int argc, char **argv)
{
    if (argc < 1) {
        return false;
    }

    for (int i = 0; i < argc; ++i) {
        const char* this_arg = argv[i];
        bool arg_matched = false;
        if(i == 0) {
            sb_ap__.self_anme = this_arg;
            continue;
        }

        for (int j = 0; j < sb_ap__.scheme_idx && !arg_matched; ++j) {
            SB_AP_Scheme *scheme = &(sb_ap__.scheme_list[j]);
            if (strcmp(scheme->key, this_arg) == 0) {
                arg_matched = true;
                const char* next_arg = i < argc ? argv[i + 1] : NULL;
                switch (scheme->type) {
                case SB_APST_STR:
                    if (!next_arg) return false;
                    sb_string_clean(&scheme->str);
                    sb_string_append_cstr(&scheme->str, next_arg);
                    ++i;
                    break;
                case SB_APST_INT:
                    if (!next_arg) return false;
                    scheme->i = atoi(next_arg);
                    ++i;
                    break;
                case SB_APST_FLOAT:
                    if (!next_arg) return false;
                    scheme->f = atof(next_arg);
                    ++i;
                    break;
                case SB_APST_BOOL:
                    if (!next_arg || strcmp(next_arg, "true") == 0) {
                        scheme->b = true;
                        ++i;
                    } else if (next_arg && strcmp(next_arg, "false") == 0) {
                        scheme->b = false;
                        ++i;
                    } else {
                        scheme->b = true;
                    }
                    break;
                case SB_APST_VSTR: {
                    if (!next_arg) return false;
                    SB_DA_FREE(scheme->vstr);
                    scheme->vstr = sb_str_split_with_comma(next_arg);
                    ++i;
                } break;
                case SB_APST_VINT: {
                    if (!next_arg) return false;
                    SB_StringList vstr = sb_str_split_with_comma(next_arg);
                    SB_DA_FREE(scheme->vi);
                    for (int str_i = 0; str_i < vstr.count; ++ str_i) {
                        SB_String *this_str = &vstr.list[str_i];
                        if (!sb_string_empty(this_str)) {
                            SB_DA_APPEND(scheme->vi, atoi(this_str->str));
                        } else {
                            log_error("Bad int type argument: %s\n", this_str->str);
                        }
                        //  free tmp string
                        sb_string_free(this_str);
                    }
                    SB_DA_FREE(vstr);
                    ++i;
                } break;
                case SB_APST_VFLOAT: {
                   if (!next_arg) return false;
                    SB_StringList vstr = sb_str_split_with_comma(next_arg);
                    SB_DA_FREE(scheme->vf);
                    for (int str_i = 0; str_i < vstr.count; ++ str_i) {
                        SB_String *this_str = &vstr.list[str_i];
                        if (!sb_string_empty(this_str)) {
                            SB_DA_APPEND(scheme->vf, atof(this_str->str));
                        } else {
                            log_error("Bad float type argument: %s\n", this_str->str);
                        }
                        //  free tmp string
                        sb_string_free(this_str);
                    }
                    SB_DA_FREE(vstr);
                    ++i;
                } break;
                case SB_APST_UNKNOWN:
                default:
                    log_error("Unknown argument type for %s\n", this_arg);
                    return false;
                }
                continue;
            }
        }
        if (!arg_matched) {
            log_error("Unknown argument: %s\n", this_arg);
            return false;
        }
    }
    return true;
}

void sb_flag_show_usage()
{
    const int help_align_len = 32;

    printf("Usage for %s\n", sb_ap__.self_anme);
    SB_String help_line = {0};
    sb_string_reserve(&help_line, 64);

    for (int i = 0; i < sb_ap__.scheme_idx; ++i) {
        SB_AP_Scheme *scheme = &(sb_ap__.scheme_list[i]);
        sb_string_append_cstr(&help_line, "  ");
        sb_string_append_cstr(&help_line, scheme->key);
        sb_string_append_char(&help_line, ' ');

        switch (scheme->type) {
        case SB_APST_STR:
            sb_string_append_cstr(&help_line, "<str>");
            break;
        case SB_APST_INT:
            sb_string_append_cstr(&help_line, "<int>");
            break;
        case SB_APST_FLOAT:
            sb_string_append_cstr(&help_line, "<float>");
            break;
        case SB_APST_BOOL:
            sb_string_append_cstr(&help_line, "<[true|false]>");
            break;
        case SB_APST_VSTR:
            sb_string_append_cstr(&help_line, "<str,str,..>");
            break;
        case SB_APST_VINT:
            sb_string_append_cstr(&help_line, "<int,int,..>");
            break;
        case SB_APST_VFLOAT:
            sb_string_append_cstr(&help_line, "<float,float,..>");
            break;
        default:
            break;
        }

        printf("%s", help_line.str);
        int help_line_len = help_line.count;
        if (help_line_len > help_align_len) {
            printf("\n");
            help_line_len = 0;
        }
        for (int j = 0; j < help_align_len - help_line_len; ++j) {
            printf(" ");
        }
        printf("%s\n", scheme->help);
        sb_string_clean(&help_line);
    }
    sb_string_free(&help_line);
    printf("\n");
}

// -------- unit testing suit --------

SB_Testing __sb_testing;

void sb_testing_assert_success(SB_TestingCase *sb_case)
{
    ++sb_case->success_assert;
}

void sb_testing_assert_failed(SB_TestingCase *sb_case, const char *filename, int line, const char *checking)
{
    ++sb_case->failed_assert;
    printf(">> Case %s.%s failed on checking"SB_TERM_COLOR_HL_S" %s "SB_TERM_COLOR_E"%s:%d\n",
        sb_case->module.str, sb_case->name.str, checking, filename, line);
}

void sb_testing_case_report(SB_TestingCase *sb_case)
{
    int padding_len = SB_TESTING_MODULE_NAME_LEN - sb_case->module.count - sb_case->name.count;

    if (sb_case->failed_assert == 0) {
        printf(SB_TERM_COLOR_SUCC_S"%s.%s ", sb_case->module.str, sb_case->name.str);
        for (int i = 0; i < padding_len; ++i) {
            printf(".");
        }
        printf(" PASSED\n");
    } else {
        printf(SB_TERM_COLOR_FAIL_S"%s.%s ", sb_case->module.str, sb_case->name.str);
        for (int i = 0; i < padding_len; ++i) {
            printf(".");
        }
        printf(" FAILED\n");
    }
    printf(SB_TERM_COLOR_E);
}

int sb_testing_summary()
{
    int passed_cnt = 0;
    int failed_cnt = 0;

    for (int i = 0; i < __sb_testing.cases.count; ++i) {
        if (__sb_testing.cases.list[i].failed_assert > 0) {
            ++failed_cnt;
        } else {
            ++passed_cnt;
        }
    }

    printf("============== summary ==============\n");
    printf("    %d cases tested\n"
           "    %d"SB_TERM_COLOR_SUCC_S" passed\n"SB_TERM_COLOR_E
           "    %d"SB_TERM_COLOR_FAIL_S" failed\n"SB_TERM_COLOR_E,
           __sb_testing.cases.count, passed_cnt, failed_cnt);
    printf("=====================================\n");
    return failed_cnt;
}

const char* sb_dir()
{
    static char output[256] = {0};
    uint32_t len = sizeof(output);
#ifdef __APPLE__
    if (_NSGetExecutablePath(output, &len) != 0) {
        abort();
    }
#else // __APPLE__
    const char *self_exe = "/proc/self/exe";
    len = readlink(self_exe, output, len - 1);
    if (len >= len - 1) {
        abort();
    }
#endif // __APPLE__

    size_t last_slash = 0;
    for (size_t i = 0; i < len; ++i) {
        if (output[i] == '/') {
            last_slash = i;
        }
    }
    output[last_slash + 1] = '\0';
    return output;
}

bool mkdir_if_not_exists(const char* dir)
{
    const mode_t mode = 0755;
    struct stat st;
    int dir_len = strlen(dir);
    char buff[dir_len + 1];
    for (size_t i = 0; i < dir_len; ++i) {
        char ch = dir[i];
        buff[i] = ch;
        buff[i + 1] = '\0';
        if (ch == '/' || i == dir_len - 1) {
            if(stat(buff, &st) != 0) {
                if (mkdir(buff, mode) != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

int copy_file(const char *src, const char *dest)
{
    FILE *src_file = fopen(src, "rb");
    if (!src_file) return -1;

    FILE *dest_file = fopen(dest, "wb");
    if (!dest_file) {
        fclose(src_file);
        return -1;
    }

    char buffer[8192]; // 8KB buffer size
    size_t bytes_read;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), src_file)) > 0) {
        fwrite(buffer, 1, bytes_read, dest_file);
    }

    fclose(src_file);
    fclose(dest_file);
    return 0;
}

SB_String* sb_string_trim_left(SB_String *str)
{
    if (sb_string_empty(str)) {
        return str;
    }

    int i = 0;
    for (; i < str->count; ++i) {
        if (!isspace(str->str[i])) {
            break;
        }
    }
    if (i != 0) {
        str->count -= i;
        char* tmp = (char*)malloc(str->capacity);
        memcpy(tmp, str->str + i, str->count);
        free(str->str);
        str->str = tmp;
    }
    return str;   
}

SB_String* sb_string_trim_right(SB_String *str)
{
    if (sb_string_empty(str)) {
        return str;
    }

    int i = str->count - 1;
    for (; i >= 0; --i) {
        if (!isspace(str->str[i])) {
            break;
        } else {
            --str->count;
        }
    }
    return str;
}

SB_String* sb_string_trim(SB_String *str)
{
    return sb_string_trim_left(sb_string_trim_right(str));
}

SB_String sb_path_join(const char *parent, const char *child)
{
    SB_String output = {0};
    if (!parent) {
        sb_string_append_cstr(&output, child);
        return output;
    }

    if (!child) {
        sb_string_append_cstr(&output, parent);
        return output;
    }

    if (parent[strlen(parent) - 1] == '/') {
        sb_string_append_cstr(&output, parent);
        sb_string_append_cstr(&output, child);
    } else {
        sb_string_append_cstr(&output, parent);
        sb_string_append_char(&output, '/');
        sb_string_append_cstr(&output, child);
    }
    return output;
}

SB_String sb_path_dirname(const char *path)
{
    SB_String output = {0};
    if (!path) {
        return output;
    }

    sb_string_append_cstr(&output, path);
    for (int i = output.count - 1; i >= 0; --i) {
        if (output.str[i] == '/') {
            if (i < output.count - 1) {
                output.str[i + 1] = '\0';
                output.count = i + 1;
            }
            break;
        }
    }
    return output;
}

SB_StringList sb_split_cstr(const char* str)
{
    size_t idx = 0;
    size_t last = 0;
    size_t i = 0;
    SB_StringList string_list = {0};

    for (;; ++i) {
        char ch = str[i];
        if (ch == '\0') {
            if (last != i) {
                SB_String string = {0};
                sb_string_append_substr(&string, str + last, str + i);
                SB_DA_APPEND(string_list, string);
            }
            break;
        }

        switch (ch) {
        case '\'':
        case '"': {
            char match_target = ch;
            int start = i + 1;
            char next = str[++i];
            while (next && next != match_target) {
                next = str[++i];
            }
            SB_String string = {0};
            sb_string_append_substr(&string, str + start, str + i);
            SB_DA_APPEND(string_list, string);
            last = i + 1;
        } break;
        case ' ':
        case '\n':
        case '\t':
            if (last != i) {
                SB_String string = {0};
                sb_string_append_substr(&string, str + last, str + i);
                SB_DA_APPEND(string_list, string);
            }
            last = i + 1;
            break;
        default:
            break;
        }
    }
    return string_list;
}

SB_String sb_file_slurp(const char *filename)
{
    SB_String content = {};
    if (!filename) {
        return content;
    }

    FILE *fp = fopen(filename, "r");
    if (!fp) {
        log_error("file_slurp: can not open file %s\n", filename);
        return content;
    }

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);

    if (size < 0) {
        fclose(fp);
        return content;
    }

    sb_string_resize(&content, size);
    int read_bytes = fread(content.str, 1, size, fp);
    content.str[read_bytes] = '\0';

    fclose(fp);
    return content;
}

int sb_string_append_substr(SB_String *string, const char *start, const char *end)
{
    if (!string || !start || !end) {
        return 0;
    }

    const int str_len = end - start;
    int remain_cap = string->capacity - string->count;
    int target_cap = remain_cap >= (str_len + 1)
        ? string->capacity
        : string->capacity * 2;
    remain_cap = target_cap - string->count;
    target_cap = remain_cap >= (str_len + 1)
        ? target_cap
        : ((string->capacity * 2 + str_len - string->count + 1) / 8 + 1) * 8;
    if (target_cap != string->capacity) {
        string->capacity = target_cap;
        string->str = string->str
            ? (char*)realloc(string->str, string->capacity)
            : (char*)malloc(string->capacity);
    }
    for (int i = 0; i < str_len; ++i) {
        string->str[string->count] = *((start) + i);
        ++(string->count);
    }
    string->str[string->count] = '\0';
    return str_len;
}

int sb_string_append_char(SB_String *string, char c)
{
    if (!string) {
        return 0;
    }

    int remain_cap = string->capacity - string->count;
    int target_cap = remain_cap >= 2
        ? string->capacity
        : string->capacity * 2;
    remain_cap = target_cap - string->count;
    target_cap = remain_cap >= 2
        ? target_cap
        : ((string->capacity + 2) / 8 + 1) * 8;
    if (target_cap != string->capacity) {
        string->capacity = target_cap;
        string->str = string->str
            ? (char*)realloc(string->str, string->capacity)
            : (char*)malloc(string->capacity);
    }

    string->str[string->count] = c;
    ++(string->count);
    string->str[string->count] = '\0';
    return 1;
}

int sb_string_append_cstr(SB_String *string, const char *cstr)
{
    if (!string || !cstr) {
        return 0;
    }

    const int str_len = strlen(cstr);
    int remain_cap = string->capacity - string->count;
    int target_cap = remain_cap >= (str_len + 1)
        ? string->capacity
        : string->capacity * 2;
    remain_cap = target_cap - string->count;
    target_cap = remain_cap >= (str_len + 1)
        ? target_cap
        : ((string->capacity * 2 + str_len - string->count + 1) / 8 + 1) * 8;
    if (target_cap != string->capacity) {
        string->capacity = target_cap;
        string->str = string->str
            ? (char*)realloc(string->str, string->capacity)
            : (char*)malloc(string->capacity);
    }
    for (int i = 0; i < str_len; ++i) {
        string->str[string->count] = cstr[i];
        ++(string->count);
        assert(string->count < string->capacity);
    }
    string->str[string->count] = '\0';
    return str_len;
}

SB_String sb_string_copy(const SB_String *src)
{
    SB_String dest = {0};
    if (!src) {
        return dest;
    }

    dest.capacity = src->capacity;
    dest.str = (char*)malloc(src->capacity);

    memcpy(dest.str, src->str, src->count + 1);
    dest.count = src->count;
    dest.capacity = src->capacity;
    return dest;
}

SB_String sb_string_copy_sv(const SB_StringView *src)
{
    SB_String dest = {0};
    if (!src || src->count == 0 || !src->str) {
        return dest;
    }

    sb_string_resize(&dest, src->count);
    memcpy(dest.str, src->str, src->count);
    return dest;
}

void sb_string_free(SB_String *string)
{
    if (!string) {
        return;
    }

    if (string->str) {
        free(string->str);
    }

    string->str = NULL;
    string->count = 0;
    string->capacity = 0;
}

bool sb_string_empty(const SB_String *string)
{
    return !string->str || string->count <= 0;
}

void sb_string_clean(SB_String *string)
{
    string->count = 0;
}

bool sb_string_resize(SB_String *string, int count)
{
    if (!sb_string_reserve(string, count + 1)) {
        return false;
    }
    string->count = count;
    string->str[count] = '\0';
    return true;
}

bool sb_string_reserve(SB_String *string, int count)
{
    if (!string) {
        return false;
    }

    int target_cap = string->capacity;
    if (string->str == NULL || string->capacity < count) {
        target_cap = (count / 8 + 1) * 8;
        string->capacity = 0;
    }

    if (string->capacity != target_cap) {
        string->capacity = target_cap;
        string->str = string->str
            ? (char*)malloc(string->capacity)
            : (char*)realloc(string->str, string->capacity);
    }

    return true;
}

#endif // SB_IMPLEMENTATION
