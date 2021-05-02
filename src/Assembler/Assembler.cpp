#include "./Assembler.hpp"

#include "../FileInput.hpp"

#include <iomanip>
#include <iostream>

int main( int argc, char* argv[] )
{
	// Get the file names and options
	std::vector<const char*> fileNames;
	std::vector<const char*> optionNames;
	for ( uint32_t i = 1; i < argc; i++ )
	{
		if ( argv[i][0] == '-' ) // Is an option
			optionNames.push_back( argv[i] );
		else
			fileNames.push_back( argv[i] );
	}

	uint16_t flags = 0;

	// Check options
	for ( const auto& option : optionNames )
	{
		if ( option[1] == 's' ) flags |= ( 1 << 0 ); // Set first bit to 1
	}

	if ( !( flags & ( 1 << 0 ) ) )
	{
		// Get the max length of the argv
		uint32_t			  maxLength = 0;
		std::vector<uint32_t> lengths;

		for ( uint32_t i = 0; i < fileNames.size(); i++ )
		{
			uint32_t len = 0;

			// Get length of argument and set maxLength
			while ( fileNames[i][len] != '\0' ) len++;
			if ( len > maxLength ) maxLength = len;

			// Add length to vector
			lengths.push_back( len );
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
	}

	return 0;
}