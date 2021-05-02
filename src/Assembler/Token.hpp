#pragma once
#include "../IntDef.hpp"

#include <string>
#include <unordered_map>

#define LOAD_ALL_OP "LDA"
#define LOAD_LO_OP	"LDL"
#define LOAD_HI_OP	"LDH"

#define STORE_ALL_OP "STA"
#define STORE_LO_OP	 "STL"
#define STORE_HI_OP	 "STH"

#define COMPARE_OP "CMP"

#define BRANCH_EQ_OP	   "BRE"
#define BRANCH_LT_OP	   "BLT"
#define BRANCH_GT_OP	   "BGT"
#define BRANCH_ALWAYS_OP   "BRA"
#define BRANCH_POSITIVE_OP "BRP"
#define BRANCH_NEGATIVE_OP "BRN"

#define ADD_OP "ADD"
#define SUB_OP "SUB"

#define HALT_OP "HLT"
#define NO_OP	"NOOP"

// Returns the opcode and if it requires a register and value
std::unordered_map<std::string, const std::pair<uint8_t, bool>> operationMap {
	{ LOAD_ALL_OP, { 0x00, true } },
	{ LOAD_LO_OP, { 0x03, true } },
	{ LOAD_HI_OP, { 0x06, true } },
	{ STORE_ALL_OP, { 0x10, true } },
	{ STORE_LO_OP, { 0x13, true } },
	{ STORE_HI_OP, { 0x16, true } },
	{ COMPARE_OP, { 0x20, true } },
	{ BRANCH_ALWAYS_OP, { 0x30, false } },
	{ BRANCH_EQ_OP, { 0x31, false } },
	{ BRANCH_GT_OP, { 0x32, false } },
	{ BRANCH_LT_OP, { 0x33, false } },
	{ BRANCH_POSITIVE_OP, { 0x34, false } },
	{ BRANCH_NEGATIVE_OP, { 0x35, false } },
	{ ADD_OP, { 0x40, true } },
	{ SUB_OP, { 0x41, true } },
	{ HALT_OP, { 0xFE, false } },
	{ NO_OP, { 0xFF, false } }
};

enum class TokenType : uint8_t
{
	UNKNOWN		  = 0,
	OPERATION	  = 1,
	REGISTER	  = 2,
	ADDRESS		  = 3,
	NUMERICAL_HEX = 4,
	NUMERICAL_BIN = 5,
	NUMERICAL_DEC = 6,
	LABEL		  = 7
};

#define COMMENT_CHAR  '#'
#define REGISTER_CHAR 'R'
#define ADDRESS_CHAR  '$'
#define LABEL_POSTFIX ':'

#define HEX_PREFIX "0x"
#define BIN_PREFIX "0b"
#define DEC_PREFIX ""

#define ARR_LENGTH( x ) sizeof( x ) / sizeof( x[0] )

struct Token
{
public:
	std::string value;
	TokenType	type;

	Token() : value( "" ), type( TokenType::UNKNOWN ) {}
	Token( std::string p_value, TokenType p_type = TokenType::UNKNOWN ) : value( p_value ), type( p_type ) {}
};

static uint8_t GetRegister( const char* p_register )
{
	// Remove first character, then convert to unsigned char
	return ( uint8_t )( std::stoi( std::string( &p_register[1] ), nullptr, 10 ) & 0xFF );
}

static uint16_t GetNumerical( const char* p_numerical, const TokenType& p_type )
{
	// Convert depending on type of numerical, then convert to unsigned short
	return ( uint16_t )( std::stoi( std::string( p_numerical ), nullptr, ( p_type == TokenType::NUMERICAL_HEX ? 16 : p_type == TokenType::NUMERICAL_BIN ? 2 : 10 ) ) & 0xFFFF );
}