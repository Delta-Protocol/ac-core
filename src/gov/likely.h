#ifndef USGOV_a5f6edaffbe1731ffbd5f85413e141177eae17e67770c54f0bff8093b4b12d9f
#define USGOV_a5f6edaffbe1731ffbd5f85413e141177eae17e67770c54f0bff8093b4b12d9f

#ifdef __GNUC__
# if (__GNUC__ >= 3)
#  define likely(x)     __builtin_expect ((x), 1)
#  define unlikely(x)   __builtin_expect ((x), 0)
# else
#  define likely(x)     (x)
#  define unlikely(x)   (x)
# endif
#else
# define likely(x)      (x)
# define unlikely(x)    (x)
#endif

#endif

