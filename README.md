# MyShell – A Simple Unix-Style File Navigation Shell

This is a custom shell program written in C as a project for CSE 3320: Operating Systems.  
It provides a user-friendly, paginated command-line interface for navigating directories, viewing files, editing files, running programs, and sorting file listings.

## 📂 Features

- 📁 List contents of the current directory (files and folders)
- ⬅️➡️ Paginate directory listings (5 items per page)
- 📝 Edit files using `nano`
- 🚀 Run executable files in the directory
- 🔃 Sort files by size or modification date
- 📂 Navigate to parent or child directories
- 🧭 Display current path and system time
- 🧼 Clears the screen after each operation for a cleaner UI

## 🛠️ Technologies Used

- Language: C (compiled with GCC)
- OS: Linux (WSL, Ubuntu, or native)
- Tools: Standard POSIX libraries (`dirent.h`, `unistd.h`, `sys/stat.h`, `time.h`)

## 📦 Files

| File | Description |
|------|-------------|
| `myshell.c` | Main source file for the shell logic |
| `Makefile` | Allows compilation with `make` |
| `CMakeLists.txt` (optional) | For building with CMake |
| `hello.c`, `CursesCounter.c` | Example C files for testing executable support |
| `Testing/`, `TestDir/` | Sample directories/files to explore in the shell |

## 🚀 How to Build and Run

### 🔧 Build with Make

```bash
make
```

### 🔧 Run the shell or start the shell in a target directory

```bash
./myshell
```
```bash
./myshell /path/to/start/dir
```

<img width="907" height="547" alt="image" src="https://github.com/user-attachments/assets/54ccc5b6-5f39-49d6-8c0f-d68f356c9515" />

