
#ifndef VFEMU_MACROS_H
#define VFEMU_MACROS_H


#ifdef __cplusplus
# define EXTERN_C_BEGIN         extern "C" {
# define EXTERN_C_END           }
#else
# define EXTERN_C_BEGIN
# define EXTERN_C_END
#endif


#define AS_VFEMU_VERSION(major, minor, patch)	\
	(((major) << 16) + ((minor) << 8) + (patch))


#endif /* VFEMU_MACROS_H */
