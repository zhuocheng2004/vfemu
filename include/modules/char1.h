
#ifndef VFEMU_MODULES_CHAR1_H
#define VFEMU_MODULES_CHAR1_H

#include <chrono>
#include <thread>
#include <vfemu/Module.h>


typedef struct {
	u8				ch;
	std::chrono::milliseconds	interval;
} char1_gen_config;

extern vfemu::VFEMUModule char1_out_module;

extern vfemu::VFEMUModule char1_gen_module;


#endif /* VFEMU_MODULES_CHAR1_H */
