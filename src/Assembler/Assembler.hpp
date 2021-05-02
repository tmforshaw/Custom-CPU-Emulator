#pragma once
#include "../IntDef.hpp"
#include "./Token.hpp"

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

class Assembler
{
public:
	static std::vector<uint8_t> Assemble( const std::vector<std::vector<char>>& p_buffer )
	{
		// Create the vectors
		std::vector<std::string> words {};
		std::vector<Token>		 tokens {};
		std::vector<uint8_t>	 bytes {};

		// Keep track of if we are on a word or not
		bool onWord = false;

		// Catch errors
		try
		{
			// Iterate the lines
			for ( uint32_t i = 0; i < p_buffer.size(); i++ )
			{
				// Keep track of the current word
				std::vector<char> currentWord {};

				for ( uint32_t j = 0; j < p_buffer[i].size(); j++ )
				{
					// Just went onto a word
					if ( !onWord && ( p_buffer[i][j] != ' ' && p_buffer[i][j] != '\t' && p_buffer[i][j] != ',' ) )
					{
						onWord = true;

						// Remove comments
						if ( p_buffer[i][j] == COMMENT_CHAR ) break;

						// Start the current word with the current char
						currentWord = { p_buffer[i][j] };
					}
					else if ( ( onWord && ( p_buffer[i][j] == ' ' || p_buffer[i][j] == '\t' || p_buffer[i][j] == ',' ) ) || j == p_buffer[i].size() - 1 ) // Just left a word or on final char
					{
						onWord = false;

						// If on final char add the last char
						if ( j == p_buffer[i].size() - 1 )
						{
							// Add the current char
							currentWord.push_back( p_buffer[i][j] );
						}

						// Convert the vector into a char*
						char* curWord = new char[currentWord.size() + 1];
						for ( uint32_t k = 0; k < currentWord.size(); k++ )
							curWord[k] = currentWord[k];
						curWord[currentWord.size()] = '\0';

						// Add the current word to the word list
						words.push_back( curWord );

						// Create a token of the word
						Token currentToken( words[words.size() - 1] );

						// Check to see if word is a register
						if ( currentToken.value[0] == REGISTER_CHAR )
						{
							currentToken.type = TokenType::REGISTER;
							goto addToken;
						}

						// Check to see if word is a register
						if ( currentToken.value[0] == ADDRESS_CHAR )
						{
							currentToken.type = TokenType::ADDRESS;
							goto addToken;
						}

						// Iterate over operation map to see if word is an operation
						for ( const auto& operation : operationMap )
						{
							if ( currentToken.value == operation.first )
							{
								currentToken.type = TokenType::OPERATION;
								goto addToken;
							}
						}

						// Check to see if it is a numerical value

						// Check for hex numbers
						for ( uint32_t i = 0; ( ARR_LENGTH( HEX_PREFIX ) > 1 ? i < ARR_LENGTH( HEX_PREFIX ) - 1 : false ); i++ )
						{
							if ( currentToken.value[i] != HEX_PREFIX[i] ) // Not the hex prefix
								break;
							else if ( i == ARR_LENGTH( HEX_PREFIX ) - 2 ) // On final one and it was all the same
							{
								currentToken.type = TokenType::NUMERICAL_HEX;
								goto addToken;
							}
						}

						// Check for binary numbers
						for ( uint32_t i = 0; ( ARR_LENGTH( BIN_PREFIX ) > 1 ? i < ARR_LENGTH( BIN_PREFIX ) - 1 : false ); i++ )
						{
							if ( currentToken.value[i] != BIN_PREFIX[i] ) // Not the binary prefix
								break;
							else if ( i == ARR_LENGTH( BIN_PREFIX ) - 2 ) // On final one and it was all the same
							{
								currentToken.type = TokenType::NUMERICAL_BIN;
								goto addToken;
							}
						}

						// If it was none of these then it is a decimal
						currentToken.type = TokenType::NUMERICAL_DEC;

					addToken:
						tokens.push_back( currentToken );
					}
					else if ( onWord ) // Still on a word
					{
						// Add the current char
						currentWord.push_back( p_buffer[i][j] );
					}
				}
			}

			// Parse the tokens
			for ( uint32_t i = 0; i < tokens.size(); i++ )
			{
				// Operations
				if ( tokens[i].type == TokenType::OPERATION )
				{
					// Adds the opcode for this general operation (it will be altered to be correct)
					bytes.push_back( operationMap[tokens[i].value].first );

					// Requires a register in the first slot and value in second
					if ( operationMap[tokens[i].value].second )
					{
						// There are enough tokens to fulfil this
						if ( tokens.size() > i + 2 )
						{
							// The next token is a register
							if ( tokens[i + 1].type == TokenType::REGISTER )
							{
								// Get the register index and add it to bytes
								bytes.push_back( GetRegister( tokens[i + 1].value.c_str() ) );

								// Is a numerical value
								if ( tokens[i + 2].type == TokenType::NUMERICAL_HEX || tokens[i + 2].type == TokenType::NUMERICAL_BIN || tokens[i + 2].type == TokenType::NUMERICAL_DEC )
								{
									// Change to be the correct opcode
									bytes[bytes.size() - 2] += 0x00;

									// Add the converted integer to the bytes vector, convert using the appropriate base
									uint16_t value = GetNumerical( tokens[i + 2].value.c_str(), tokens[i + 2].type );
									bytes.push_back( ( uint8_t )( value >> 8 ) );
									bytes.push_back( ( uint8_t )( value & 0xFF ) );
								}
								// Is a register
								else if ( tokens[i + 2].type == TokenType::REGISTER )
								{
									// Change to be the correct opcode
									bytes[bytes.size() - 2] += 0x01;

									// Push back these placeholder bytes
									bytes.push_back( 0x00 );

									// Push back the register index
									bytes.push_back( GetRegister( tokens[i + 2].value.c_str() ) );
								}
								else if ( tokens[i + 2].type == TokenType::ADDRESS )
								{
									// Change to be the correct opcode
									bytes[bytes.size() - 2] += 0x02;

									// Add the converted integer to the bytes vector, convert using the appropriate base
									uint16_t value = GetNumerical( tokens[i + 2].value.c_str(), tokens[i + 2].type );
									bytes.push_back( ( uint8_t )( value >> 8 ) );
									bytes.push_back( ( uint8_t )( value & 0xFF ) );
								}

								// Increment i by 2 because there are 2 following words and they've been processed
								i += 2;
							}
						}
					}
					else // Doesn't require a register
					{
						// Misc
						if ( tokens[i].value == HALT_OP || tokens[i].value == NO_OP )
						{
							// Add the proceeding bytes
							bytes.push_back( operationMap[tokens[i].value].first );
							bytes.push_back( operationMap[tokens[i].value].first );
							bytes.push_back( operationMap[tokens[i].value].first );
						}
					}
				}
			}
		}
		catch ( const std::exception& e )
		{
			std::cerr << "Error: " << e.what() << std::endl;
		}

		return bytes;
	}
};