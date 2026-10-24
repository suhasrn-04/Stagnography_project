#include <stdio.h>
#include "encode.h"
#include "decode.h"

int main(int argc, char *argv[])
{
    EncodeInfo encodeInfo;

    if( argc == 1 )
    {
	// print  -> Error + usage msg
    printf("Invalid input it should have more than one command");
	return 0;
    }

    int opr = check_operation(argv[1]);

    if( opr == encode )
    {
	if( argc < 4 || argc > 5 )
	{
	    // print -> Error + usage msg
        printf("Invalid input !! input should have more 4 or 5")
	    return 0;
	}
    if(do_encoding(argv, &encodeInfo) == success)
    {
        printf("Encoding completed successfully\n");
    }
    else
    {
        printf("Encoding failed\n");
    }

    }
    else if( opr == decode )
    {
	if( argc < 3 || argc > 4 )	
	{
	    // print -> Error + usage msg
        printf("Error the decode arguments should be 3 or 4\n");
	    return 0;
	}
    }
    else
    {
	// Print -> Error + usage msg
    printf("Invalid arguments");
    }
    return 0;
}

