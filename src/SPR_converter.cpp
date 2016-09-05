/*
 * SPR_converter.cpp
 *
 *  Created on: Apr 24, 2014
 *      Author: zagevalo
 */

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

const string SPR_FILE_SIGNATURE("SEGA SPRED 02.0M");

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <path_to_SPR_file>" << endl;
	    return 1;
	}

	string input_file_path = argv[1];

	struct spr_block_index {
		unsigned int offset; // File offset
		unsigned int size; // File size
		unsigned int type; // Type of file
		unsigned int reserve; // Always zero
	};

	struct spr_index {
		char signature[16];
		spr_block_index first_block_idx;
		spr_block_index image_block_idx;
		spr_block_index third_block_idx;
		spr_block_index fourth_block_idx;
		spr_block_index fifth_block_idx;
	};

	struct data_block_header {
		unsigned short pic_count; // Total number of pics
		unsigned short var2;
		unsigned short var3;
		unsigned short var4;
		unsigned int var5;
		unsigned int var6;
	};

	struct spr_subpic_info {
		unsigned short width;
		unsigned short height;
		unsigned short var3;
		unsigned short var4;
		unsigned int offset; // Offset of the image, from the end of the index
		unsigned int size;
	};

	struct spr_data_block {
			data_block_header db_header;
			char* data;
		};

	//streampos spr_file_size;
	//string signature("");

	//char* mem_buffer;
	//char* spr_index_entry;
	//char* spr_pic_info_entry;
	//char* spr_subpic_info_entry;

	fstream spr_file;
	//spr_file.open(input_file_path.c_str(), std::fstream::in | std::fstream::binary | std::fstream::ate);
	spr_file.open(input_file_path.c_str(), fstream::in | fstream::binary);

	spr_block_index idx = {0, 0, 0, 0};
	spr_index spr_idx = {"", idx, idx, idx, idx};

	if (spr_file.is_open()) {

		//// Get spr file size
		//spr_file_size = spr_file.tellg();

		////mem_buffer = new char[spr_file_size];
		spr_file.seekg(0, fstream::beg);

		spr_file.read(spr_idx.signature, sizeof(spr_idx.signature));
		spr_file.read((char *)&spr_idx.first_block_idx, sizeof(spr_idx.first_block_idx));
		spr_file.read((char *)&spr_idx.image_block_idx, sizeof(spr_idx.image_block_idx));
		spr_file.read((char *)&spr_idx.third_block_idx, sizeof(spr_idx.third_block_idx));
		spr_file.read((char *)&spr_idx.fourth_block_idx, sizeof(spr_idx.fourth_block_idx));
		spr_file.read((char *)&spr_idx.fifth_block_idx, sizeof(spr_idx.fifth_block_idx));
		//spr_file.get(signature, 17);

		//delete[] mem_buffer;
	}
	else {
		cout << "ERROR: Can't open file!!!" << endl;
	}

	//spr_file.close();

	cout << "Checking input file signature..." << endl;
	cout << "Read signature: " << spr_idx.signature << endl;

	if (spr_idx.signature != SPR_FILE_SIGNATURE) {
		cerr << "ERROR: File signatures mismatch!!!" << endl;
		cerr << "       Input file signature (16 bytes): " << spr_idx.signature << endl;
		cerr << "       SPR file signature must be:      " << SPR_FILE_SIGNATURE << endl;

		return 1;
	}
	else {
		cout << "Signature: OK" << endl;
	}

	cout << "Images header..." << endl;

	spr_idx.image_block_idx.offset = __builtin_bswap32(spr_idx.image_block_idx.offset);
	spr_idx.image_block_idx.size = __builtin_bswap32(spr_idx.image_block_idx.size);
	spr_idx.image_block_idx.type = __builtin_bswap32(spr_idx.image_block_idx.type);
	spr_idx.image_block_idx.reserve = __builtin_bswap32(spr_idx.image_block_idx.reserve);

	cout << "Offset: " << hex << spr_idx.image_block_idx.offset << endl;
	cout << "Size: " << hex << spr_idx.image_block_idx.size << endl;
	cout << "Type: " << hex << spr_idx.image_block_idx.type << endl;
	cout << "Reserve: " << hex << spr_idx.image_block_idx.reserve << endl;


	cout << "Reading images data..." << endl;

	spr_file.seekg(spr_idx.image_block_idx.offset, fstream::beg);

	data_block_header db_hdr = {0, 0, 0, 0, 0, 0};
	spr_data_block dat_block = {db_hdr, nullptr};
	spr_subpic_info pic_info = {0, 0, 0, 0, 0, 0};

	spr_file.read((char *)&dat_block.db_header, sizeof(dat_block.db_header));

	dat_block.db_header.pic_count = __builtin_bswap16(dat_block.db_header.pic_count);

	cout << "Num of pic files: " << dat_block.db_header.pic_count << endl;

	//spr_file.seekg(sizeof(dat_block.db_header), fstream::cur);

	for (int i=1; i<=dat_block.db_header.pic_count; i++) {
		spr_file.read((char *)&pic_info, sizeof(pic_info));

		int cur_offset = spr_file.tellg();

		int data_offset = spr_idx.image_block_idx.offset +
					sizeof(dat_block.db_header) +
					__builtin_bswap32(pic_info.offset);

		cout << endl;
		cout << "Pic " << i << ": " << endl;
		cout << "Width: " << __builtin_bswap16(pic_info.width); cout << endl;
		cout << "Height: " << __builtin_bswap16(pic_info.height); cout << endl;
		cout << "Offset: " << __builtin_bswap32(pic_info.offset); cout << endl;
		cout << "Size: " << __builtin_bswap32(pic_info.size); cout << endl;
		cout << endl;

		spr_file.seekg(data_offset, fstream::beg);

		spr_file.seekg(cur_offset, fstream::beg);
	}

	spr_file.close();

	return 0;
}



