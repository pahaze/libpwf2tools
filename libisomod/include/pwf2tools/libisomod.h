#pragma once
#ifndef LIBISOMOD_H
#define LIBISOMOD_H
// Non-Qt headers
#include <cstring>
#include <iostream>
#include <sstream>
#include <stdint.h>

// Namespaces
namespace libisomod {
	// Defines
	#define ALIGN(x, y) (((x) + ((y)-1)) & (~((y)-1)))
	#define CD_SECTOR_SIZE 0x800
	#define ISO_PADSIZE 0x10

	// Typedefs
	typedef uint8_t byte;
	typedef uint32_t loc_t;

	// Structs
	struct iso_dir {
		uint8_t length;
		uint8_t ex_length;
		uint32_t lba;
		uint32_t big_lba;
		uint32_t file_length;
		uint32_t big_file_length;
		byte time_date[7];
		byte file_flags;
		byte file_unit_size;
		byte interleave_gap_size;
		uint16_t volseq_number;
		uint16_t big_volseq_number;
		uint8_t fileid_length;
		char fileid[0];
	} __attribute((packed));

	// Others
	bool fileExistsInDirOnISO(iso_dir *directory, FILE *ISO, const char *file, int filename_length, loc_t *sector_num, loc_t *offset_num);
	bool fileExistsOnISO(iso_dir *directory, FILE *ISO, const char *file, loc_t *sector_num, loc_t *offset_num);
	bool fileIsISO(byte *sector);
	bool strcmplen(const char *string1, const char *string2, int length);
	extern int statusCode;
	extern std::string isomodError;
	extern std::stringstream isomodOutput;
	int getFile(std::string isoFile, std::string targetFile, std::string hostFile);
	int getSizeOfFile(FILE *file);
	int putFile(std::string isoFile, std::string targetFile, std::string hostFile);
	loc_t tellCurrentSectorOfISO(FILE *ISO);
	void readISOIntoMemory(void *buffer, loc_t sector, FILE *ISO);
	void setCurrentISOLocation(FILE *ISO, loc_t position);
	void writeContentToISO(void *buffer, loc_t sector, FILE *ISO);
};

#endif // LIBISOMOD_H
