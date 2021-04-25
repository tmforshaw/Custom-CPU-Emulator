#include "./Assembler.hpp"
#include "./FileInput.hpp"

#include <iomanip>
#include <iostream>

// Bug with sta

int main( int argc, char* argv[] )
{
	// Get the max length of the argv
	uint32_t maxLength = 0;
	uint32_t lengths[argc - 1];

	for ( uint32_t i = 1; i < argc; i++ )
	{
		uint32_t len = 0;

		// Get length of argument
		while ( argv[i][len] != '\0' )
			len++;

		lengths[i - 1] = len;

		if ( len > maxLength )
			maxLength = len;
	}

	uint16_t byteGrouping = 4;

	for ( uint32_t i = 1; i < argc; i++ )
	{
		std::vector<uint8_t> assembledCode = Assembler::Assemble( FileIO::Read( argv[i] ) );

		// Print the file name and spacing
		std::cout << argv[i] << ": ";
		for ( uint32_t j = 0; j < maxLength - lengths[i - 1]; j++ ) std::cout << ' '; // Print the spacing for the code

		// Print the assembled code
		for ( uint32_t j = 0; j < assembledCode.size(); j++ )
			// Print as hex, then group into byte chunks
			std::cout << std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 2 ) << (uint16_t)assembledCode[j] << ( j % ( byteGrouping ) == ( byteGrouping - 1 ) ? " " : "" );

		std::cout << std::endl;
	}

	return 0;
}