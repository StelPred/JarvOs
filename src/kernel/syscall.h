#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>

// Basic type definitions for compatibility
typedef int             pid_t;
typedef unsigned int    uid_t;
typedef unsigned int    gid_t;
typedef unsigned int    mode_t;
typedef unsigned int    dev_t;
typedef long            off_t;
typedef long            ssize_t;
typedef unsigned int    key_t;
typedef unsigned int    socklen_t;
typedef long            time_t;

// Forward declarations of structures (minimal definitions for now)
struct stat {
    unsigned long st_mode;
    unsigned long st_ino;
    unsigned long st_dev;
    unsigned long st_nlink;
    unsigned int  st_uid;
    unsigned int  st_gid;
    long          st_size;
    long          st_atime;
    long          st_mtime;
    long          st_ctime;
};

struct utimbuf {
    long actime;
    long modtime;
};

struct flock {
    short l_type;
    short l_whence;
    long  l_start;
    long  l_len;
    long  l_pid;
};

struct itimerval {
    struct timeval {
        long tv_sec;
        long tv_usec;
    } it_interval;
    struct timeval it_value;
};

struct statfs {
    long f_type;
    long f_bsize;
    long f_blocks;
    long f_bfree;
    long f_bavail;
    long f_files;
    long f_ffree;
    long f_fsid[2];
    long f_namelen;
    long f_frsize;
    long f_spare[5];
};

struct timespec {
    long tv_sec;
    long tv_nsec;
};

// IPC permission structure (simplified)
struct ipc_perm {
    unsigned short uid;   // owner's user id
    unsigned short gid;   // owner's group id
    unsigned short cuid;  // creator's user id
    unsigned short cgid;  // creator's group id
    unsigned short mode;  // access modes
    unsigned short seq;   // sequence number
};

// Socket address structure (simplified)
struct sockaddr {
    unsigned short sa_family; // address family
    char sa_data[14];         // address data
};

struct sysinfo {
    long uptime;
    unsigned long loads[3];
    unsigned long totalram;
    unsigned long freeram;
    unsigned long sharedram;
    unsigned long bufferram;
    unsigned long totalswap;
    unsigned long freeswap;
    unsigned short procs;
    unsigned short pad;
    unsigned long totalhigh;
    unsigned long freehigh;
    unsigned int mem_unit;
    char _f[20-2*sizeof(long)-sizeof(int)];
};

struct tms {
    long tms_utime;
    long tms_stime;
    long tms_cutime;
    long tms_cstime;
};

struct msqid_ds {
    struct ipc_perm msg_perm;
    long msg_first;
    long msg_last;
    long msg_cbytes;
    long msg_qnum;
    long msg_qbytes;
    long msg_lspid;
    long msg_lrpid;
    long msg_stime;
    long msg_rtime;
    long msg_ctime;
};

struct shmid_ds {
    struct ipc_perm shm_perm;
    int shm_segsz;
    time_t shm_atime;
    time_t shm_dtime;
    time_t shm_ctime;
    pid_t shm_cpid;
    pid_t shm_lpid;
    unsigned short shm_nattch;
    unsigned short shm_natts;
};

struct sembuf {
    unsigned short sem_num;
    short sem_op;
    short sem_flg;
};

// Registers structure for interrupt handling
// This matches the layout created by our syscall_stub.asm
typedef struct registers_t {
    uint32_t gs, fs, es, ds;      // Segment registers
    uint32_t eax, ecx, edx, ebx;  // General purpose registers (in the order we pushed them)
    uint32_t esp_orig;            // Original ESP value
    uint32_t ebp, esi, edi;       // More general purpose registers
} registers_t;

// System call numbers
#define SYS_EXIT        1
#define SYS_FORK        2
#define SYS_READ        3
#define SYS_WRITE       4
#define SYS_OPEN        5
#define SYS_CLOSE       6
#define SYS_WAITPID     7
#define SYS_CREAT       8
#define SYS_LINK        9
#define SYS_UNLINK     10
#define SYS_EXECVE     11
#define SYS_CHDIR      12
#define SYS_TIME       13
#define SYS_MKNOD      14
#define SYS_CHMOD      15
#define SYS_CHOWN      16
#define SYS_BREAK      17
#define SYS_STAT       18
#define SYS_LSEEK      19
#define SYS_GETPID     20
#define SYS_MOUNT      21
#define SYS_UMOUNT     22
#define SYS_SETUID     23
#define SYS_GETUID     24
#define SYS_STIME      25
#define SYS_PTRACE     26
#define SYS_ALARM      27
#define SYS_FSTAT      28
#define SYS_PAUSE      29
#define SYS_UTIME      30
#define SYS_ACCESS     33
#define SYS_NICE       34
#define SYS_SYNC       36
#define SYS_KILL       37
#define SYS_RENAME     38
#define SYS_MKDIR      39
#define SYS_RMDIR      40
#define SYS_DUP        41
#define SYS_PIPE       42
#define SYS_TIMES      43
#define SYS_PROF       44
#define SYS_BRK        45
#define SYS_SETGID     46
#define SYS_GETGID     47
#define SYS_SIGNAL     48
#define SYS_GETEUID    49
#define SYS_GETEGID    50
#define SYS_ACCT       51
#define SYS_UMOUNT2    52
#define SYS_LOCK       53
#define SYS_IOCTL      54
#define SYS_FCNTL      55
#define SYS_MPG        56
#define SYS_SETSETID   57
#define SYS_XSWAP      58
#define SYS_FHSTAT     59
#define SYS_SYSLOG     60
#define SYS_SETITIMER  61
#define SYS_GETITIMER  62
#define SYS_STATFS     63
#define SYS_GETPRIORITY 64
#define SYS_SETPRIORITY 65
#define SYS_TIMESPEC_GET 66
#define SYS_MSGGET     67
#define SYS_MSGSND     68
#define SYS_MSGRCV     69
#define SYS_MSGCTL     70
#define SYS_SEMAPHORE  71
#define SYS_SEMOP      72
#define SYS_SHMGET     73
#define SYS_SHMAT      74
#define SYS_SHMDT      75
#define SYS_SHMCTL     76
#define SYS_SOCKET     77
#define SYS_BIND       78
#define SYS_CONNECT    79
#define SYS_LISTEN     80
#define SYS_ACCEPT     81
#define SYS_SEND       82
#define SYS_RECV       83
#define SYS_SENDTO     84
#define SYS_RECVFROM   85
#define SYS_SETSOCKOPT 86
#define SYS_GETSOCKOPT 87
#define SYS_SOCKETPAIR 88
#define SYS_SYSINFO    99

// IPC flags
#define IPC_CREAT  00001000  // create if key is nonexistent
#define IPC_EXCL   00002000  // fail if key exists
#define IPC_NOWAIT 00004000  // return error on wait
#define IPC_PRIVATE 0        // private key
#define IPC_RMID   0         // remove resource
#define IPC_SET    1         // set resource options
#define IPC_STAT   2         // get resource options

// System call handler prototype
void syscall_handler(registers_t *regs);

// Initialize system calls
void syscall_init();

#endif /* _SYSCALL_H */