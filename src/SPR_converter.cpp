/*
 * SPR_converter.cpp
 *
 *  Created on: Apr 24, 2014
 *      Author: zagevalo
 */

#include <iostream>
#include <fstream>
#include <string>

#include "data_structs.h"
#include "decoder.h"

using namespace std;

const string SPR_FILE_SIGNATURE("SEGA SPRED 02.0M");

int main(int argc, char **argv) {

	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " <path_to_SPR_file>" << endl;
	    return 1;
	}

	string input_file_path = argv[1];

	fstream spr_file;
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
	spr_subpic_info pic_info = {0, 0, 0, 0, 0, 0};

	spr_file.read((char *)&db_hdr, sizeof(db_hdr));

	db_hdr.pic_count = __builtin_bswap16(db_hdr.pic_count);

	cout << "Num of pic files: " << db_hdr.pic_count << endl;

	//spr_file.seekg(sizeof(dat_block.db_header), fstream::cur);

	for (int i=1; i<=db_hdr.pic_count; i++) {
		spr_file.read((char *)&pic_info, sizeof(pic_info));

		pic_info.width = __builtin_bswap16(pic_info.width);
		pic_info.height = __builtin_bswap16(pic_info.height);
		pic_info.offset = __builtin_bswap32(pic_info.offset);
		pic_info.size = __builtin_bswap32(pic_info.size);

		cout << endl;
		cout << "Pic " << i << ": " << endl;
		cout << "Width: " << hex << pic_info.width << endl;
		cout << "Height: " << hex << pic_info.height << endl;
		cout << "Offset: " << hex << pic_info.offset << endl;
		cout << "Size: " << hex << pic_info.size << endl;
		cout << endl;

		int cur_offset = spr_file.tellg();

		int data_offset = spr_idx.image_block_idx.offset +
				sizeof(db_hdr) +
				(db_hdr.pic_count * sizeof(db_hdr)) + pic_info.offset;

		cout << "sizeof(db_hdr): " << hex << sizeof(db_hdr) << endl;
		cout << "data_offset: " << hex << data_offset << endl;

		spr_file.seekg(data_offset, fstream::beg);

		//int final_size = (pic_info.width * pic_info.height * 16) / 8;
		int final_size = ((pic_info.width * 16 + 31) / 32 ) * 4 * pic_info.height;
		char *DestBuf = new char[final_size];
		char *SrcBuf = new char[pic_info.size];

		spr_file.read(SrcBuf, pic_info.size);

		//cout << "sizeof(SrcBuf): " << sizeof(&SrcBuf) << endl;
		cout << "final_size: " << hex << final_size << endl;

		spr_decompress(SrcBuf, pic_info.size, DestBuf, final_size);

		fstream out_file;
		string out_file_name = "pic_" + to_string(i);
		out_file.open(out_file_name.c_str(), ios_base::out | ios_base::binary);
		out_file.imbue(locale::classic());

		// Write bmp header
		bmp_header out_file_header;
		out_file_header.size = 0x36 + final_size;
		out_file_header.dib_hdr.img_height = pic_info.height;
		out_file_header.dib_hdr.img_width = pic_info.width;
		out_file_header.dib_hdr.size_of_raw_bitmap_data = final_size;
		out_file.put('B');
		out_file.put('M');
		out_file.write((char *)&out_file_header, sizeof(out_file_header));

		// Write img data
		out_file.write(DestBuf, final_size);

		out_file.close();

		delete SrcBuf;
		delete DestBuf;

		spr_file.seekg(cur_offset, fstream::beg);
	}

	spr_file.close();

	return 0;
}



