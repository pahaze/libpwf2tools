// Non-Qt headers
#include <pwf2tools/libisomod.h>

// Namespaces
namespace libisomod {
	// Initialization
	int statusCode = -1;
	std::string isomodError;
	std::stringstream isomodOutput;

	bool fileIsISO(byte *sector) {
		// If the first byte of the sector equals 1 and comparing sector (+1) and CD001 is true (AKA == 0), return true!
		if((sector[0] == 1) && (memcmp(sector + 1, "CD001", 5) == 0))
			return true;

		// Otherwise, return false!
		return false;
	}

	bool strcmplen(const char *string1, const char *string2, int length) {
		// A for loop is ran based on the length...
		for(int i = 0; i < length; i++) {
			// If the current 1st string in the loop does not equal the current 2nd string in the loop... return false!
			if(std::tolower(string1[i]) != std::tolower(string2[i]))
				return false;
		}

		// Otherwise, return true!
		return true;
	}

	bool fileExistsInDirOnISO(iso_dir *directory, FILE *ISO, const char *file, int filename_length, loc_t *sector_num, loc_t *offset_num) {
		// Create a byte the size of the CD sector size and create a directory with it
		byte sector[CD_SECTOR_SIZE];
		iso_dir *sectorDir = (iso_dir *)(sector);

		// How long the filename length should be ("sectorLength" as in the OG dir sector's filename length)
		int sectorLength = filename_length;
		// Directory offset
		int dirOffset = 0;

		// Read the ISO into the sector...
		readISOIntoMemory(sector, 1, ISO);

		// While the newly created sector directory length doesn't equal zero...
		while (sectorDir->length != 0) {
			// Get the file ID length (directory length here)
			int dirLength = sectorDir->fileid_length;
			// If the file flags and 2 (AKA file flags isn't 2) aren't true...
			if (!(sectorDir->file_flags & 2)) {
				// Remove 2 from the directory length
				dirLength -= 2;
			}

			// If the directory length equals the sector length...
			if (dirLength == sectorLength) {
				// And the file ID and filename match...
				if (strcmplen(sectorDir->fileid, file, sectorLength)) {
					// Set the output directory to the sector directory...
					*directory = *sectorDir;
					// If the output sector number isn't NULL, set that to the current sector - 1...
					if (sector_num != NULL)
						*sector_num = tellCurrentSectorOfISO(ISO) - 1;
					// If the output offset number isn't NULL, set that to the directory offset...
					if (offset_num != NULL)
						*offset_num = dirOffset;

					// And finally, return true!
					return true;
				}
			}

			// Otherwise, add the new sector dir's length to the offset...
			dirOffset += sectorDir->length;
			// And if it's longer than the CD sector size...
			if (dirOffset >= CD_SECTOR_SIZE) {
				// Read the next sector into memory...
				readISOIntoMemory(sector, 1, ISO);
				// And divide the offset by the CD sector size!
				dirOffset %= CD_SECTOR_SIZE;
			}

			// Set the new sector directory to the sector + offset!
			sectorDir = (iso_dir *)(sector + dirOffset);
		}

		// If true is never returned, return false!
		return false;
	}

	bool fileExistsOnISO(iso_dir *directory, FILE *ISO, const char *file, loc_t *sector_num, loc_t *offset_num) {
		// Create two chars for filename tests, one for the next to test and one for the current to test
		const char *nextFilename;
		const char *currentFilename = file;
		// Create an int for the sector "length" (our filename's length)
		int sectorLength;

		// While nextFilename isn't NULL...
		do {
			// Make the next filename the first occurence of "/" (from the directory's path)...
			nextFilename = strchr(currentFilename, '/');
			// If it returns NULL... set the sector "length" to the length of the string...
			if (nextFilename == NULL)
				sectorLength = strlen(currentFilename);
			// Otherwise... Set it to an int of nextFilename - currentFilename!
			else
				sectorLength = int(nextFilename - currentFilename);

			// If it does not exist in any re-ran loops, return false!
			if (!fileExistsInDirOnISO(directory, ISO, currentFilename, sectorLength, sector_num, offset_num)) {
				return false;
			}

			// If nextFilename is NOT NULL...
			if (nextFilename != NULL) {
				// Set the current location to the directory LBA...
				setCurrentISOLocation(ISO, directory->lba);
				// Add to nextFilename...
				nextFilename++;
				// And set currentFilename to nextFilename for the next run!
				currentFilename = nextFilename;
			// Otherwise... break!
			} else {
				break;
			}
		} while (nextFilename != NULL);

		// If false is never returned, return true!
		return true;
	}

