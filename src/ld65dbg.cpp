// ld65dbg.cpp
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "types.h"
#include "ld65dbg.h"


namespace ld65
{
	//---------------------------------------------------------------------------------------------------
	segment::segment( int id, const char *name, int startAddr, int size, int ofs, unsigned char type )
		: _name(name ? name : ""), _id(id), _startAddr(startAddr), _size(size), _ofs(ofs), _type(type)
	{
	}
	//---------------------------------------------------------------------------------------------------
	scope::scope(int id, const char *name, int size, int parentID)
		: _name(name ? name : ""), _id(id), _parentID(parentID), _size(size), _parent(nullptr)
	{
	}
	//---------------------------------------------------------------------------------------------------
	sym::sym(int id, const char *name, int size)
		: _name(name ? name : ""), _id(id), _size(size), _scope(nullptr)
	{
	}
	//---------------------------------------------------------------------------------------------------
	database::database(void)
	{
	}
	//---------------------------------------------------------------------------------------------------
	database::~database(void)
	{
		for (auto itSym = symMap.begin(); itSym != symMap.end(); itSym++)
		{
			delete itSym->second;
		}
		for (auto itScope = scopeMap.begin(); itScope != scopeMap.end(); itScope++)
		{
			delete itScope->second;
		}
		for (auto itSeg = segmentMap.begin(); itSeg != segmentMap.end(); itSeg++)
		{
			delete itSeg->second;
		}
	}
	//---------------------------------------------------------------------------------------------------
	database::dbgLine::dbgLine(size_t bufferSize)
	{
		buf = NULL;
		bufSize = 0;
		readPtr = 0;

		allocBuffer( bufferSize );
	}
	//---------------------------------------------------------------------------------------------------
	database::dbgLine::~dbgLine(void)
	{
		if (buf)
		{
			::free(buf); buf = NULL;
		}
		bufSize = 0;
		readPtr = 0;
	}
	//---------------------------------------------------------------------------------------------------
	void database::dbgLine::allocBuffer(size_t bufferSize)
	{
		if (buf)
		{
			::free(buf); buf = NULL;
		}
		bufSize = 0;
		readPtr = 0;

		buf = static_cast<char*>( ::malloc( bufferSize ) );

		if (buf == NULL)
		{
			bufSize = 0;
		}
		else
		{
			buf[0] = 0;
			bufSize = bufferSize;
		}
		readPtr = 0;
	}
	//---------------------------------------------------------------------------------------------------
	const char *database::dbgLine::readFromFile( FILE *fp )
	{
		readPtr = 0;

		return fgets(buf, bufSize, fp);
	}
	//---------------------------------------------------------------------------------------------------
	int database::dbgLine::readToken( char *tk, size_t tkSize )
	{
		int charsRead = 0;
		size_t i,j;

		i=readPtr; j=0;
		if ( buf[i] != 0 )
		{
			while (isspace(buf[i])) i++;

			if ( isalpha(buf[i]) || (buf[i] == '_') )
			{
				while ( isalnum(buf[i]) || (buf[i] == '_') )
				{
					if (j < tkSize)
					{
						tk[j] = buf[i]; j++;
					}
					i++;
				}
			}
			else if (buf[i] != 0)
			{
				if (j < tkSize)
				{
					tk[j] = buf[i]; j++;
				}
				i++;
			}
		}
		charsRead = j;
		readPtr = i;

		if (j < tkSize)
		{
			tk[j] = 0;
		}
		else
		{
			tk[tkSize-1] = 0;
		}
		return charsRead;
	}
	//---------------------------------------------------------------------------------------------------
	int database::dbgLine::readKeyValuePair( char *keyValueBuffer, size_t keyValueBufferSize )
	{
		int charsRead = 0;
		size_t i,j;
		bool isStringLiteral = false;

		i=readPtr; j=0;
		if ( buf[i] != 0 )
		{
			while (isspace(buf[i])) i++;

			if ( isalpha(buf[i]) || (buf[i] == '_') )
			{
				while ( isalnum(buf[i]) || (buf[i] == '_') )
				{
					if (j < keyValueBufferSize)
					{
						keyValueBuffer[j] = buf[i]; j++;
					}
					i++;
				}
			}
			else if (buf[i] != 0)
			{
				if (j < keyValueBufferSize)
				{
					keyValueBuffer[j] = buf[i]; j++;
				}
				i++;
			}

			while (isspace(buf[i])) i++;
		}

		if ( buf[i] == '=' )
		{
			if (j < keyValueBufferSize)
			{
				keyValueBuffer[j] = buf[i]; j++;
			}
			i++;

			while (isspace(buf[i])) i++;

			while ( buf[i] != 0 )
			{
				if ( !isStringLiteral && buf[i] == ',' )
				{
					break;
				}
				else if ( buf[i] == '\"' )
				{
					isStringLiteral = !isStringLiteral;
				}
				else
				{
					if (j < keyValueBufferSize)
					{
						if (!isspace(buf[i]))
						{
							keyValueBuffer[j] = buf[i]; j++;
						}
					}
				}
				i++;
			}
			if (buf[i] == ',')
			{
				i++;
			}
		}
		charsRead = j;
		readPtr = i;

		if (j < keyValueBufferSize)
		{
			keyValueBuffer[j] = 0;
		}
		else
		{
			keyValueBuffer[keyValueBufferSize-1] = 0;
		}
		return charsRead;
	}
	//---------------------------------------------------------------------------------------------------
	int database::dbgLine::splitKeyValuePair( char *keyValueBuffer, char **keyPtr, char **valuePtr )
	{
		size_t i=0;

		if (keyPtr != nullptr)
		{
			*keyPtr = keyValueBuffer;
		}
		while (keyValueBuffer[i] != 0)
		{
			if (keyValueBuffer[i] == '=')
			{
				keyValueBuffer[i] = 0; i++; break;
			}
			i++;
		}
		if (valuePtr != nullptr)
		{
			*valuePtr = &keyValueBuffer[i];
		}
		return 0;
	}
	//---------------------------------------------------------------------------------------------------
	int database::dbgFileLoad( const char *dbgFilePath )
	{
		static constexpr size_t lineSize = 4096;
		FILE *fp;
		dbgLine line( lineSize );
		char lineType[64];
		fceuScopedPtr <char> keyValueBuffer( new char[ lineSize ], fceuScopedPtr<char>::NewArray );

		fp = ::fopen( dbgFilePath, "r");

		if (fp == NULL)
		{
			return -1;
		}

		while ( line.readFromFile(fp) != NULL )
		{
			printf("%s", line.getLine());

			if ( line.readToken( lineType, sizeof(lineType) ) )
			{
				int id = -1, size = 0, parentID = -1, scopeID = -1;
				char name[256];

				name[0] = 0;

				while ( line.readKeyValuePair( keyValueBuffer.get(), lineSize) )
				{
					char *key, *val;

					line.splitKeyValuePair( keyValueBuffer.get(), &key, &val );

					printf("   Key '%s' -> Value '%s' \n", key, val );

					if ( strcmp( key, "id") == 0 )
					{
						id = strtol( val, nullptr, 0 );
					}
					else if ( strcmp( key, "name") == 0 )
					{
						strncpy( name, val, sizeof(name));
					}
					else if ( strcmp( key, "size") == 0 )
					{
						size = strtol( val, nullptr, 0 );
					}
					else if ( strcmp( key, "scope") == 0 )
					{
						scopeID = strtol( val, nullptr, 0 );
					}
					else if ( strcmp( key, "scope") == 0 )
					{
						parentID = strtol( val, nullptr, 0 );
					}
				}

				if ( strcmp( lineType, "scope" ) == 0 )
				{
					if ( id >= 0 )
					{
						scope *s = new scope( id, name, size, parentID );

						scopeMap[id] = s;

						auto it = scopeMap.find( parentID );

						if ( it != scopeMap.end() )
						{
							printf("Found Parent:%i for %i\n", parentID, id );
							s->_parent = it->second;
						}
					}
				}
				else if ( strcmp( lineType, "sym") == 0 )
				{
					if ( id >= 0 )
					{
						sym *s = new sym( id, name, size );

						auto it = scopeMap.find( scopeID );

						if ( it != scopeMap.end() )
						{
							printf("Found Scope:%i for %s\n", scopeID, name );
							s->_scope = it->second;
						}
						symMap[id] = s;
					}
				}
			}
		}
		::fclose(fp);

		return 0;
	}
	//---------------------------------------------------------------------------------------------------
}
