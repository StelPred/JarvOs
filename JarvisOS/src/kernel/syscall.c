#include <stdint.h>
#include <stddef.h>
#include "hal.h"
#include "syscall.h"
#include "../lib/io.h"
#include "ipc/msg.h"

#define ENOSYS 38

// Forward declarations for process management (to be implemented)
int sys_exit(int status);
int sys_fork(void);
int sys_read(int fd, void *buf, size_t count);
int sys_write(int fd, const void *buf, size_t count);
int sys_open(const char *pathname, int flags);
int sys_close(int fd);
int sys_waitpid(pid_t pid, int *status, int options);
int sys_creat(const char *pathname, mode_t mode);
int sys_link(const char *oldpath, const char *newpath);
int sys_unlink(const char *pathname);
int sys_execve(const char *filename, char *const argv[], char *const envp[]);
int sys_chdir(const char *path);
int sys_time(time_t *tloc);
int sys_mknod(const char *pathname, mode_t mode, dev_t dev);
int sys_chmod(const char *pathname, mode_t mode);
int sys_chown(const char *pathname, uid_t owner, gid_t group);
int sys_break(void *addr);
int sys_stat(const char *path, struct stat *buf);
off_t sys_lseek(int fd, off_t offset, int whence);
pid_t sys_getpid(void);
int sys_mount(const char *dev_name, const char *dir_name, const char *type, unsigned long flags, void *data);
int sys_umount(const char *special_device);
int sys_setuid(uid_t uid);
uid_t sys_getuid(void);
int sys_stime(const time_t *tptr);
int sys_ptrace(long request, pid_t pid, void *addr, void *data);
unsigned int sys_alarm(unsigned int seconds);
int sys_fstat(unsigned int fd, struct stat *statbuf);
int sys_pause(void);
int sys_utime(const char *filename, const struct utimbuf *times);
int sys_access(const char *filename, int mode);
int sys_nice(int inc);
int sys_sync(void);
int sys_kill(pid_t pid, int sig);
int sys_rename(const char *oldname, const char *newname);
int sys_mkdir(const char *pathname, mode_t mode);
int sys_rmdir(const char *pathname);
int sys_dup(unsigned int fildes);
int sys_pipe(int filedes[2]);
struct tms *sys_times(struct tms *buf);
unsigned long sys_prof(unsigned short *buffer, unsigned int bufsiz, unsigned int offset, unsigned int scale);
int sys_brk(void *addr);
gid_t sys_getgid(void);
int sys_setgid(gid_t gid);
int sys_signal(int sig, void (*handler)(int));
uid_t sys_geteuid(void);
gid_t sys_getegid(void);
int sys_acct(const char *name);
int sys_umount2(const char *special_device, int flags);
int sys_lock(int cmd, struct flock *flock);
int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg);
int sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg);
int sys_mpg(void);
int sys_setsetid(void);
int sys_xswap(void);
int sys_fhstat(const char *filename, struct stat *statbuf);
int sys_syslog(int type, char *buf, int len);
int sys_setitimer(int which, const struct itimerval *value, struct itimerval *ovalue);
int sys_getitimer(int which, struct itimerval *value);
int sys_statfs(const char *path, struct statfs *buf);
int sys_getpriority(int which, int who);
int sys_setpriority(int which, int who, int prio);
int sys_timespec_get(struct timespec *ts, int base);
int sys_semget(key_t key, int nsems, int semflg);
int sys_semop(int semid, struct sembuf *sops, size_t nsops);
int sys_shmget(key_t key, size_t size, int shmflg);
long sys_shmat(int shmid, const void *shmaddr, int shmflg);
int sys_shmdt(const void *shmaddr);
int sys_shmctl(int shmid, int cmd, struct shmid_ds *buf);
int sys_socket(int domain, int type, int protocol);
int sys_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int sys_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int sys_listen(int sockfd, int backlog);
int sys_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
long sys_send(int sockfd, const void *buf, size_t len, int flags);
long sys_recv(int sockfd, void *buf, size_t len, int flags);
long sys_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen);
long sys_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
long sys_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);
long sys_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen);
long sys_socketpair(int d, int type, int protocol, int sv[2]);
long sys_sysinfo(struct sysinfo *info);
int sys_msgget(key_t key, int msgflg);
int sys_msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
int sys_msgrcv(int msqid, void *msgp, size_t msgsz, int msgtyp, int msgflg);
int sys_msgctl(int msqid, int cmd, struct msqid_ds *buf);

