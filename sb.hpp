#ifndef SB_HPP_
#define SB_HPP_

#include <string>
#include <string_view>
#include <vector>

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
} // namespace sb

#endif // SB_HPP_

#define SB_IMPLEMENTATION

#ifdef SB_IMPLEMENTATION

#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <iostream>

#ifdef __APPLE__
#   include <mach-o/dyld.h>
#endif

namespace sb
{

#define SB_SEC_TO_NS        (1000 * 1000 * 1000)
#define TIMESPEC_TO_NS(ts)  ((ts).tv_nsec + (ts).tv_sec * SB_SEC_TO_NS)

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
bool mkdir(const std::string &dir);
int copy_file(const char *src, const char *dest);
bool should_compile(const std::string& src, const std::string& target);
void print_command(const std::vector<std::string>& cmds);
std::vector<std::string> split_string(const char* str);
std::vector<std::string> split_string(const std::string_view& str);
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
        cmd.push_back(obj.name);
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
    default_build_dir__ = sb_dir() + build_dir + "/";
    if (!mkdir(default_build_dir__)) {
        abort();
    }
}

bool mkdir(const std::string &dir)
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
