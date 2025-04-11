# 🗂️ Apex Custom Virtual File System (CVFS)

**Apex CVFS** is a fully functional command-line based virtual file system implemented in C++. It mimics the structure and behavior of a real file system, offering users basic file management commands such as `create`, `read`, `write`, `ls`, `stat`, and more — all within a simulated environment.

---

## 🚀 Features

- Create regular files with permission control.
- Read from and write to virtual files.
- List all created files with metadata (`ls`).
- View file information using `stat` or `fstat`.
- File deletion via `rm`.
- Custom command set for file handling: `open`, `close`, `closeall`, `truncate`, `lseek`, `man`, and `help`.
- In-memory file system using a dynamic inode-based structure.
- Maintains a superblock and user file descriptor table (UFDT) for tracking file usage.

---

## 🛠️ Technologies Used

- **Language**: C++
- **Concepts**: File systems, Data structures (Linked Lists, Tables), Command-line interface, Pointer manipulation, Memory management

---

## 🗃️ Core Components

- **Superblock**: Manages total and free inodes.
- **Inode Table (DILB)**: Represents files and their metadata.
- **UFDT (User File Descriptor Table)**: Keeps track of open file descriptors and modes.
- **FileTable**: Maintains read/write offsets and reference counts for each file.

---

## 📚 Supported Commands

| Command | Description |
|--------|-------------|
| `create <filename> <permission>` | Create a new file (permission: 1-Read, 2-Write, 3-Read+Write) |
| `open <filename> <mode>` | Open file with specific mode |
| `write <filename>` | Write data to a file |
| `read <filename> <no_of_bytes>` | Read data from a file |
| `ls` | List all files with basic metadata |
| `stat <filename>` | Display detailed file info |
| `fstat <fd>` | Show file info using file descriptor |
| `close <filename>` | Close a file |
| `closeall` | Close all opened files |
| `truncate <filename>` | Remove all data from a file |
| `rm <filename>` | Delete a file |
| `lseek <filename> <size> <START/CURRENT/END>` | Move the file offset |
| `help` / `man <command>` | Display usage guide |
| `exit` | Terminate the CVFS session |

---

## 🧪 Sample Usage

```bash
> create demo.txt 3
File is Successfully Created with file descriptor : 3.

> open demo.txt 3
File is successfully opened with file descriptor : 3

> write demo.txt
Enter the Data:
Hello World!

> read demo.txt 11
Hello World!

> ls
File Name    Inode Number    File Size    Link Count
----------------------------------------------------
demo.txt     1               11           1
----------------------------------------------------
