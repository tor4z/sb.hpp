#ifndef SB_HPP_
#define SB_HPP_

#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>
#include <unistd.h>
#include <stdint.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <stdarg.h>

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

class Entity
{
public:
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
    const TargetType tt_;
    bool always_build_;
    int status_;
}; // class Entity

bool auto_rebuild_self__(int argc, char** argv, const char* src);
int run_command(const std::vector<std::string>& cmd);
int run_command(const char* first, ...);
Entity create_lib(const std::string& name);
Entity create_exe(const std::string& name);

#define auto_rebuild_self(argc, argv) auto_rebuild_self__(argc, argv, __FILE__)
} // namespace sb

#endif // SB_HPP_

#define SB_IMPLEMENTATION

#ifdef SB_IMPLEMENTATION

namespace sb
{

#define SB_SEC_TO_NS        (1000 * 1000 * 1000)
#define TIMESPEC_TO_NS(ts)  ((ts).tv_nsec + (ts).tv_sec * SB_SEC_TO_NS)

class Object
{
public:
    Object(const std::string& src, const Entity* entity);
    int compile() const;

    const std::string src;
    const std::string name;
private:
    const Entity* entity_;
}; // class Object

int copy_file(const char *src, const char *dest);
void print_command(const std::vector<std::string>& cmds);

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
        bin_mtime = TIMESPEC_TO_NS(st.st_mtim);
    } else {
        perror("Error bin file stats");
        return false;
    }

    if (stat(src, &st) == 0) {
        src_mtime = TIMESPEC_TO_NS(st.st_mtim);
    } else {
        perror("Error src file stats");
        return false;
    }

    if (src_mtime > bin_mtime) {
        // rebuild self
        if (is_tmp) {
            std::cout << "Rebuilding self ..\n";
            if (run_command({"c++", src, "-o", sb_name}) == 0) {
                std::vector<std::string> cmd{};
                cmd.push_back(sb_name);
                for (int i = 1; i < argc; ++i) {
                    cmd.push_back(argv[i]);
                }
                exit(run_command(cmd));
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
        execvp(cmd_arr[0], (char*const*)cmd_arr);
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

Object::Object(const std::string& src, const Entity* entity)
    : entity_(entity)
    , src(src)
    , name(src + ".o")
{}

int Object::compile() const
{
    bool should_compile = true;
    struct stat st;
    if (stat(src.c_str(), &st) == 0) {
        uint64_t src_mtime = TIMESPEC_TO_NS(st.st_mtim);
        if (stat(name.c_str(), &st) == 0) {
            uint64_t obj_mtime = TIMESPEC_TO_NS(st.st_mtim);
            if (obj_mtime > src_mtime && !entity_->always_build_) {
                should_compile = false;
            }
        }
    } else {
        std::cerr << "source file " << src << " not found\n";
        return -1;
    }

    if (should_compile) {
        std::vector<std::string> cmd{};
        cmd.push_back(entity_->compiler_);
        cmd.push_back(src);
        cmd.push_back("-c");
        for (const auto& flag : entity_->flags_) {
            cmd.push_back(flag);
        }
        cmd.push_back("-o");
        cmd.push_back(name);

        print_command(cmd);
        return run_command(cmd);
    }
    return 0;
}

Entity::Entity(const std::string& name, TargetType tt)
    : compiler_("c++")
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
    check_append_flag(std::string(flag));
    return *this;
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
    for (const auto& obj : objs_) {
        if (obj.compile() != 0) {
            status_ = -1;
            return *this;
        }
        cmd.push_back(obj.name);
    }
    for (const auto& f: flags_) {
        cmd.push_back(f);
    }
    cmd.push_back("-o");
    cmd.push_back(name);

    print_command(cmd);
    status_ = run_command(cmd);
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


} // namespace sb

#endif // SB_IMPLEMENTATION
