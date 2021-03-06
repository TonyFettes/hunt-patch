#include <errno.h>
#include <signal.h>
int __set_errno(int mask)
{
	errno = mask;
	return -1;
}

# define __sigmask(sig) (((__sigset_t) 1) << ((sig) - 1))

#define __sigemptyset(set)			\
  (__extension__ ({				\
    *(set) = (__sigset_t) 0;			\
    0;					\
  }))
#define __sigfillset(set)			\
  (__extension__ ({				\
    *(set) = ~(__sigset_t) 0;			\
    0;					\
  }))

# define __sigisemptyset(set)			\
  (*(set) == (__sigset_t) 0)

# define __sigandset(dest, left, right)		\
  (__extension__ ({				\
    *(dest) = *(left) & *(right);		\
    0;					\
  }))

# define __sigorset(dest, left, right)		\
  (__extension__ ({				\
    *(dest) = *(left) | *(right);		\
    0;					\
  }))

# define __sigismember(set, sig)		\
  (__extension__ ({				\
    __sigset_t __mask = __sigmask (sig);	\
    *(set) & __mask ? 1 : 0;			\
  }))

# define __sigaddset(set, sig)			\
  (__extension__ ({				\
    __sigset_t __mask = __sigmask (sig);	\
    *(set) |= __mask;				\
    0;					\
  }))

# define __sigdelset(set, sig)			\
  (__extension__ ({				\
    __sigset_t __mask = __sigmask (sig);	\
    *(set) &= ~__mask;				\
    0;					\
	}))

static inline int __attribute__ ((unused))
sigset_set_old_mask (sigset_t *set, int mask)
{
  if (sizeof (__sigset_t) == sizeof (unsigned int))
    *set = (unsigned int) mask;
  else
    {
      unsigned int __sig;

      if (__sigemptyset (set) < 0)
	return -1;

      for (__sig = 1; __sig < NSIG && __sig <= sizeof (mask) * 8; __sig++)
	if (mask & sigmask (__sig))
	  if (__sigaddset (set, __sig) < 0)
	    return -1;
    }
  return 0;
}
static inline int __attribute__ ((unused))
sigset_get_old_mask (const sigset_t *set)
{
  if (sizeof (sigset_t) == sizeof (unsigned int))
    return (unsigned int) *set;
  else
    {
      unsigned int mask = 0;
      unsigned int sig;

      for (sig = 1; sig < NSIG && sig <= sizeof (mask) * 8; sig++)
	if (__sigismember (set, sig))
	  mask |= sigmask (sig);

      return mask;
    }
}
int sigblock(int mask)
{
	sigset_t set, oset;
	if (sigset_set_old_mask (&set, mask) < 0) return -1;
	if (sigprocmask (SIG_BLOCK, &set, &oset) < 0) return -1;
	return sigset_get_old_mask(&oset);
}
int sigsetmask(int mask)
{
	sigset_t set, oset;
	if (sigset_set_old_mask (&set, mask) < 0) return -1;
	if (sigprocmask (SIG_SETMASK, &set, &oset) < 0) return -1;
	return sigset_get_old_mask (&oset);
}
