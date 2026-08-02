#ifndef SB_HPP_
#define SB_HPP_

#include <string>
#include <string_view>
#include <vector>
#include <ostream>
#include <iostream>

#ifndef SB_MAX_ARGS
#   define SB_MAX_ARGS 64
#endif // SB_MAX_ARGS

namespace sb
{

enum Lib
{
    DYNAMIC = 0,
    STATIC,
};

const std::vector<std::string> excluded_flags {
    "-o", "-c"
};

class Object;
extern std::string default_compiler__;
extern std::string build_dir__;

class Entity
{
public:
    Entity& set_build_dir(const std::string& build_dir);
    Entity& set_compiler(const std::string& compiler);
    Entity& add_srcs(const std::string_view& file);
    Entity& add_srcs(const std::vector<std::string>& files);
    Entity& add_flags(const std::string_view& flag);
    Entity& add_flags(const std::vector<std::string>& flags);
    Entity& always_build(bool sure = true);
    int status() const;
    const std::string& path() const;

#if __cplusplus >= 201703L
    template<typename... Args>
    Entity& add_srcs(const std::string_view& file, Args... args);
    template<typename... Args>
    Entity& add_flags(const std::string_view& flag, Args... args);
#endif //__cplusplus >= 201703L
    Entity& build();

    friend class Object;
    friend Entity create_lib(const std::string& name);
    friend Entity create_exe(const std::string& name);

    const std::string name;
private:
    enum TargetType
    {
        EXECUTABLE,
        LIBRARY,
    }; // enum TargetType

    Entity(const std::string& name, TargetType tt);
    bool check_append_flag(const std::string& flag);
    bool check_append_obj(const std::string& src);

    std::vector<std::string> flags_;
    std::vector<Object> objs_;
    std::string compiler_;
    std::string build_dir_;
    std::string path_;
    const TargetType tt_;
    bool always_build_;
    int status_;
}; // class Entity

bool auto_rebuild_self__(int argc, char** argv, const char* src);
void set_default_compiler(const char* compiler);
void set_build_dir(const char* build_dir);
int run_command(const std::vector<std::string>& cmd);
int run_command(const char* first, ...);
std::string shell(const std::vector<std::string>& cmds);
std::string shell(const char* first, ...);
std::string shell_s(const char* cmd_str);
Entity create_lib(const std::string& name);
Entity create_exe(const std::string& name);

#define auto_rebuild_self(argc, argv) auto_rebuild_self__(argc, argv, __FILE__)

// ------------ flags ------------

template<typename T, typename U>
struct same_type
{
    static const bool value = false;
}; // struct same_type<T, U>

template<typename T>
struct same_type<T, T>
{
    static const bool value = true;
}; // struct same_type<T, U>

class ArgParser
{
    enum SchemeType
    {
        ST_UNKNOWN = 0,
        ST_STR,
        ST_INT,
        ST_FLOAT,
        ST_BOOL,
        ST_VSTR,
        ST_VINT,
        ST_VFLOAT,
    };
    struct Scheme
    {
        Scheme();
        ~Scheme();

        union
        {
            std::string str;
            int i;
            float f;
            bool b;
            std::vector<std::string> vstr;
            std::vector<int> vi;
            std::vector<float> vf;
            std::vector<bool> vb;
        };
        SchemeType type;
        const char* key;
        const char* help;
    }; // struct Scheme

    template<typename T>
    friend T& flags_arg(const char *key, T default_val, const char *help);
    friend bool flags_parse(int argc, char **argv);
    friend void flags_show_usage();

    static ArgParser* instance();
    Scheme& add_scheme(const char *key, const char *help);
    ArgParser();
    const char* self_anme_;
    Scheme scheme_list_[SB_MAX_ARGS];
    int scheme_idx_;
}; // class ArgParser

template<typename T>
T& flags_arg(const char *key, T default_val, const char *help)
{
    std::cout << "Unknoen type\n";
    return T();
}

bool flags_parse(int argc, char **argv);
void flags_show_usage();
    
// -------- Unit testing ---------

struct BaseTestingCase
{
    BaseTestingCase(const std::string& module, const std::string& name);
    std::ostream& on_assert_success(std::ostream& os);
    std::ostream& on_assert_failed(std::ostream& os, const char *filename, int line,
        const char *checking);
    void report(std::ostream& os) const;

    virtual void body() = 0;

