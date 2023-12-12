
#ifndef VFEMU_TYPES_H
#define VFEMU_TYPES_H

#include <string>
#include <vfemu/macros.h>


typedef unsigned char		u8;
typedef unsigned short		u16;
typedef unsigned int		u32;
typedef unsigned long long	u64;


namespace vfemu {

typedef enum {
	/**
	 * Operation is successful.
	 */
	SUCCESS = 0,

	/**
	 * warning, but not fatal
	 */
	WARNING,

	/**
	 * generic error
	 */
	ERROR,

	/**
	 * invalid argument
	 */
	ERR_INVALID,

	/**
	 * NULL pointer error
	 */
	ERR_NULL,

	/**
	 * Target already exists.
	 */
	ERR_EXIST,

	/**
	 * Target doesn't exist.
	 */
	ERR_NONEXIST,
} Status;

}

#endif /* VFEMU_TYPES_H */