// Stub implementations for system calls
int sys_fork(void) { return -ENOSYS; }
int sys_read(int fd, void *buf, size_t count) { return -ENOSYS; }
int sys_open(const char *pathname, int flags) { return -ENOSYS; }
int sys_close(int fd) { return -ENOSYS; }
int sys_waitpid(pid_t pid, int *stat_addr, int options) { return -ENOSYS; }
int sys_creat(const char *pathname, mode_t mode) { return -ENOSYS; }
int sys_link(const char *oldpath, const char *newpath) { return -ENOSYS; }
int sys_unlink(const char *pathname) { return -ENOSYS; }
int sys_execve(const char *filename, char *const argv[], char *const envp[]) { return -ENOSYS; }
int sys_chdir(const char *path) { return -ENOSYS; }
int sys_time(time_t *tloc) { return -ENOSYS; }
int sys_mknod(const char *filename, mode_t mode, dev_t dev) { return -ENOSYS; }
int sys_chmod(const char *filename, mode_t mode) { return -ENOSYS; }
int sys_chown(const char *filename, uid_t user, gid_t group) { return -ENOSYS; }
int sys_break(void *addr) { return -ENOSYS; }
int sys_stat(const char *filename, struct stat *statbuf) { return -ENOSYS; }
off_t sys_lseek(int fd, off_t offset, int whence) { return (off_t)-ENOSYS; }
pid_t sys_getpid(void) { return (pid_t)-ENOSYS; }
int sys_mount(const char *dev_name, const char *dir_name, const char *type, unsigned long flags, void *data) { return -ENOSYS; }
int sys_umount(const char *special_device) { return -ENOSYS; }
int sys_setuid(uid_t uid) { return -ENOSYS; }
uid_t sys_getuid(void) { return (uid_t)-ENOSYS; }
int sys_stime(const time_t *tptr) { return -ENOSYS; }
int sys_ptrace(long request, pid_t pid, void *addr, void *data) { return -ENOSYS; }
unsigned int sys_alarm(unsigned int seconds) { return (unsigned int)-ENOSYS; }
int sys_fstat(unsigned int fd, struct stat *statbuf) { return -ENOSYS; }
int sys_pause(void) { return -ENOSYS; }
int sys_utime(const char *filename, const struct utimbuf *times) { return -ENOSYS; }
int sys_access(const char *filename, int mode) { return -ENOSYS; }
int sys_nice(int inc) { return -ENOSYS; }
int sys_sync(void) { return -ENOSYS; }
int sys_kill(pid_t pid, int sig) { return -ENOSYS; }
int sys_rename(const char *oldname, const char *newname) { return -ENOSYS; }
int sys_mkdir(const char *pathname, mode_t mode) { return -ENOSYS; }
int sys_rmdir(const char *pathname) { return -ENOSYS; }
int sys_dup(unsigned int fildes) { return -ENOSYS; }
int sys_pipe(int filedes[2]) { return -ENOSYS; }
struct tms *sys_times(struct tms *buf) { return (struct tms *)-1; }
unsigned long sys_prof(unsigned short *buffer, unsigned int bufsiz, unsigned int offset, unsigned int scale) { return (unsigned long)-1; }
int sys_brk(void *addr) { return -ENOSYS; }
gid_t sys_getgid(void) { return (gid_t)-ENOSYS; }
int sys_setgid(gid_t gid) { return -ENOSYS; }
int sys_signal(int sig, void (*handler)(int)) { return -ENOSYS; }
uid_t sys_geteuid(void) { return (uid_t)-ENOSYS; }
gid_t sys_getegid(void) { return (gid_t)-ENOSYS; }
int sys_acct(const char *name) { return -ENOSYS; }
int sys_umount2(const char *special_device, int flags) { return -ENOSYS; }
int sys_lock(int cmd, struct flock *flock) { return -ENOSYS; }
int sys_ioctl(unsigned int fd, unsigned int cmd, unsigned long arg) { return -ENOSYS; }
int sys_fcntl(unsigned int fd, unsigned int cmd, unsigned long arg) { return -ENOSYS; }
int sys_mpg(void) { return -ENOSYS; }
int sys_setsetid(void) { return -ENOSYS; }
int sys_xswap(void) { return -ENOSYS; }
int sys_fhstat(const char *filename, struct stat *statbuf) { return -ENOSYS; }
int sys_syslog(int type, char *buf, int len) { return -ENOSYS; }
int sys_setitimer(int which, const struct itimerval *value, struct itimerval *ovalue) { return -ENOSYS; }
int sys_getitimer(int which, struct itimerval *value) { return -ENOSYS; }
int sys_statfs(const char *path, struct statfs *buf) { return -ENOSYS; }
int sys_getpriority(int which, int who) { return -ENOSYS; }
int sys_setpriority(int which, int who, int prio) { return -ENOSYS; }
int sys_timespec_get(struct timespec *ts, int base) { return -ENOSYS; }
int sys_semget(key_t key, int nsems, int semflg) { return -ENOSYS; }
int sys_semop(int semid, struct sembuf *sops, size_t nsops) { return -ENOSYS; }
int sys_shmget(key_t key, size_t size, int shmflg) { return -ENOSYS; }
long sys_shmat(int shmid, const void *shmaddr, int shmflg) { return (long)(void *)-1; }
int sys_shmdt(const void *shmaddr) { return -ENOSYS; }
int sys_shmctl(int shmid, int cmd, struct shmid_ds *buf) { return -ENOSYS; }
int sys_socket(int domain, int type, int protocol) { return -ENOSYS; }
int sys_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return -ENOSYS; }
int sys_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) { return -ENOSYS; }
int sys_listen(int sockfd, int backlog) { return -ENOSYS; }
int sys_accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) { return -ENOSYS; }
long sys_send(int sockfd, const void *buf, size_t len, int flags) { return -ENOSYS; }
long sys_recv(int sockfd, void *buf, size_t len, int flags) { return -ENOSYS; }
long sys_sendto(int sockfd, const void *buf, size_t len, int flags, const struct sockaddr *dest_addr, socklen_t addrlen) { return -ENOSYS; }
long sys_recvfrom(int sockfd, void *buf, size_t len, int flags, struct sockaddr *src_addr, socklen_t *addrlen) { return -ENOSYS; }
long sys_setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen) { return -ENOSYS; }
long sys_getsockopt(int sockfd, int level, int optname, void *optval, socklen_t *optlen) { return -ENOSYS; }
long sys_socketpair(int d, int type, int protocol, int sv[2]) { return -ENOSYS; }
long sys_sysinfo(struct sysinfo *info) { return -ENOSYS; }

