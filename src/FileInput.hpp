#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class FileIO
{
public:
	static std::vector<std::vector<char>> Read( const char* path )
	{
		std::ifstream fs( path );

		std::vector<std::vector<char>> output;

		try
		{
			if ( fs.is_open() )
			{
				std::string line;

				for ( uint32_t i = 0; !fs.eof(); i++ )
				{
					// Get the line from the file
					getline( fs, line );

					// Create a new vector
					output.push_back( {} );

					// Add all the chars to it
					for ( const auto& chr : line )
						output[output.size() - 1].push_back( chr );
				}

				// Close the file stream
				fs.close();
			}
			else
				std::runtime_error( "Failed to open file" );
		}
		catch ( const std::exception& e )
		{
			std::cerr << "File Read Error: " << e.what() << std::endl;
		}

		return output;
	}
};