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

struct dib_header {
		unsigned int header_size = 0x28;
		unsigned int img_width;
		unsigned int img_height;
		unsigned short color_panes = 0x01;
		unsigned short bpp = 0x10;
		unsigned int compression = 0x0;
		unsigned int size_of_raw_bitmap_data;
		unsigned int resolution1 = 0x0B13;
		unsigned int resolution2 = 0x0B13;
		unsigned int num_of_colors_in_palette = 0x0;
		unsigned int important_colors = 0x0;
};

struct bmp_header {
		//unsigned short sig = 0x4D42;
		unsigned int size;
		unsigned short res1 = 0x0;
		unsigned short res2 = 0x0;
		unsigned int img_data_offset = 0x36;
		dib_header dib_hdr;
};