    const std::string __module;
    const std::string __name;
    const int __module_name_len = 47;
    const int __padding_len;
    int __failed_assert;
    int __success_assert;
}; // class BaseTestingCase

#define SB_ABS(x) ((x) > 0 ? (x) : -(x))
#define SB_FLT_NEAR(x, y, err) (SB_ABS((x) - (y)) < err)


class TestingCases
{
public:
    static TestingCases* instance();
    void report() const;
    void test_all();
    int failed_cnt() const { return failed_cnt_; }
    int passed_cnt() const { return passed_cnt_; }
private:
    friend class BaseTestingCase;
    const std::vector<BaseTestingCase*>& cases() const;
    void add_case(BaseTestingCase* c);
    TestingCases();

    std::vector<BaseTestingCase*> cases_;
    int passed_cnt_;
    int failed_cnt_;
}; // class TestingCases


#define SB_ASSERT_T(x)                                                                              \
    do {                                                                                            \
        if (x) {                                                                                    \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_T")                             \
                << #x" != true\n";                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_F(x)                                                                              \
    do {                                                                                            \
        if (!(x)) {                                                                                 \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_F")                             \
                << #x" != false\n";                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_EQ(x, y)                                                                          \
    do {                                                                                            \
        if ((x) == (y)) {                                                                           \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_EQ")                            \
                << #x" != "#y"\n";                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_NE(x, y)                                                                          \
    do {                                                                                            \
        if ((x) != (y)) {                                                                           \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_NE")                            \
                << #x" == "#y"\n";                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_GT(x, y)                                                                          \
    do {                                                                                            \
        if ((x) > (y)) {                                                                            \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_GT")                            \
                << #x" <= "#y"\n";                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_GE(x, y)                                                                          \
    do {                                                                                            \
        if ((x) >= (y)) {                                                                           \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_GE")                            \
                << #x" < "#y"\n";                                                                   \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_LT(x, y)                                                                          \
    do {                                                                                            \
        if ((x) < (y)) {                                                                            \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_NT")                            \
                << #x" >= "#y"\n";                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_LE(x, y)                                                                          \
    do {                                                                                            \
        if ((x) <= (y)) {                                                                           \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_LE")                            \
                << #x" > "#y"\n";                                                                   \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY_EQ(x, y, n)                                                                 \
    do {                                                                                            \
        std::ostream& os = std::cout;                                                               \
        bool has_failed_case = false;                                                               \
        for (size_t i = 0; i < n; ++i) {                                                            \
            if ((x)[i] != (y)[i]) {                                                                 \
                if (!has_failed_case) {                                                             \
                    on_assert_failed(os, __FILE__, __LINE__, "ASSERT_ARRAY_EQ");                    \
                }                                                                                   \
                if (has_failed_case) {                                                              \
                    os << ", ";                                                                     \
                }                                                                                   \
                os << #x "[" << i << "] != " #y "[" << i << ']';                                    \
                has_failed_case = true;                                                             \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            os << '\n';                                                                             \
        } else {                                                                                    \
            on_assert_success(os);                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY_NEAR(x, y, n, abs_err)                                                      \
    do {                                                                                            \
        std::ostream& os = std::cout;                                                               \
        bool has_failed_case = false;                                                               \
        for (size_t i = 0; i < n; ++i) {                                                            \
            if (!SB_FLT_NEAR((x)[i], (y)[i], abs_err)) {                                            \
                if (!has_failed_case) {                                                             \
                    on_assert_failed(os, __FILE__, __LINE__, "ASSERT_ARRAY_NEAR");                  \
                }                                                                                   \
                if (has_failed_case) {                                                              \
                    os << ", ";                                                                     \
                }                                                                                   \
                os << #x "[" << i << "] != " #y "[" << i << ']';                                    \
                has_failed_case = true;                                                             \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            os << '\n';                                                                             \
        } else {                                                                                    \
            on_assert_success(os);                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY2_EQ(x, y, m, n)                                                             \
    do {                                                                                            \
        std::ostream& os = std::cout;                                                               \
        bool has_failed_case = false;                                                               \
        for (size_t i = 0; i < n; ++i) {                                                            \
            for (size_t j = 0; j < m; ++j) {                                                        \
                if ((x)[i][j] != (y)[i][j]) {                                                       \
                    if (!has_failed_case) {                                                         \
                        on_assert_failed(os, __FILE__, __LINE__, "ASSERT_ARRAY2_EQ");               \
                    }                                                                               \
                    if (has_failed_case) {                                                          \
                        os << ", ";                                                                 \
                    }                                                                               \
                    os << #x "[" << i << "][" << j << "] != " #y "[" << i << "][" << j << ']';      \
                    has_failed_case = true;                                                         \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            os << '\n';                                                                             \
        } else {                                                                                    \
            on_assert_success(os);                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_ARRAY2_NEAR(x, y, m, n, abs_err)                                                  \
    do {                                                                                            \
        std::ostream& os = std::cout;                                                               \
        bool has_failed_case = false;                                                               \
        for (size_t i = 0; i < n; ++i) {                                                            \
            for (size_t j = 0; j < m; ++j) {                                                        \
                if (!SB_FLT_NEAR((x)[i][j], (y)[i][j], abs_err)) {                                  \
                    if (!has_failed_case) {                                                         \
                        on_assert_failed(os, __FILE__, __LINE__, "ASSERT_ARRAY2_NEAR");             \
                    }                                                                               \
                    if (has_failed_case) {                                                          \
                        os << ", ";                                                                 \
                    }                                                                               \
                    os << #x "[" << i << "][" << j << "] != " #y "[" << i << "][" << j << ']';      \
                    has_failed_case = true;                                                         \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
        if (has_failed_case) {                                                                      \
            os << '\n';                                                                             \
        } else {                                                                                    \
            on_assert_success(os);                                                                  \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_NEAR(x, y, abs_err)                                                               \
    do {                                                                                            \
        if (SB_FLT_NEAR(x, y, abs_err)) {                                                           \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_NEAR")                          \
                <<  #x" != "#y"\n";                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_NNEAR(x, y, abs_err)                                                              \
    do {                                                                                            \
        if (!SB_FLT_NEAR(x, y, abs_err)) {                                                          \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_NNEAR")                         \
                <<  #x" == "#y"\n";                                                                 \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_CSTR_EQ(x, y)                                                                     \
    do {                                                                                            \
        if ((x) == nullptr && (y) == nullptr) {                                                     \
            on_assert_success(std::cout);                                                           \
        } else {                                                                                    \
            if ((x) == nullptr) {                                                                   \
                on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_CSTR_EQ")                   \
                    << #x" is NULL while y not\n";                                                  \
            } else if ((y) == nullptr) {                                                            \
                on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_CSTR_EQ")                   \
                    << #y" is NULL while x not\n";                                                  \
            } else {                                                                                \
                size_t i = 0;                                                                       \
                bool failed = false;                                                                \
                while (!failed && (x)[i] && (y)[i]) {                                               \
                    if ((x)[i] != (y)[i]) {                                                         \
                        failed = true;                                                              \
                        on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_CSTR_EQ")           \
                            << #x" != "#y", the first not equal occurred at " << #x"[" << i         \
                            << "](" << (x)[i] << ") != " << #y"[" << i << "](" << (y)[i] << ")\n";  \
                    }                                                                               \
                    ++i;                                                                            \
                }                                                                                   \
                if (!failed) {                                                                      \
                    if ((x)[i] == '\0' && (y)[i] == '\0') {                                         \
                        on_assert_success(std::cout);                                               \
                    } else {                                                                        \
                        on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_CSTR_EQ")           \
                            << #x" and "#y" has different length\n";                                \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
    } while (0)

#define SB_ASSERT_CSTR_NE(x, y)                                                                     \
    do {                                                                                            \
        if ((x) == nullptr && (y) == nullptr) {                                                     \
            on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_CSTR_NE")                       \
                << "both "#x" and "#y" are NULL\n";                                                 \
        } else {                                                                                    \
            if ((x) == nullptr || (y) == nullptr) {                                                 \
                on_assert_success(std::cout);                                                       \
            } else {                                                                                \
                size_t i = 0;                                                                       \
                bool success = false;                                                               \
                while (!success && (x)[i] && (y)[i]) {                                              \
                    if ((x)[i] != (y)[i]) {                                                         \
                        success = true;                                                             \
                        on_assert_success(std::cout);                                               \
                    }                                                                               \
                    ++i;                                                                            \
                }                                                                                   \
                if (!success) {                                                                     \
                    if ((x)[i] == '\0' && (y)[i] == '\0') {                                         \
                        on_assert_failed(std::cout, __FILE__, __LINE__, "ASSERT_CSTR_NE")           \
                            << #x" == "#y"\n";                                                      \
                    } else {                                                                        \
                        on_assert_success(std::cout);                                               \
                    }                                                                               \
                }                                                                                   \
            }                                                                                       \
        }                                                                                           \
    } while (0)

#define SB_CASE(module, name)                                                                       \
    struct SBTestingCase##module##name : public sb::BaseTestingCase                                 \
    {                                                                                               \
        SBTestingCase##module##name() : sb::BaseTestingCase(#module, #name) {}                      \
        virtual void body() override;                                                               \
    };                                                                                              \
    static SBTestingCase##module##name SbTestingCase##_##module##_##name;                           \
    void SBTestingCase##module##name::body()
} // namespace sb

#endif // SB_HPP_

#define SB_IMPLEMENTATION

#ifdef SB_IMPLEMENTATION

#include <iostream>
#include <stdint.h>
#include <unistd.h>
#include <stdarg.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/stat.h>

#ifdef __APPLE__
#   include <mach-o/dyld.h>
#endif

#ifdef SB_TESTING_MAIN
#define SB_IMPLEMENTATION

int main(int argc, char **argv)
{
    sb::TestingCases::instance()->test_all();
    sb::TestingCases::instance()->report();
    return sb::TestingCases::instance()->failed_cnt();
}

#endif // SB_TESTING_MAIN

namespace sb
{

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

class Object
{
public:
    Object(const std::string& src, const Entity* entity);
    int compile() const;

    const std::string src;
    const std::string name;
    const std::string path;
private:
    const Entity* entity_;
}; // class Object

std::string sb_dir();
bool mkdir_if_not_exists(const std::string &dir);
int copy_file(const char *src, const char *dest);
bool should_compile(const std::string& src, const std::string& target);
void print_command(const std::vector<std::string>& cmds);
std::vector<std::string> split_string(const char* str);
std::vector<std::string> split_string(const std::string_view& str);
std::vector<std::string> str_split_with_comma(const char* str);
std::string trim_right(const std::string& str);
std::string trim_left(const std::string& str);
std::string trim(const std::string& str);

std::string default_compiler__ = "c++";
std::string default_cxx_compiler__ = "c++";
std::string default_build_dir__ = "./";

void set_default_compiler(const char* compiler)
{
    if (!compiler) {
        return;
    }
    default_compiler__ = compiler;
}

bool auto_rebuild_self__(int argc, char** argv, const char* src)
{
    if (!argv || argc == 0) {
        std::cerr << "Invalid argc argv.\n";
        return false;
    }

    uint64_t bin_mtime;
    uint64_t src_mtime;
    struct stat st;

    const char* tmp_suffix = ".tmp";
    bool is_tmp = strstr(argv[0], tmp_suffix) != NULL;
    std::string tmp_path = argv[0];
    tmp_path += tmp_suffix;
    char sb_name[128];

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

    if (stat(src, &st) == 0) {
        src_mtime = FILE_MTIME_NS(st);
    } else {
        perror("Error src file stats");
        return false;
    }

    if (src_mtime > bin_mtime) {
        // rebuild self
        if (is_tmp) {
            std::cout << "Rebuilding self ..\n";
            if (run_command({default_cxx_compiler__, src, "-o", sb_name}) == 0) {
                std::vector<std::string> cmd{};
                cmd.push_back(sb_name);
                for (int i = 1; i < argc; ++i) {
                    cmd.push_back(argv[i]);
                }
                exit(run_command(cmd));
            } else {
                // restore sb file if compiling new sb failed
                copy_file(tmp_path.c_str(), sb_name);
                remove(tmp_path.c_str());
                exit(-1);
            }
        }
        copy_file(argv[0], tmp_path.c_str());
        if (chmod(tmp_path.c_str(), S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH) != 0) {
            perror("Failed to set executable permissions");
            return false;
        }
        std::vector<std::string> cmd{};
        cmd.push_back(tmp_path);
        for (int i = 1; i < argc; ++i) {
            cmd.push_back(argv[i]);
        }
        exit(run_command(cmd));
    } else if (stat(tmp_path.c_str(), &st) == 0) {
        remove(tmp_path.c_str());
    }
    return true;
}

int run_command(const std::vector<std::string>& cmd)
{
    if (cmd.empty()) {
        return -1;
    }

    const char* cmd_arr[cmd.size() + 1];
    for (size_t i = 0; i < cmd.size(); ++i) {
        cmd_arr[i] = cmd.at(i).c_str();
    }
    cmd_arr[cmd.size()] = nullptr;

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    }
    
    int status = 0;
    if (pid == 0) {
        execvp(cmd_arr[0], (char* const*)cmd_arr);
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

int run_command(const char* first, ...)
{
    std::vector<std::string> cmds{};
    va_list args;
    va_start(args, first);
        const char* curr = first;
        while (curr) {
            cmds.push_back(std::string(curr));
            curr = va_arg(args, const char*);
        }
    va_end(args);

    return run_command(cmds);
}

std::string shell(const std::vector<std::string>& cmds)
{
    int pipefd[2];
    pid_t pid;
    std::string output;
    char buff[512];
    ssize_t bytes_read;

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
        const char* cmd_arr[cmds.size() + 1];
        for (size_t i = 0; i < cmds.size(); ++i) {
            cmd_arr[i] = cmds.at(i).c_str();
        }
        cmd_arr[cmds.size()] = nullptr;
        execvp(cmd_arr[0], (char* const*) cmd_arr);
        perror("execvp");
    } else {
        close(pipefd[1]); 
        while ((bytes_read = read(pipefd[0], buff, sizeof(buff) - 1)) > 0) {
            output += std::string(buff, buff + bytes_read);
        }
        close(pipefd[0]); 
        int status;
        waitpid(pid, &status, 0); 
    }

    return trim(output);
}

std::string shell(const char* first, ...)
{
    std::vector<std::string> cmds{};
    va_list args;
    va_start(args, first);
        const char* curr = first;
        while (curr) {
            cmds.push_back(std::string(curr));
            curr = va_arg(args, const char*);
        }
    va_end(args);

    return shell(cmds);
}

std::string shell_s(const char* cmd_str)
{
    return shell(split_string(cmd_str));
}

Object::Object(const std::string& src, const Entity* entity)
    : entity_(entity)
    , src(src)
    , name(src + ".o")
    , path(entity->build_dir_ + name)
{}

int Object::compile() const
{
    if (entity_->always_build_ || should_compile(src, path)) {
        std::vector<std::string> cmd{};
        cmd.push_back(entity_->compiler_);
        cmd.push_back(src);
        cmd.push_back("-c");
        for (const auto& flag : entity_->flags_) {
            cmd.push_back(flag);
        }
        cmd.push_back("-Wno-unused-command-line-argument");
        cmd.push_back("-o");
        cmd.push_back(path);

        print_command(cmd);
        return run_command(cmd);
    }
    return 0;
}

Entity::Entity(const std::string& name, TargetType tt)
    : compiler_(default_compiler__)
    , build_dir_(default_build_dir__)
    , path_(build_dir_ + name)
    , name(name)
    , tt_(tt)
    , always_build_(false)
    , status_(0)
{}

Entity create_lib(const std::string& name)
{
    return Entity(name, Entity::LIBRARY);
}

Entity create_exe(const std::string& name)
{
    return Entity(name, Entity::EXECUTABLE);
}

Entity& Entity::add_srcs(const std::string_view& file)
{
    check_append_obj(std::string(file));
    return *this;
}

Entity& Entity::add_srcs(const std::vector<std::string>& files)
{
    for (const auto& file : files) {
        check_append_obj(file);
    }
    return *this;
}

Entity& Entity::add_flags(const std::string_view& flag)
{
    const std::vector<std::string> flags = split_string(flag);
    return add_flags(flags);
}

Entity& Entity::add_flags(const std::vector<std::string>& flags)
{
    for (const auto& flag : flags) {
        check_append_flag(flag);
    }
    return *this;
}

#if __cplusplus >= 201703L

template<typename... Args>
Entity& Entity::add_srcs(const std::string_view& file, Args... args)
{
    check_append_obj(std::string(file));
    return add_srcs(args...);
}

template<typename... Args>
Entity& Entity::add_flags(const std::string_view& flag, Args... args)
{
    check_append_flag(std::string(flag));
    return add_flags(args...);
}

#endif //__cplusplus >= 201703L

Entity& Entity::build()
{   
    std::vector<std::string> cmd;
    cmd.push_back(compiler_);
    bool should_build = always_build_;
    for (const auto& obj : objs_) {
        if (obj.compile() != 0) {
            status_ = -1;
            return *this;
        }
        if (should_build || should_compile(obj.path, path_)) {
            should_build = true;
        }
        cmd.push_back(obj.path);
    }
    for (const auto& f: flags_) {
        cmd.push_back(f);
    }
    cmd.push_back("-Wno-unused-command-line-argument");
    cmd.push_back("-o");
    cmd.push_back(path_);

    if (should_build) {
        print_command(cmd);
        status_ = run_command(cmd);
    } else {
        status_ = 0;
    }
    return *this;
}

int Entity::status() const
{
    return status_;
}

const std::string& Entity::path() const
{
    return path_;
}


Entity& Entity::set_compiler(const std::string& compiler)
{
    compiler_ = compiler;
    return *this;
}

Entity& Entity::always_build(bool sure)
{
    always_build_ = sure;
    return *this;
}

bool Entity::check_append_flag(const std::string& flag)
{
    if (flag.empty()) {
        return false;
    }

    for (const auto& e_flag : excluded_flags) {
        if (flag == e_flag) {
            std::cerr << "Excluded compile flag `" << flag << "` found.";
            return false;
        }
    }

    flags_.push_back(flag);
    return true;
}

bool Entity::check_append_obj(const std::string& src)
{
    for (const auto obj: objs_) {
        if (src == obj.src) {
            std::cerr << "Duplicated src file `" << src << "` found.";
            return false;
        }
    }

    objs_.push_back(Object(src, this));
    return true;
}

int copy_file(const char *src, const char *dest) {
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

void set_build_dir(const char* build_dir)
{
    default_build_dir__ = std::string(build_dir) + "/";
    if (!mkdir_if_not_exists(default_build_dir__)) {
        abort();
    }
}

ArgParser* ArgParser::instance()
{
    static ArgParser *instance_ = nullptr;
    static std::once_flag flag;
    if (!instance_) {
        std::call_once(flag, [&]() -> void {
            instance_ = new (std::nothrow) ArgParser();
        });
    }
    return instance_;
}

ArgParser::ArgParser()
    : self_anme_(nullptr)
    , scheme_idx_(0)
{}

ArgParser::Scheme::Scheme()
    : type(ST_UNKNOWN)
{}

ArgParser::Scheme::~Scheme()
{}

ArgParser::Scheme& ArgParser::add_scheme(const char *key, const char *help)
{
    if (scheme_idx_ >= SB_MAX_ARGS) {
        std::cout << "tooo much args to parse\n";
        abort();
    }
    ArgParser::Scheme& scheme = scheme_list_[scheme_idx_++];
    scheme.help = help;
    scheme.key = key;
    return scheme;
}

std::vector<std::string> str_split_with_comma(const char* str)
{
    std::vector<std::string> output;
    if (str) {
        output.push_back(std::string());
        while (*str) {
            if (*str != ',') {
                output.back().push_back(*str);
            } else {
                output.push_back(std::string());
            }
            ++str;
        }
    }
    return output;
}

bool flags_parse(int argc, char **argv)
{
    ArgParser* arg_parser = ArgParser::instance();
    if (argc < 1) {
        return false;
    }

    for (int i = 0; i < argc; ++i) {
        const char* this_arg = argv[i];
        bool arg_matched = false;
        if(i == 0) {
            arg_parser->self_anme_ = this_arg;
            continue;
        }

        for (int j = 0; j < arg_parser->scheme_idx_ && !arg_matched; ++j) {
            ArgParser::Scheme& scheme = arg_parser->scheme_list_[j];
            if (strcmp(scheme.key, this_arg) == 0) {
                arg_matched = true;
                const char* next_arg = i < argc ? argv[i + 1] : nullptr;
                switch (scheme.type) {
                case ArgParser::ST_STR:
                    if (!next_arg) return false;
                    scheme.str = next_arg;
                    ++i;
                    break;
                case ArgParser::ST_INT:
                    if (!next_arg) return false;
                    scheme.i = std::stoi(next_arg);
                    ++i;
                    break;
                case ArgParser::ST_FLOAT:
                    if (!next_arg) return false;
                    scheme.f = std::stof(next_arg);
                    ++i;
                    break;
                case ArgParser::ST_BOOL:
                    if (!next_arg || strcmp(next_arg, "true") == 0) {
                        scheme.b = true;
                        ++i;
                    } else if (next_arg && strcmp(next_arg, "false") == 0) {
                        scheme.b = false;
                        ++i;
                    } else {
                        scheme.b = true;
                    }
                    break;
                case ArgParser::ST_VSTR: {
                    if (!next_arg) return false;
                    scheme.vstr.clear();
                    scheme.vstr = str_split_with_comma(next_arg);
                    ++i;
                } break;
                case ArgParser::ST_VINT: {
                    if (!next_arg) return false;
                    std::vector<std::string> vstr = str_split_with_comma(next_arg);
                    scheme.vi.clear();
                    for (const auto s : vstr) {
                        if (!s.empty()) {
                            scheme.vi.push_back(std::stoi(s));
                        } else {
                            return false;
                        }
                    }
                    ++i;
                } break;
                case ArgParser::ST_VFLOAT: {
                    if (!next_arg) return false;
                    std::vector<std::string> vstr = str_split_with_comma(next_arg);
                    scheme.vf.clear();
                    for (const auto s : vstr) {
                        if (!s.empty()) {
                            scheme.vf.push_back(std::stof(s));
                        } else {
                            return false;
                        }
                    }
                    ++i;
                } break;
                case ArgParser::ST_UNKNOWN:
                default:
                    std::cout << "Unknown argument type for " << this_arg << "\n";
                    return false;
                }
                continue;
            }
        }
        if (!arg_matched) {
            std::cout << "Unknown argument: " << this_arg << "\n";
            return false;
        }
    }
    return true;
}

template<>
bool& flags_arg<bool>(const char *key, bool default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.b = default_val;
    scheme.type = ArgParser::ST_BOOL;
    return scheme.b;
}

template<>
int& flags_arg<int>(const char *key, int default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.i = default_val;
    scheme.type = ArgParser::ST_INT;
    return scheme.i;
}

template<>
float& flags_arg<float>(const char *key, float default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.f = default_val;
    scheme.type = ArgParser::ST_FLOAT;
    return scheme.f;
}

template<>
std::string& flags_arg<std::string>(const char *key, std::string default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.str = default_val;
    scheme.type = ArgParser::ST_STR;
    return scheme.str;
}

template<>
std::vector<std::string>& flags_arg<std::vector<std::string>>(const char *key, std::vector<std::string> default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.vstr = default_val;
    scheme.type = ArgParser::ST_VSTR;
    return scheme.vstr;
}

template<>
std::vector<int>& flags_arg<std::vector<int>>(const char *key, std::vector<int> default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.vi = default_val;
    scheme.type = ArgParser::ST_VINT;
    return scheme.vi;
}

template<>
std::vector<float>& flags_arg<std::vector<float>>(const char *key, std::vector<float> default_val, const char *help)
{
    ArgParser* arg_parser = ArgParser::instance();
    ArgParser::Scheme& scheme = arg_parser->add_scheme(key, help);
    scheme.vf = default_val;
    scheme.type = ArgParser::ST_VFLOAT;
    return scheme.vf;
}

void flags_show_usage()
{
    const int help_align_len = 32;

    ArgParser* arg_parser = ArgParser::instance();
    std::cout << "Usage for " << arg_parser->self_anme_ << "\n";
    std::string help_line;
    help_line.reserve(64);

    for (int i = 0; i < arg_parser->scheme_idx_; ++i) {
        const auto& scheme = arg_parser->scheme_list_[i];
        help_line += "  ";
        help_line += scheme.key;
        help_line += ' ';

        switch (scheme.type) {
        case ArgParser::ST_STR:
            help_line += "<str>";
            break;
        case ArgParser::ST_INT:
            help_line += "<int>";
            break;
        case ArgParser::ST_FLOAT:
            help_line += "<float>";
            break;
        case ArgParser::ST_BOOL:
            help_line += "<[true|false]>";
            break;
        case ArgParser::ST_VSTR:
            help_line += "<str,str,..>";
            break;
        case ArgParser::ST_VINT:
            help_line += "<int,int,..>";
            break;
        case ArgParser::ST_VFLOAT:
            help_line += "<float,float,..>";
            break;
        default:
            break;
        }

        std::cout << help_line;
        int help_line_len = help_line.length();
        if (help_line_len > help_align_len) {
            std::cout << "\n";
            help_line_len = 0;
        }
        for (int j = 0; j < help_align_len - help_line_len; ++j) {
            std::cout << ' ';
        }
        std::cout << scheme.help << "\n";
        help_line.clear();
    }
    std::cout << "\n";
}

BaseTestingCase::BaseTestingCase(const std::string& module, const std::string& name)
    : __module(module)
    , __name(name)
    , __failed_assert(0)
    , __success_assert(0)
    , __padding_len(__module_name_len - name.length() - module.length())
{
    TestingCases::instance()->add_case(this);
}

std::ostream& BaseTestingCase::on_assert_success(std::ostream& os)
{
    ++__success_assert;
    return os;
}

std::ostream& BaseTestingCase::on_assert_failed(std::ostream& os, const char *filename, int line,
    const char *checking)
{
    ++__failed_assert;
    os << ">> Case " << __module << '.' << __name << " failed on checking "
       << SB_TERM_COLOR_HL_S << checking << SB_TERM_COLOR_E << ", "
       << filename << ':' << line << '\n';
    return os;
}

void BaseTestingCase::report(std::ostream& os) const
{
    if (__failed_assert == 0) {
        os << SB_TERM_COLOR_SUCC_S << __module << '.' << __name << ' ';
        for (int i = 0; i < __padding_len; ++i) {
            os << '.';
        }
        os << " PASSED\n";
    } else {
        os << SB_TERM_COLOR_FAIL_S << __module << '.' << __name << ' ';
        for (int i = 0; i < __padding_len; ++i) {
            os << '.';
        }
        os << " FAILED\n";
    }
    os << SB_TERM_COLOR_E;
}

TestingCases* TestingCases::instance()
{
    static TestingCases *instance_ = nullptr;
    static std::once_flag flag;
    if (!instance_) {
        std::call_once(flag, [&]() -> void {
            instance_ = new (std::nothrow) TestingCases();
        });
    }
    return instance_;
}

TestingCases::TestingCases()
    : passed_cnt_(0)
    , failed_cnt_(0)
{}

void TestingCases::add_case(BaseTestingCase* c)
{
    cases_.push_back(c);
}

const std::vector<BaseTestingCase*>& TestingCases::cases() const
{
    return cases_;
}

void TestingCases::test_all()
{
    for (sb::BaseTestingCase* c : cases_) {
        c->body();
        c->report(std::cout);

        if (c->__failed_assert > 0) {
            ++failed_cnt_;
        } else {
            ++passed_cnt_;
        }
    }
}

void TestingCases::report() const
{
    std::cout << "=============================\n";
    std::cout << "    " << cases_.size() << " cases tested\n"
              << "    " << passed_cnt_ << SB_TERM_COLOR_SUCC_S << " passed\n" << SB_TERM_COLOR_E
              << "    " << failed_cnt_ << SB_TERM_COLOR_FAIL_S << " failed\n" << SB_TERM_COLOR_E;
    std::cout << "=============================\n";
}

bool mkdir_if_not_exists(const std::string &dir)
{
    const mode_t mode = 0755;
    struct stat st;
    char buff[dir.length() + 1];
    for (size_t i = 0; i < dir.length(); ++i) {
        char ch = dir.at(i);
        buff[i] = ch;
        buff[i + 1] = '\0';
        if (ch == '/') {
            if(stat(buff, &st) != 0) {
                if (::mkdir(buff, mode) != 0) {
                    return false;
                }
            }
        }
    }
    return true;
}

void print_command(const std::vector<std::string>& cmd)
{
    for (size_t i = 0; i < cmd.size(); ++i) {
        std::cout << cmd.at(i);
        if (i == cmd.size() - 1)
            std::cout << "\n";
        else
            std::cout << " ";
    }
}

bool should_compile(const std::string& src, const std::string& target)
{
   bool result = true;
    struct stat st;
    if (stat(src.c_str(), &st) == 0) {
        uint64_t src_mtime = FILE_MTIME_NS(st);
        if (stat(target.c_str(), &st) == 0) {
            uint64_t obj_mtime = FILE_MTIME_NS(st);
            if (obj_mtime > src_mtime) {
                result = false;
            }
        }
    } else {
        std::cerr << "source file " << src << " not found\n";
        return -1;
    }
    return result;
}

std::vector<std::string> split_string(const char* str)
{
    std::vector<std::string> output{};
    size_t last = 0;
    size_t i = 0;

    for (;; ++i) {
        char ch = str[i];
        if (ch == '\0') {
            if (last != i) {
                output.emplace_back(str + last, str + i);
            }
            break;
        }

        switch (ch) {
        case ' ':
        case '\n':
        case '\t':
            if (last != i) {
                output.emplace_back(str + last, str + i);
            }
            last = i + 1;
            break;
        default:
            break;
        }
    }
    return output;
}

std::string sb_dir()
{
    std::string output;
    output.resize(256);
    uint32_t len = output.length();
#ifdef __APPLE__
    if (_NSGetExecutablePath(&output[0], &len) != 0) {
        abort();
    }
#else // __APPLE__
    const char *self_exe = "/proc/self/exe";
    len = readlink(self_exe, output.data(), len);
    if (len >= sizeof(output.size())) {
        abort();
    }
#endif // __APPLE__

    size_t last_slash = 0;
    for (size_t i = 0; i < output.size(); ++i) {
        if (output.at(i) == '/') {
            last_slash = i;
        }
    } 
    return output.substr(0, last_slash + 1);
}

std::vector<std::string> split_string(const std::string_view& str)
{
    return split_string(str.data());
}

std::string trim_left(const std::string& str)
{
    auto const start = std::find_if_not(str.begin(), str.end(), ::isspace);
    return str.substr(std::distance(str.begin(), start));
}

std::string trim_right(const std::string& str)
{
    auto const end = std::find_if_not(str.rbegin(), str.rend(), ::isspace).base();
    return str.substr(0, std::distance(str.begin(), end));
}

std::string trim(const std::string& str)
{
    return trim_left(trim_right(str));
}

} // namespace sb

#endif // SB_IMPLEMENTATION
