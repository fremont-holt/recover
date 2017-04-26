/**
 * Read from raw memory card image recovering any jpgs
 */
 
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    typedef uint8_t  BYTE;
    
    // Check there is only one input
    if (argc != 2)
    {
        fprintf(stderr, "Usage: ./recover file\n");
        return 1;
    }
    
    char *infile = argv[1];
    
    // open input file 
    FILE* inptr = fopen(infile, "r");
    if (inptr == NULL)
    {
        fprintf(stderr, "Could not open %s.\n", infile);
        return 2;
    }
    
    int jpeg_count = 0;
    BYTE block[512];
    char file_name[8];
    FILE *img;
    
    // While there are blocks of 512 left read keep reading the file
    while( fread( &block, 1, 512, inptr ) == 512 )
    {
        // Check that this block is the start of a jpeg
        if( block[0] == 0xff &&
            block[1] == 0xd8 &&
            block[2] == 0xff &&
            ( block[3] & 0xf0 ) == 0xe0)
            {
                // Close previous jpeg if it is not the first
                if( jpeg_count > 0 )
                {
                    fclose( img );
                }
        
                sprintf( file_name, "%03d.jpg", jpeg_count );
                jpeg_count++;
                
                // Open a new jpeg
                img = fopen( file_name, "w" );
                if ( img == NULL )
                {
                    fclose( inptr );
                    fprintf( stderr, "Could not create %s.\n", file_name );
                    return 3;
                }
            }
        
        // Write to the current jpeg after one has been found
        if( jpeg_count > 0 )
        {
            fwrite( &block, 512, 1, img );
        }
    }
    // Close the file we are reading from
    fclose( inptr );
}