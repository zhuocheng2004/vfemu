
#include <iostream>
#include <nes/cartridge.h>
#include <nes/nespin.h>

namespace vfemu {

namespace nes {


CartMapper0Module::CartMapper0Module(const INESStruct& info) : Module({
	std::make_pair("cart", new Port("nes:nesconn", cart_receive)),
}), info(info) {
}


Status CartMapper0Module::init() {
	if (info.flag & INES_MPL != 0)	// only mapper0
		return Status::ERR_INVALID;

	return Status::SUCCESS;
}

Status CartMapper0Module::exit() {
	return Status::SUCCESS;
}


Status CartMapper0Module::cart_receive(Module* receiver, u64 data) {
	auto module = (CartMapper0Module*) receiver;
	auto dat = (NesConnector::Data*) module->ports[IDX_CART].second->data;
	if (!dat)
		return Status::ERR_NULL;
	auto& info = module->info;

	// CPU read/write
	u16 addr = dat->addr;
	if (data & NesConnector::MSK_RW) {
		// read
		if (addr & 0x8000) {	// RPG ROM Range
			if (!(addr & 0x4000)) {
				// first 16k
				dat->data = info.rpg[addr & 0x3fff];
			} else {
				// second 16k
				if (info.rpgSize <= 1) {
					// mirror the first 16k
					dat->data = info.rpg[addr & 0x3fff];
				} else {
					dat->data = info.rpg[addr & 0x7fff];
				}
			}
		}
	} else {
		// write
		// Nothing happens.
	}

	return Status::SUCCESS;
}


} // namespace nes

} // namespace vfemu