	int getFile(std::string isoFile, std::string targetFile, std::string hostFile) {
		// Reset output variables so they can be reused!
		statusCode = -1;
		isomodError = "";
		isomodOutput.str("");
		isomodOutput.clear();

		// Try to open the ISO...
		isomodOutput << "Attempting to open ISO at " << isoFile << "..." << std::endl;
		FILE *ISO = fopen(isoFile.c_str(), "r+");
		if(ISO == NULL) {
			// And if it fails, tell the user...
			isomodOutput << "Couldn't open ISO file located at " << isoFile << "!" << std::endl;
			// Set the error...
			isomodError = "Unopenable ISO file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "Successfully opened ISO!" << std::endl;

		// Try to create/open the host file...
		isomodOutput << "Attempting to create/open host file located at " << hostFile << "..." << std::endl;
		FILE *HOST = fopen(hostFile.c_str(), "wb");
		if(HOST == NULL) {
			// And if it fails, tell the user...
			isomodOutput << "Couldn't create/open host file located at " << hostFile << "!" << std::endl;
			// Set the error...
			isomodError = "Unopenable/uncreatable host file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "Successfully created/opened host file!" << std::endl;

		// Create a new sector that will be used to read from the ISO and used as
		// an output directory for any functions ran to find files (in directories)!
		byte sector[CD_SECTOR_SIZE];
		iso_dir *directory = (iso_dir *)(sector);

		// Set the current ISO location to the padsize and read the first sector into memory!
		setCurrentISOLocation(ISO, ISO_PADSIZE);
		readISOIntoMemory(sector, 1, ISO);

		// Then that's used to check whether or not the file is actually an ISO or not...
		isomodOutput << "Checking whether the ISO you opened is a proper ISO file or not..." << std::endl;
		if(!fileIsISO(sector)) {
			// And if it fails, tell the user...
			isomodOutput << "The file you opened as your ISO is not an ISO file!" << std::endl;
			// Set the error...
			isomodError = "Opened ISO isn't an ISO9660 ISO file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "The ISO is good!" << std::endl;

		// Find the root LBA and tell what it is!
		loc_t root_lba = *(loc_t *)(sector + 158);
		// This has to be formatted and then reverted to the regular flags!
		std::ios::fmtflags originalFormat(isomodOutput.flags());
		isomodOutput << "The ISO's root LBA is " << std::hex << root_lba << std::endl;
		isomodOutput.flags(originalFormat);

		// Set the current ISO location to the root LBA and create a directory LBA and offset loc_t
		setCurrentISOLocation(ISO, root_lba);
		loc_t directory_lba, directory_offset;

		// Attempt to find the file you're looking for on the ISO...
		isomodOutput << "Attempting to find the file " << targetFile << " on the ISO..." << std::endl;
		if(!fileExistsOnISO(directory, ISO, targetFile.c_str(), &directory_lba, &directory_offset)) {
			// And if it fails, tell the user...
			isomodOutput << "Could not find the target file \"" << targetFile.c_str() << "\" on the ISO located at " << isoFile << std::endl;
			// Set the error...
			isomodError = "Unfindable target file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "File " << targetFile << " was successfully found on the ISO!" << std::endl;

		// Set the current ISO location to the directory LBA and read the next sector into memory...
		setCurrentISOLocation(ISO, directory_lba);
		readISOIntoMemory(sector, 1, ISO);
		// Then, set the directory to the sector + the directory offset!
		directory = (iso_dir *)(sector + directory_offset);
		// Get the file length of the file on the ISO as an int and align the length in a separate int!
		int fileLength = directory->file_length;
		int aligned_length = ALIGN(fileLength, 0x800);
		isomodOutput << "File size of the file " << targetFile << " on the ISO is " << fileLength << std::endl;

		// Set the ISO location to the directory's LBA...
		setCurrentISOLocation(ISO, directory->lba);
		// Create a byte pointer that allocates enough memory for the aligned length to read the host file into...
		byte *rawFileData = (byte *)(malloc(aligned_length));
		memset(rawFileData, 0, aligned_length);
		// Read the file from the ISO into the raw data byte pointer...
		fread(rawFileData, 1, fileLength, ISO);

		// Inform that we're writing bytes to the file!
		isomodOutput << "Writing " << fileLength << " bytes to the file " << hostFile <<  "..." << std::endl;
		// Actually write the data to the host file!
		fwrite(rawFileData, 1, fileLength, HOST);

		// Close our host and ISO files as we no longer need them!
		fclose(HOST);
		fclose(ISO);

		// Done!
		isomodOutput << "Finished!";
		// Set the status code to 0 to let it be known that isomod ran successfully
		statusCode = 0;
		return 0;
	}

	int getSizeOfFile(FILE *file) {
		// Get the starting position of the file before we start at 0...
		int startPosition = ftell(file);
		// Seek from 0 to the end...
		fseek(file, 0, SEEK_END);
		// The file length is now what ftell returns...
		int fileLength = ftell(file);
		// Seek back to startPosition and set it...
		fseek(file, startPosition, SEEK_SET);
		// And return fileLength!
		return fileLength;
	}

	int putFile(std::string isoFile, std::string targetFile, std::string hostFile) {
		// Reset output variables so they can be reused!
		statusCode = -1;
		isomodError = "";
		isomodOutput.str("");
		isomodOutput.clear();

		// Try to open the ISO...
		isomodOutput << "Attempting to open ISO at " << isoFile << "..." << std::endl;
		FILE *ISO = fopen(isoFile.c_str(), "r+");
		if(ISO == NULL) {
			// And if it fails, tell the user...
			isomodOutput << "Couldn't open ISO file located at " << isoFile << "!" << std::endl;
			// Set the error...
			isomodError = "Unopenable ISO file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "Successfully opened ISO!" << std::endl;

		// And the host file...
		isomodOutput << "Attempting to open host file located at " << hostFile << "..." << std::endl;
		FILE *HOST = fopen(hostFile.c_str(), "rb");
		if(HOST == NULL) {
			// And if it fails, tell the user...
			isomodOutput << "Couldn't open host file located at " << hostFile << "!" << std::endl;
			// Set the error...
			isomodError = "Unopenable host file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "Successfully opened host file!" << std::endl;

		// Create a new sector that will be written to the ISO and used as an
		// output directory for any functions ran to find files (in directories)!
		byte sector[CD_SECTOR_SIZE];
		iso_dir *directory = (iso_dir *)(sector);

		// Set the current ISO location to the padsize and read the first sector into memory!
		setCurrentISOLocation(ISO, ISO_PADSIZE);
		readISOIntoMemory(sector, 1, ISO);

		// Then that's used to check whether or not the file is actually an ISO or not...
		isomodOutput << "Checking whether the ISO you opened is a proper ISO file or not..." << std::endl;
		if(!fileIsISO(sector)) {
			// And if it fails, tell the user...
			isomodOutput << "The file you opened as your ISO is not an ISO file!" << std::endl;
			// Set the error...
			isomodError = "Opened ISO isn't an ISO9660 ISO file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "The ISO is good!" << std::endl;

		// Find the root LBA and tell what it is!
		loc_t root_lba = *(loc_t *)(sector + 158);
		// This has to be formatted and then reverted to the regular flags!
		std::ios::fmtflags originalFormat(isomodOutput.flags());
		isomodOutput << "The ISO's root LBA is " << std::hex << root_lba << std::endl;
		isomodOutput.flags(originalFormat);

		// Set the current ISO location to the root LBA and create a directory LBA and offset loc_t
		setCurrentISOLocation(ISO, root_lba);
		loc_t directory_lba, directory_offset;

		// Attempt to find the file you're looking for on the ISO...
		isomodOutput << "Attempting to find the file " << targetFile << " on the ISO..." << std::endl;
		if(!fileExistsOnISO(directory, ISO, targetFile.c_str(), &directory_lba, &directory_offset)) {
			// And if it fails, tell the user...
			isomodOutput << "Could not find the target file \"" << targetFile.c_str() << "\" on the ISO located at " << isoFile << std::endl;
			// Set the error...
			isomodError = "Unfindable target file!";
			// And set the status code to 1 to let the user know isomod failed to run!
			statusCode = 1;
			return -1;
		}
		isomodOutput << "File " << targetFile << " was successfully found on the ISO!" << std::endl;

		// Set the current ISO location to the directory LBA and read the next sector into memory...
		setCurrentISOLocation(ISO, directory_lba);
		readISOIntoMemory(sector, 1, ISO);
		// Then, set the directory to the sector + the directory offset!
		directory = (iso_dir *)(sector + directory_offset);
		// Give the file length of the currently existing file on the ISO!
		isomodOutput << "File size of the file " << targetFile << " on the ISO is " << directory->file_length << std::endl;

		// Get the file length of the host file as an int and align the length in a separate int!
		int fileLength = getSizeOfFile(HOST);
		int aligned_length = ALIGN(fileLength, 0x800);
		// Give the file length of the host file!
		isomodOutput << "File size of the file " << hostFile << " is " << fileLength << std::endl;

		// Create a byte pointer that allocates enough memory for the aligned length to read the host file into...
		byte *rawFileData = (byte *)(malloc(aligned_length));
		memset(rawFileData, 0, aligned_length);
		// Read the host file into the raw data byte pointer...
		fread(rawFileData, 1, fileLength, HOST);
		// Then close our host file as we no longer need it!
		fclose(HOST);
		// Inform that we're writing bytes to the sector!
		isomodOutput << "Writing " << fileLength << " bytes to sector " << directory->lba <<  "..." << std::endl;

		// Set the ISO location to the directory's LBA...
		setCurrentISOLocation(ISO, directory->lba);
		// Then write the raw file data to the file on the ISO!
		writeContentToISO(rawFileData, (aligned_length / CD_SECTOR_SIZE), ISO);
		// Set the new file length to the host file's file length!
		directory->file_length = fileLength;
		// Write that we're writing a new directory sector!
		isomodOutput << "Writing new directory to sector " << directory_lba << "..." << std::endl;

		// Set the ISO location to the output directory LBA!
		setCurrentISOLocation(ISO, directory_lba);
		// Write the new sector to the ISO!
		writeContentToISO(sector, 1, ISO);
		// Close our ISO file as we're done with it!
		fclose(ISO);

		// Done!
		isomodOutput << "Finished!";
		// Set the status code to 0 to let it be known that isomod ran successfully
		statusCode = 0;
		return 0;
	}

	loc_t tellCurrentSectorOfISO(FILE *ISO) {
		// Get the current sector of the ISO as loc_t by using ftell...
		loc_t sector = ftell(ISO);
		// And return it and -0x7FF / 0x800 (sector and -2048 / 2048)!
		return((sector & (~(0x7FF))) / 0x800);
	}

	void readISOIntoMemory(void *buffer, loc_t sector, FILE *ISO) {
		// Use fread to read into a buffer with the ISO at the gave sector!
		fread(buffer, sector, CD_SECTOR_SIZE, ISO);
	}

	void setCurrentISOLocation(FILE *ISO, loc_t position) {
		// Use fseeko64 to set the ISO position by multiplying the position by the CD sector size!
		fseeko64(ISO, position * CD_SECTOR_SIZE, SEEK_SET);
	}

	void writeContentToISO(void *buffer, loc_t sector, FILE *ISO) {
		// Use fwrite to write a buffer to the ISO at the gave sector!
		fwrite(buffer, sector, CD_SECTOR_SIZE, ISO);
	}
}