// Implement sys_write: write to file descriptor
// For now, we only support fd=1 (stdout) which we direct to serial port
int sys_write(int fd, const void *buf, size_t count) {
    if (fd == 1) { // stdout
        hal_serial_write(buf, count);
        return count;
    }
    // For other fd, return -EBADF for now
    return -9; // EBADF
}

// Implement sys_exit: terminate the process
// For now, we just halt the system (since we don't have processes yet)
int sys_exit(int status) {
    hal_serial_write("System exit called\n", 19);
    // In a real OS, we would terminate the current process.
    // For now, we just hang.
    for (;;) {
        __asm__ __volatile__("hlt");
    }
    return 0; // never reached
}

// System call dispatcher
void syscall_handler(registers_t *regs) {
    // Assuming the calling convention:
    // EAX: system call number
    // EBX: arg1
    // ECX: arg2
    // EDX: arg3
    // ESI: arg4
    // EDI: arg5
    // Return value in EAX

    int syscall_num = regs->eax;
    long ret = -38; // Default to ENOSYS

    switch (syscall_num) {
        case SYS_EXIT:
            ret = sys_exit(regs->ebx);
            break;
        case SYS_FORK:
            ret = sys_fork();
            break;
        case SYS_READ:
            ret = sys_read(regs->ebx, (void*)regs->ecx, regs->edx);
            break;
        case SYS_WRITE:
            ret = sys_write(regs->ebx, (const void*)regs->ecx, regs->edx);
            break;
        case SYS_OPEN:
            ret = sys_open((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_CLOSE:
            ret = sys_close(regs->ebx);
            break;
        case SYS_WAITPID:
            ret = sys_waitpid(regs->ebx, (int*)regs->ecx, regs->edx);
            break;
        case SYS_CREAT:
            ret = sys_creat((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_LINK:
            ret = sys_link((const char*)regs->ebx, (const char*)regs->ecx);
            break;
        case SYS_UNLINK:
            ret = sys_unlink((const char*)regs->ebx);
            break;
        case SYS_EXECVE:
            ret = sys_execve((const char*)regs->ebx,
                             (char* const*)regs->ecx,
                             (char* const*)regs->edx);
            break;
        case SYS_CHDIR:
            ret = sys_chdir((const char*)regs->ebx);
            break;
        case SYS_TIME:
            ret = sys_time((time_t*)regs->ebx);
            break;
        case SYS_MKNOD:
            ret = sys_mknod((const char*)regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_CHMOD:
            ret = sys_chmod((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_CHOWN:
            ret = sys_chown((const char*)regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_BREAK:
            ret = sys_break((void*)regs->ebx);
            break;
        case SYS_STAT:
            ret = sys_stat((const char*)regs->ebx, (struct stat*)regs->ecx);
            break;
        case SYS_LSEEK:
            ret = sys_lseek(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_GETPID:
            ret = sys_getpid();
            break;
        case SYS_MOUNT:
            ret = sys_mount((const char*)regs->ebx, (const char*)regs->ecx,
                            (const char*)regs->edx, regs->esi,
                            (void*)regs->edi);
            break;
        case SYS_UMOUNT:
            ret = sys_umount((const char*)regs->ebx);
            break;
        case SYS_SETUID:
            ret = sys_setuid(regs->ebx);
            break;
        case SYS_GETUID:
            ret = sys_getuid();
            break;
        case SYS_STIME:
            ret = sys_stime((const time_t*)regs->ebx);
            break;
        case SYS_PTRACE:
            ret = sys_ptrace(regs->ebx, regs->ecx,
                             (void*)regs->edx, (void*)regs->esi);
            break;
        case SYS_ALARM:
            ret = sys_alarm(regs->ebx);
            break;
        case SYS_FSTAT:
            ret = sys_fstat(regs->ebx, (struct stat*)regs->ecx);
            break;
        case SYS_PAUSE:
            ret = sys_pause();
            break;
        case SYS_UTIME:
            ret = sys_utime((const char*)regs->ebx,
                            (const struct utimbuf*)regs->ecx);
            break;
        case SYS_ACCESS:
            ret = sys_access((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_NICE:
            ret = sys_nice(regs->ebx);
            break;
        case SYS_SYNC:
            ret = sys_sync();
            break;
        case SYS_KILL:
            ret = sys_kill(regs->ebx, regs->ecx);
            break;
        case SYS_RENAME:
            ret = sys_rename((const char*)regs->ebx, (const char*)regs->ecx);
            break;
        case SYS_MKDIR:
            ret = sys_mkdir((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_RMDIR:
            ret = sys_rmdir((const char*)regs->ebx);
            break;
        case SYS_DUP:
            ret = sys_dup(regs->ebx);
            break;
        case SYS_PIPE:
            ret = sys_pipe((int*)regs->ebx);
            break;
        case SYS_TIMES:
            ret = (long)sys_times((struct tms*)regs->ebx);
            break;
        case SYS_PROF:
            ret = sys_prof((unsigned short*)regs->ebx, regs->ecx,
                           regs->edx, regs->esi);
            break;
        case SYS_BRK:
            ret = sys_brk((void*)regs->ebx);
            break;
        case SYS_SETGID:
            ret = sys_setgid(regs->ebx);
            break;
        case SYS_GETGID:
            ret = sys_getgid();
            break;
        case SYS_SIGNAL:
            ret = sys_signal(regs->ebx, (void (*)(int))regs->ecx);
            break;
        case SYS_GETEUID:
            ret = sys_geteuid();
            break;
        case SYS_GETEGID:
            ret = sys_getegid();
            break;
        case SYS_ACCT:
            ret = sys_acct((const char*)regs->ebx);
            break;
        case SYS_UMOUNT2:
            ret = sys_umount2((const char*)regs->ebx, regs->ecx);
            break;
        case SYS_LOCK:
            ret = sys_lock(regs->ebx, (struct flock*)regs->ecx);
            break;
        case SYS_IOCTL:
            // Variadic, we'll ignore for now
            ret = sys_ioctl(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_FCNTL:
            // Variadic
            ret = sys_fcntl(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_MPG:
            ret = sys_mpg();
            break;
        case SYS_SETSETID:
            ret = sys_setsetid();
            break;
        case SYS_XSWAP:
            ret = sys_xswap();
            break;
        case SYS_FHSTAT:
            ret = sys_fhstat((const char*)regs->ebx, (struct stat*)regs->ecx);
            break;
        case SYS_SYSLOG:
            ret = sys_syslog(regs->ebx, (char*)regs->ecx, regs->edx);
            break;
        case SYS_SETITIMER:
            ret = sys_setitimer(regs->ebx,
                                (const struct itimerval*)regs->ecx,
                                (struct itimerval*)regs->edx);
            break;
        case SYS_GETITIMER:
            ret = sys_getitimer(regs->ebx, (struct itimerval*)regs->ecx);
            break;
        case SYS_STATFS:
            ret = sys_statfs((const char*)regs->ebx, (struct statfs*)regs->ecx);
            break;
        case SYS_GETPRIORITY:
            ret = sys_getpriority(regs->ebx, regs->ecx);
            break;
        case SYS_SETPRIORITY:
            ret = sys_setpriority(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_TIMESPEC_GET:
            ret = sys_timespec_get((struct timespec*)regs->ebx, regs->ecx);
            break;
        case SYS_MSGGET:
            ret = sys_msgget(regs->ebx, regs->ecx);
            break;
        case SYS_MSGSND:
            ret = sys_msgsnd(regs->ebx, (const void*)regs->ecx, regs->edx, regs->esi);
            break;
        case SYS_MSGRCV:
            ret = sys_msgrcv(regs->ebx, (void*)regs->ecx, regs->edx, regs->esi, regs->edi);
            break;
        case SYS_MSGCTL:
            ret = sys_msgctl(regs->ebx, regs->ecx, (struct msqid_ds*)regs->edx);
            break;
        case SYS_SEMAPHORE:
            // We'll implement semget as SEMAPHORE for now
            ret = sys_semget(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_SEMOP:
            ret = sys_semop(regs->ebx, (struct sembuf*)regs->ecx, regs->edx);
            break;
        case SYS_SHMGET:
            ret = sys_shmget(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_SHMAT:
            ret = (long)sys_shmat(regs->ebx, (const void*)regs->ecx, regs->edx);
            break;
        case SYS_SHMDT:
            ret = sys_shmdt((const void*)regs->ebx);
            break;
        case SYS_SHMCTL:
            ret = sys_shmctl(regs->ebx, regs->ecx, (struct shmid_ds*)regs->edx);
            break;
        case SYS_SOCKET:
            ret = sys_socket(regs->ebx, regs->ecx, regs->edx);
            break;
        case SYS_BIND:
            ret = sys_bind(regs->ebx, (const struct sockaddr*)regs->ecx, regs->edx);
            break;
        case SYS_CONNECT:
            ret = sys_connect(regs->ebx, (const struct sockaddr*)regs->ecx, regs->edx);
            break;
        case SYS_LISTEN:
            ret = sys_listen(regs->ebx, regs->ecx);
            break;
        case SYS_ACCEPT:
            ret = sys_accept(regs->ebx, (struct sockaddr*)regs->ecx, (socklen_t*)regs->edx);
            break;
        case SYS_SEND:
            ret = sys_send(regs->ebx, (const void*)regs->ecx, regs->edx, regs->esi);
            break;
        case SYS_RECV:
            ret = sys_recv(regs->ebx, (void*)regs->ecx, regs->edx, regs->esi);
            break;
        case SYS_SENDTO:
            ret = sys_sendto(regs->ebx, (const void*)regs->ecx, regs->edx, regs->esi,
                             (const struct sockaddr*)regs->edi, /* addrlen from stack? we'll skip for now */ 0);
            break;
        case SYS_RECVFROM:
            ret = sys_recvfrom(regs->ebx, (void*)regs->ecx, regs->edx, regs->esi,
                               (struct sockaddr*)regs->edi, (socklen_t*)/* we'll skip */0);
            break;
        case SYS_SETSOCKOPT:
            ret = sys_setsockopt(regs->ebx, regs->ecx, regs->edx,
                                 (const void*)regs->esi, regs->edi);
            break;
        case SYS_GETSOCKOPT:
            ret = sys_getsockopt(regs->ebx, regs->ecx, regs->edx,
                                 (void*)regs->esi, (socklen_t*)regs->edi);
            break;
        case SYS_SOCKETPAIR:
            ret = sys_socketpair(regs->ebx, regs->ecx, regs->edx, (int*)regs->esi);
            break;
        case SYS_SYSINFO:
            ret = sys_sysinfo((struct sysinfo*)regs->ebx);
            break;
        default:
            ret = -38; // ENOSYS
            break;
    }

    regs->eax = ret;
}

// Initialize system calls
void syscall_init(void) {
    // We'll set up the interrupt vector for INT 0x80 later
    // For now, just a placeholder
}

