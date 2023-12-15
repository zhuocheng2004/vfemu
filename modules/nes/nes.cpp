
#include <fstream>
#include <nes/nes.h>


namespace vfemu {

namespace nes {


INESStruct::INESStruct(const std::string& filename) {
	std::ifstream fs(filename, std::ios::in | std::ios::binary);

	if(!fs) {
		errorMsg = "unable to open file";
		return;
	}
	
	u8 buffer[16];
	fs.read((char*) buffer, 16);
	if (!(buffer[0] == INES_MAG0 && buffer[1] == INES_MAG1 &&
		buffer[2] == INES_MAG2 && buffer[3] == INES_MAG3)) {
		fs.close();
		errorMsg = "MAGIC is not \"NES\\x1A\"";
		return;
	}

	rpgSize = buffer[4];
	chrSize = buffer[5];
	flag = buffer[6];

	if (flag & INES_TN) {	// has trainer
		trainer = new u8[0x200];	// 512
		fs.read((char*) trainer, 0x200);
	}

	rpg = new u8[0x4000 * rpgSize];		// 16k
	if (rpgSize > 0 && !rpg) {
		fs.close();
		return;
	}
	fs.read((char*) rpg, 0x4000 * rpgSize);

	chr = new u8[0x2000 * rpgSize];		// 8k
	if (chrSize > 0 && !chr) {
		fs.close();
		return;
	}
	fs.read((char*) chr, 0x2000 * rpgSize);
	
	fs.close();
	success = true;
}

INESStruct::~INESStruct() {
	if (rpg)
		delete rpg;
	if (chr)
		delete chr;
	
	if (trainer)
		delete trainer;
}


} // namespace nes

} // namespace vfemu
