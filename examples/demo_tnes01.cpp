
#include <chrono>
#include <iostream>
#include <thread>
#include <vfemu/Connector.h>
#include <modules/pin.h>
#include <modules/gen.h>
#include <nes/nes.h>
#include <nes/tnes01.h>
#include <nes/cartridge.h>

using namespace std::chrono_literals;
using namespace vfemu;

int main(int argc, char* argv[]) {
	if (argc < 2) {
		std::cerr << "Usage: " << argv[0] << " NESFilePath" << std::endl;
		return 1;
	}

	nes::INESStruct iNESInfo(argv[1]);

	if (!iNESInfo.success) {
		std::cerr << "Error reading iNES file: " 
			  << iNESInfo.errorMsg 
			  << std::endl;
		return 1;
	}

	/*
	 * show iNES info
	 */
	std::cout << "  ==== iNES file info ====  " << std::endl;
	std::cout << "File Name: " << argv[1] << std::endl;
	std::cout << "RPG ROM Size (in 16KB units): " << (int) iNESInfo.rpgSize << std::endl;
	std::cout << "CHR ROM Size (in  8KB units): " << (int) iNESInfo.chrSize << std::endl;
	printf("Flag: %08X\n", iNESInfo.flag);
	std::cout << "  Orientation: " 
		  << ((iNESInfo.flag & nes::INES_HV) ? "Vertical (horizontal arrangement)" : "Horizontal (vertical arrangement)") 
		  << std::endl;
	std::cout << "  Has battery-backed RPG ROM: " 
		  << ((iNESInfo.flag & nes::INES_BB) ? "Yes" : "No") 
		  << std::endl;
	std::cout << "  Mapper Number: " << ((iNESInfo.flag & nes::INES_MPL) >> 4) << std::endl;
	std::cout << std::endl;

	/*
	 * "pin2pin" connector type 
	 */
	auto pin2pin = pin::Pin2pin();
	/*
	 * "nes::nesconn" connector type
	 */
	auto nesConn = nes::NesConn();

	auto reset_ctrl = new gen::Gen1Module();	// signal to reset and start machine
	auto board = new nes::TNES01Module();		// main board
	auto cartridge = new nes::CartMapper0Module(iNESInfo);	// cartridge

	initModules({
		reset_ctrl, board, cartridge,
	});

	/**
	 * Here we define how ports are connected.
	 * 
	 * The entry { &pin2pin, reset_ctrl, "out", board, "rst" },
	 * means connecting port "out" of reset_ctrl with port "rst" of board, 
	 * using connector type "pin2pin".
	 */
	connectPorts({
		{ &pin2pin, reset_ctrl, "out", board, "rst" },
		{ &nesConn, cartridge, "cart", board, "cart" },
	});

	// reset board
	reset_ctrl->send(1);

	do {
		std::this_thread::sleep_for(1000ms);
	} while (1);

	return 0;
}