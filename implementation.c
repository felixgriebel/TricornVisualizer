#include <stdio.h>
#include <complex.h>
#include <stdlib.h>
#include <limits.h>
#include <getopt.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <errno.h>
#include <xmmintrin.h>
#include <emmintrin.h>
#include <pmmintrin.h>

// CONSTANT VALUES FOR THE PROGRAM
#define BORDER_FOR_CALCULATION 4.0
#define MAX_FILENAME_LENGTH 24
#define MIN_RES_VALUE 0.00000000000000000000000000000000001f
#define MAX_RES_VALUE 6000000.0f
#define MAX_SIZE_T 65535
#define MAX_VERSION 4
#define MAX_COLOR_SCHEME 3

// PRE-DECLARATION---------------------------------------------------------------------------------------------
void tricorn_V0(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img);
void tricorn_V1(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img);
void tricorn_V2(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img);
void tricorn_V3(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img);
void tricorn_V4(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img);
int writeBMP(unsigned char *matrix, size_t height, size_t width, char *filename, int colorscheme);

/*MAIN FUNCTION-----------------------------------------------------------------------------------------------

    contains the getopt construct and the calls for the different versions.


*/
int main(int argc, char **argv)
{

    // PARAMETER---------------------------------------------------------------------------------------------------
    int versionnumber = 0;                     // describes the version that is used
    int measurementsActive = 0;                // boolean value that describes if benchmarking is active
    unsigned long amountOfCalls = 200;         // amount of benchmarking cycles
    float complex start = (-2.0) + (-2.0) * I; // marks the start point (at the most lower-left corner of the calculation area)
    size_t width = 2000;                       // amount of hoizontal pixels/measurement points
    size_t height = 2000;                      // amount of vertal pixels/measurement points
    unsigned iterations = 100;                 // number of iterations
    float res = 0.002;                         // the resolution/step-width for the calculation
    char *filename;                            // name of the file
    int colorscheme = 0;

    // malloc memory for filename
    if ((filename = (char *)malloc((8 * sizeof(char)))) == NULL)
    {
        perror("Could not allocate memory!\n");
        return -1;
    }
    strcpy(filename, "out");

    // configuration of the getopt construct
    static const struct option long_options[] =
        {
            {"help", no_argument, 0, 'h'},
            {NULL,0,NULL,0}
        };

    int option_index = 0;
    char c = getopt_long(argc, argv, "V:B::s:d:n:r:o:hc:", long_options, &option_index);

    while (c != -1)
    {

        switch (c)
        {

        case 'V':
            // sets the version number
            // if standard version is wished for
            if (optarg == NULL)
            {
                versionnumber = 0;
                break;
            }

            int helpv = atoi(optarg);
            // check for invalid numbers and an error during the conversion
            if (helpv < 0 || helpv > MAX_VERSION)
            {
                perror("This version does not exist.\n");
            }
            else
            {
                versionnumber = helpv;
            }
            break;

        case 'B':
            // sets the testing to active and sets the amount of calls
            measurementsActive = 1;
            if (optarg == NULL)
            {
                break;
            }
            char *helpptrb = NULL;
            if(strtol(optarg, &helpptrb, 10)<0){
                perror("The number is not in between the boundaries of an unsigned long!\n");
                break;
            }
            unsigned long helpb = strtoul(optarg, &helpptrb, 10);

            if (helpb != 0)
            {
                if (helpb <= 0 || helpb >= ULONG_MAX)
                {
                    perror("The number is not in between the boundaries of an unsigned long!\n");
                }
                else
                {
                    amountOfCalls = helpb;
                }
            }
            else
            {
                perror("The input for option '-B' was not valid!\n");
            }
            break;

        case 's':
            // converts to floats, seperated via comma to a complex number
            float re;
            float im;
            char *helpptrs = NULL;

            re = strtof(optarg, &helpptrs);
            if (optarg != helpptrs)
            {
                optarg = helpptrs;
                helpptrs = NULL;
                // check for the comma in between
                if (*optarg == ',')
                {
                    optarg++;

                    im = strtof(optarg, &helpptrs);
                    if (optarg != helpptrs && re != NAN && re != INFINITY && im != NAN && im != INFINITY)
                    {
                        start = re + im * I;
                    }
                    else
                        perror("Input error, something went wrong with the imaginary part!\n");
                }
                else
                    perror("Input error, real and imaginary part should be seperated via comma!\n");
            }
            else
            {
                perror("Input error, complex number expected! \n");
            }
            break;

        case 'd':
            // width and height calculation and conversion from string to numbers
            size_t helpwidth;
            size_t helpheight;
            if (sscanf(optarg, "%zu,%zu", &helpwidth, &helpheight) == 2)
            {
                if ((size_t)MAX_SIZE_T > helpwidth && helpwidth > 0)
                {
                    width = helpwidth;
                }
                if ((size_t)MAX_SIZE_T > helpheight && helpheight > 0)
                {
                    height = helpheight;
                }
            }
            else
                perror("Input error converting the width and height!\n");

            break;

        case 'n':
            // converts the to the unsigned títerations
            char *helpptrn = NULL;
            unsigned long helpn = strtoul(optarg, &helpptrn, 10);
            if (helpn != 0)
            {
                if (helpn <= 0 || helpn > UINT_MAX)
                {
                    perror("The number is not in between the boundaries of an unsigned int!\n");
                }
                else
                    iterations = (unsigned)helpn;
            }
            else
            {
                perror("The number should end without string literals!\n");
            }

            break;

        case 'r':
            // converts the resolution
            float helpr;
            char *helpptrr = NULL;
            errno = 0;

            helpr = strtof(optarg, &helpptrr);
            if (optarg != helpptrr && helpr != NAN && helpr != INFINITY)
            {
                // limits the resolution
                if (helpr < MAX_RES_VALUE && helpr > MIN_RES_VALUE)
                {
                    res = helpr;
                }
                else
                {
                    perror("Resolution is not in a Reasonable range! \n");
                }
            }
            else
            {
                perror("Input error, float number expected! \n");
            }

            break;

        case 'o':
            // changes the name of the output file
            if (strlen(optarg) > MAX_FILENAME_LENGTH)
            {
                perror("Filename is too long!\n");
                break;
            }
            char *helpptro = (char *)malloc(sizeof(char) * (strlen(optarg) + 1));

            if (helpptro != NULL)
            {
                free(filename);
                filename = helpptro;
                strcpy(filename, optarg);
            }
            break;

        case 'h':
            printf("MANUAL FOR THE PROGRAM:\n\n\n[OPTIONS]:\n\nAfter calling the program, there will be a list of which parameters were used.\n_ ->    You can use as much SPACE as you want in between. Otherwise, you aren't allowed to put any SPACE.\n        You should not write this character after the option!\n\n");
            printf("    -V_<INT>\n        The number defines which implementation of the tricorn function is used.\n        The possible values are between 0 and 4 (included).\n        By default Version 0 is used.\n        To read more about the different versions, read below.\n\n");
            printf("    -B<UNSIGNED LONG> / -B\n        Activates the benchmarking. With a number directly after the B the function is called <NUMBER>-times.\n        It is recommended to use this flag as the first one.\n        The value of the benchmarks should be in the range of an unsigned long. \n        The default value is 200 calls for benchmarking.\n\n");
            printf("    -s_<FLOAT>,<FLOAT>\n        The option defines the start-point of the calculation area. \n        This point will be the left-lower corner of the area. \n        The first float is the real-part of the complex number, and the second float is the imaginary-part.\n        The default starting point is '-2.0 - 1.0i'.\n\n");
            printf("    -d_<UNSIGNED INT>,<UNSIGNED INT>\n        This will set the width and height of the calculated area.\n        The first number is the width, and the second is the height.\n        These dimensions define the measurement points, or pixels.\n        Therefore the actual width in the complex area is measured by width*resolution.\n        If you start at a point and want to measure to another point , you should consider this.\n        To calculate the width you need to put in, you can calculate the actual width of the complex area as follows:\n\n            width(input) = width(actual)/resolution \n\n        The default value of this is 2000,2000.\n\n");
            printf("    -n_<UNSIGNED LONG>\n        The option n sets the number of iterations for every complex point to the given number.\n        The value is by default 100.\n\n");
            printf("    -r_<FLOAT>\n        This value sets the resolution for the given float.\n        You can consider the resolution to be the difference between two neighboring measurement points.\n        By default this value will be 0.002 \n        You can only put in values between 6,000,000 and 1*10^-35\n\n");
            printf("    -o_<STRING>\n        With this option, the name of the output file will be set to <STRING>.bmp\n        The value is by default ' out '.\n        The String can only be 24 character long.\n\n");
            printf("    -c_<NUMBER>\n        Defines the coloration scheme of the output.\n        The value can only be between 0 and 3 (included) and is 0 by default.\n        To read more about the different colorations, read below.\n\n\n\n\n");
            printf("VERSIONS:\n\n");
            printf("    0\n        This is the default implementation of the tricorn function.\n        It uses no particular opimization.\n\n");
            printf("    1\n        This version is an improvement over the default version and uses a principle called area-deletion.\n        Some complex numbers that are for sure part of the tricorn set aren't calculated.\n        This can be useful for calls with a high number of iterations.\n        However, for calls with more measurement points and fewer iterations, this could not be beneficial.\n\n");
            printf("    2\n        This version is a further developed implementation of version 1.\n        Even more complex numbers that are for sure part of the tricorn set aren't calculated.\n        This can be useful for calls with a high number of iterations.\n        However, for calls with more measurement points and fewer iterations, this could not be beneficial.\n\n");
            printf("    3\n        This version is similar to the default one.\n        However, it uses SSE intrinsics to implement a SIMD variant of the default implementation.\n\n\n");
            printf("    4\n        This version is a mixture of the SIMD variant and the area-deletion version.\n\n\n");
            printf("COLORATION SCHEMES:\n\nAll schemes are based on which iteration of the series is not limited anymore.\n\n");
            printf("    0\n        The first coloration scheme is in different blue-variations.\n        The pixel is white if the correlating number is part of the tricorn set.\n        The fewer iterations needed to calculate that the number is not limited, the darker the coloration.\n\n");
            printf("    1\n        This scheme helps to distinguish the different areas better in comparison to the first-/blue-scheme.\n\n");
            printf("    2\n        This scheme is inverted. The points that are part of the tricorn set are colored black.\n        The fewer iterations needed to escape, the lighter the color is.\n\n");
            printf("    3\n        This scheme is a two-colored variant.\n        It helps to distinguish which point is part  of the tricorn set and which is not.\n        Points that are part of it are colored white, and ones that are not are colored black.\n");
            free(filename);
            return 0;
            break;
        case 'c':
            int helpc = atoi(optarg);
            // check for invalid numbers and an error during the conversion
            if (helpc < 0 || helpc > MAX_COLOR_SCHEME)
            {
                perror("This color scheme does not exist.\n");
            }
            else
            {
                colorscheme = helpc;
            }
            break;

        case '?':
            break;

        default:
            abort();
        }

        c = getopt_long(argc, argv, "V:B:s:d:n:r:o:hc:", long_options, &option_index);
    }

    // Details what was choosen
    printf("Startpoint:          %f + %f i\n", creal(start), cimag(start));
    printf("Width x Height:      %i x %i\n", (int)width, (int)height);
    printf("resolution:          %f\n", res);
    printf("scientif. res:       %E\n", res);
    printf("Iterations:          %u\n", iterations);
    printf("version:             %i\n", versionnumber);
    if (measurementsActive != 0)
    {
        printf("benchmarking status: active\n");
        printf("benchmark calls:     %lu\n", amountOfCalls);
    }
    else
    {
        printf("benchmarking status: inactive\n");
    }
    printf("output-file name:    %s\n", filename);
    if (measurementsActive == 0)
    {
        printf("coloration:          %i\n", colorscheme);
    }

    // allocates memory for the matrix for calculation
    unsigned char *outputMatrix = malloc(sizeof(unsigned char) * width * height);
    if (!outputMatrix)
    {
        perror("Could not allocate enough bytes for the filename.\n");
        free(filename);
        return -1;
    }

    // benchmarking active
    if (measurementsActive)
    {
        double time = 0;
        struct timespec starttime;
        struct timespec end;

        switch (versionnumber)
        {
        case 0:

            clock_gettime(CLOCK_MONOTONIC, &starttime);

            for (unsigned long measurei = 0; measurei < amountOfCalls; measurei++)
            {
                tricorn_V0(start, width, height, res, iterations, outputMatrix);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            time += end.tv_sec - starttime.tv_sec + 1e-9 * (end.tv_nsec - starttime.tv_nsec);

            break;
        case 1:

            clock_gettime(CLOCK_MONOTONIC, &starttime);

            for (unsigned long measurei = 0; measurei < amountOfCalls; measurei++)
            {
                tricorn_V1(start, width, height, res, iterations, outputMatrix);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            time += end.tv_sec - starttime.tv_sec + 1e-9 * (end.tv_nsec - starttime.tv_nsec);

            break;
        case 2:

            clock_gettime(CLOCK_MONOTONIC, &starttime);

            for (unsigned long measurei = 0; measurei < amountOfCalls; measurei++)
            {
                tricorn_V2(start, width, height, res, iterations, outputMatrix);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            time += end.tv_sec - starttime.tv_sec + 1e-9 * (end.tv_nsec - starttime.tv_nsec);

            break;
        case 3:

            clock_gettime(CLOCK_MONOTONIC, &starttime);

            for (unsigned long measurei = 0; measurei < amountOfCalls; measurei++)
            {
                tricorn_V3(start, width, height, res, iterations, outputMatrix);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            time += end.tv_sec - starttime.tv_sec + 1e-9 * (end.tv_nsec - starttime.tv_nsec);

            break;
        case 4:

            clock_gettime(CLOCK_MONOTONIC, &starttime);

            for (unsigned long measurei = 0; measurei < amountOfCalls; measurei++)
            {
                tricorn_V4(start, width, height, res, iterations, outputMatrix);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            time += end.tv_sec - starttime.tv_sec + 1e-9 * (end.tv_nsec - starttime.tv_nsec);

            break;
        default:
            clock_gettime(CLOCK_MONOTONIC, &starttime);

            for (unsigned long measurei = 0; measurei < amountOfCalls; measurei++)
            {
                tricorn_V0(start, width, height, res, iterations, outputMatrix);
            }

            clock_gettime(CLOCK_MONOTONIC, &end);
            time += end.tv_sec - starttime.tv_sec + 1e-9 * (end.tv_nsec - starttime.tv_nsec);

            break;
        }
        // printing out the results
        printf("The version %i calculated %lux%lu points, %lu times in %f seconds.\n", versionnumber, width, height, amountOfCalls, time);
    }
    else
    { // without benchmarking
        switch (versionnumber)
        {
        case 0:
            tricorn_V0(start, width, height, res, iterations, outputMatrix);
            break;
        case 1:
            tricorn_V1(start, width, height, res, iterations, outputMatrix);
            break;
        case 2:
            tricorn_V2(start, width, height, res, iterations, outputMatrix);
            break;
        case 3:
            tricorn_V3(start, width, height, res, iterations, outputMatrix);
            break;
        case 4:
            tricorn_V4(start, width, height, res, iterations, outputMatrix);
            break;
        default:
            tricorn_V0(start, width, height, res, iterations, outputMatrix);
            break;
        }
        writeBMP(outputMatrix, height, width, filename, colorscheme);
    }

    // frees the two allocated memory areas
    free(outputMatrix);
    free(filename);

    return 0;
}





/*


    THIS FUNCTION WRITES A .BMP FILE with the given input to the output file.

    depending on the give color scheme, the output wil follow different coloring-rules
    
    
*/
int writeBMP(unsigned char *matrix, size_t height, size_t width, char *filename, int colorscheme)
{

    // CONSTANTS FOR BMP-FILE--------------------------------------------------------------------------------------
    const char HEADER_B = 66;
    const char Header_M = 77;
    const int HEADER_ZERO = 0;
    const int HEADER_SIZE = 14;
    const int INFO_HEADER_SIZE = 40;
    const int HEADER_COLOR_PLANES = 1;
    const int HEADER_BIT_PER_PIXEL = 24;

    FILE *outputFile = fopen(strcat(filename, ".bmp"), "wb");

    // identify the BMP file with 'BM'
    fwrite(&HEADER_B, 1, 1, outputFile);
    fwrite(&Header_M, 1, 1, outputFile);
    // write the total size of the bmp file
    int ceilinghelp = (int)ceil((width / 4.0));
    int rowsWithPadding = (size_t)((ceilinghelp)*4) * 3;
    int sizeofBMP = (rowsWithPadding * height) + HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&sizeofBMP, 4, 1, outputFile);
    // 4 zero bytes
    fwrite(&HEADER_ZERO, 4, 1, outputFile);
    // offset of the pixels
    int dataOffset = HEADER_SIZE + INFO_HEADER_SIZE;
    fwrite(&dataOffset, 4, 1, outputFile);

    fwrite(&INFO_HEADER_SIZE, 4, 1, outputFile);
    fwrite(&width, 4, 1, outputFile);

    fwrite(&height, 4, 1, outputFile);
    // color planes must be 1
    fwrite(&HEADER_COLOR_PLANES, 2, 1, outputFile);
    // defines the bit per pixel
    fwrite(&HEADER_BIT_PER_PIXEL, 2, 1, outputFile);
    // compression method used: -> Default is zero
    fwrite(&HEADER_ZERO, 4, 1, outputFile);
    size_t imageByteSize = (height * width * 3);
    fwrite(&imageByteSize, 4, 1, outputFile);
    // safe the resolution
    int resolutionX = 11811; // 300 dpi
    int resolutionY = 11811; // 300 dpi
    fwrite(&resolutionX, 4, 1, outputFile);
    fwrite(&resolutionY, 4, 1, outputFile);
    // colors in the palette, 0 for default
    fwrite(&HEADER_ZERO, 4, 1, outputFile);
    // all colors that are required, default is 0
    fwrite(&HEADER_ZERO, 4, 1, outputFile);

    for (size_t y = 0; y < height; y++)
    {
        for (size_t x = 0; x < width; x++)
        {
            char value = *(matrix + x + (width * y));

            int bgr = 0;
            //switch based on the different scheme writes the color of the pixels different
            switch (colorscheme)
            {
            case 0:

                if (value == -1)
                {
                    bgr = 0xffffff;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                else
                {
                    bgr = value * 50;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                break;
            case 1:
                if (value == -1)
                {
                    bgr = 0xffffff;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                else
                {
                    //rainbow scheme
                    switch (value % 21)
                    {
                    case 0:
                        bgr = 0xff0000;
                        break;
                    case 1:
                        bgr = 0xe02000;
                        break;
                    case 2:
                        bgr = 0xc04000;
                        break;
                    case 3:
                        bgr = 0xa06000;
                        break;
                    case 4:
                        bgr = 0x808000;
                        break;
                    case 5:
                        bgr = 0x60a000;
                        break;
                    case 6:
                        bgr = 0x40c000;
                        break;
                    case 7:
                        bgr = 0x20e000;
                        break;
                    case 8:
                        bgr = 0x00ff00;
                        break;
                    case 9:
                        bgr = 0x00e020;
                        break;
                    case 10:
                        bgr = 0x00c040;
                        break;
                    case 11:
                        bgr = 0x00a060;
                        break;
                    case 12:
                        bgr = 0x008080;
                        break;
                    case 13:
                        bgr = 0x0060a0;
                        break;
                    case 14:
                        bgr = 0x0040c0;
                        break;
                    case 15:
                        bgr = 0x0020e0;
                        break;
                    case 16:
                        bgr = 0x0000ff;
                        break;
                    case 17:
                        bgr = 0x2000e0;
                        break;
                    case 18:
                        bgr = 0x4000c0;
                        break;
                    case 19:
                        bgr = 0x6000a0;
                        break;
                    default:
                        bgr = 0x800080;
                        break;
                    }
                    fwrite(&bgr, 3, 1, outputFile);
                }

                break;
            case 2:
                if (value == -1)
                {
                    bgr = 0x000000;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                else
                {
                    bgr = 255 - ((value * 3) % 250);
                    fwrite(&bgr, 1, 1, outputFile);
                    fwrite(&bgr, 1, 1, outputFile);
                    fwrite(&bgr, 1, 1, outputFile);
                }
                break;
            case 3:
                if (value == -1)
                {
                    bgr = 0xffffff;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                else
                {
                    bgr = 0x101020;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                break;
            default:

                if (value == -1)
                {
                    bgr = 0xffffff;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                else
                {
                    bgr = value * 50;
                    fwrite(&bgr, 3, 1, outputFile);
                }
                break;
            }
        }

        // add the padding
        for (size_t padd = 0; padd < (rowsWithPadding - (width * 3)); padd++)
        {
            fwrite(&HEADER_ZERO, 1, 1, outputFile);
        }
    }
    fclose(outputFile);

    return 0;
}

/*


    FIRST VERSION OF TRICORN CALCULATION WITHOUT ANY OPTIMIZATION


*/
void tricorn_V0(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img)
{

    float currRealX = creal(start);
    float currImagY = cimag(start);

    for (size_t y = 0; y < height - 1; y++)
    {
        // resets the x part for the next line
        currRealX = creal(start);

        for (size_t x = 0; x < width - 1; x++)
        {

            // make the calculation here
            float real = currRealX;
            float imag = currImagY;

            unsigned itererhelp = 0;

            while ((((real * real) + (imag * imag)) < BORDER_FOR_CALCULATION) && itererhelp < n)
            {
                float xtemp = (real * real) - (imag * imag) + currRealX;
                imag = (((-2) * real * imag) + currImagY);
                real = xtemp;

                itererhelp++;
            }

            if (itererhelp == n)
            {
                itererhelp = -1;
            }

            *(img + x + width * y) = itererhelp;

            currRealX += res;
        }
        currImagY += res;
    }
    return;
}

/*


    SECOND VERSION OF TRICORN IMPLEMENTATION WITH ARE DELETION VERSION 1
    (-> uses four checks for area deletion)


*/
void tricorn_V1(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img)
{

    float realstarter = creal(start);
    float currRealX = realstarter;
    float currImagY = cimag(start);

    for (size_t y = 0; y < height - 1; y++)
    {

        if (currImagY >= 2.0)
        {
            for (size_t x = 0; x < width - 1; x++)
            {
                *(img + x + width * y) = 0;
            }

            continue;
        }

        // resets the x part for the next line
        currRealX = realstarter;

        for (size_t x = 0; x < width - 1; x++)
        {
            
            //The if-statements are used for area deletion
            if (((currRealX) * (currRealX)) + (currImagY * currImagY) < 0.05)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX + 1) * (currRealX + 1)) + (currImagY * currImagY) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX - 0.472) * (currRealX - 0.472)) + ((currImagY - 0.82) * (currImagY - 0.82)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX - 0.472) * (currRealX - 0.472)) + ((currImagY + 0.82) * (currImagY + 0.82)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }

            // make the calculation here
            float real = currRealX;
            float imag = currImagY;

            unsigned itererhelp = 0;

            while ((((real * real) + (imag * imag)) < BORDER_FOR_CALCULATION) && itererhelp < n)
            {
                float xtemp = (real * real) - (imag * imag) + currRealX;
                imag = (((-2) * real * imag) + currImagY);
                real = xtemp;

                itererhelp++;
            }

            if (itererhelp == n)
            {
                itererhelp = -1;
            }

            *(img + x + width * y) = itererhelp;

            currRealX += res;
        }
        currImagY += res;
    }
    return;
}

/*


    THIRD VERSION OF TRICORN IMPLEMENTATION WITH AREA DELETION VERSION 2
    (-> uses seven checks for area deletion)


*/
void tricorn_V2(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img)
{

    float realstarter = creal(start);
    float currRealX = realstarter;
    float currImagY = cimag(start);

    for (size_t y = 0; y < height - 1; y++)
    {

        if (currImagY >= 2.0)
        {
            for (size_t x = 0; x < width - 1; x++)
            {
                *(img + x + width * y) = 0;
            }

            continue;
        }

        // resets the x part for the next line
        currRealX = realstarter;

        for (size_t x = 0; x < width - 1; x++)
        {

            //The if-statements are used for area deletion
            if (((currRealX) * (currRealX)) + (currImagY * currImagY) < 0.05)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX + 1) * (currRealX + 1)) + (currImagY * currImagY) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX - 0.472) * (currRealX - 0.472)) + ((currImagY - 0.82) * (currImagY - 0.82)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX - 0.472) * (currRealX - 0.472)) + ((currImagY + 0.82) * (currImagY + 0.82)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }

            if (((currRealX - 0.15) * (currRealX - 0.15)) + ((currImagY - 0.26) * (currImagY - 0.26)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX - 0.15) * (currRealX - 0.15)) + ((currImagY + 0.26) * (currImagY + 0.26)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }
            if (((currRealX + 0.3) * (currRealX + 0.3)) + ((currImagY) * (currImagY)) < 0.007)
            {
                *(img + x + width * y) = -1;
                currRealX += res;
                continue;
            }

            // make the calculation here
            float real = currRealX;
            float imag = currImagY;

            unsigned itererhelp = 0;

            while ((((real * real) + (imag * imag)) < BORDER_FOR_CALCULATION) && itererhelp < n)
            {
                float xtemp = (real * real) - (imag * imag) + currRealX;
                imag = (((-2) * real * imag) + currImagY);
                real = xtemp;

                itererhelp++;
            }

            if (itererhelp == n)
            {
                itererhelp = -1;
            }

            *(img + x + width * y) = itererhelp;

            currRealX += res;
        }
        currImagY += res;
    }
    return;
}

/*


    FOURTH VERSION. Is the same algorithm as tricorn_V0 but uses SIMD to optimize


*/
void tricorn_V3(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img)
{

    float currRealX = creal(start);
    float currImagY = cimag(start);

    // minus two for calculation
    const __m128 minustwo128 = _mm_set_ps1(-2.0f);
    const __m128 border128 = _mm_set_ps1(BORDER_FOR_CALCULATION);

    for (size_t y = 0; y < height - 1; y++)
    {
        // resets the x part for the next line
        currRealX = creal(start);

        // SIMD loop
        for (size_t xf = 0; xf < width / 4; xf++)
        {

            __m128 realpart = _mm_set_ps(currRealX, currRealX + res, currRealX + (2.0 * res), currRealX + (3.0 * res));
            __m128 imagpart = _mm_set_ps1(currImagY);

            // xx and yy
            __m128 xtimestwo = _mm_mul_ps(realpart, realpart);
            __m128 ytimestwo = _mm_mul_ps(imagpart, imagpart);
            __m128 norming = _mm_add_ps(xtimestwo, ytimestwo);

            // compares the two float-vectors and should result in 0xffffffff or 0x00000000
            __m128 helpingb = _mm_cmpgt_ps(border128, norming);
            unsigned int maskingAsInt = _mm_movemask_ps(helpingb);

            __m128 real128 = realpart;
            __m128 imag128 = imagpart;

            unsigned results[4] = {0, 0, 0, 0};

            unsigned counter = 0;

            // while there is still something to calc
            while (counter < n && (maskingAsInt != 0))
            {
                // doing the computation
                __m128 tempx = _mm_add_ps(_mm_sub_ps(xtimestwo, ytimestwo), realpart);
                imag128 = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(real128, imag128), minustwo128), imagpart);
                real128 = tempx;

                // norms the number
                xtimestwo = _mm_mul_ps(real128, real128);
                ytimestwo = _mm_mul_ps(imag128, imag128);
                norming = _mm_add_ps(xtimestwo, ytimestwo);

                // adds the results
                int zeh = 0;
                for (int i = 1; i < 9; i *= 2, zeh++)
                {
                    if (maskingAsInt & i)
                    {
                        results[zeh]++;
                    }
                }
                // ands the previous bitmask with the current one and updates it this way
                helpingb = _mm_cmpgt_ps(border128, norming);
                maskingAsInt = maskingAsInt & (_mm_movemask_ps(helpingb));

                counter++;
            }
            for (size_t i = 0; i < 4; i++)
            {
                unsigned char tempresults = results[i];
                if (tempresults == n)
                {
                    *(img + (4 * xf) + ((3 - i)) + width * y) = -1;
                }
                else
                {
                    *(img + (4 * xf) + (3 - i) + width * y) = tempresults;
                }
            }
            currRealX = currRealX + 4 * res;
        }

        // Tthe single loop
        for (size_t x = 0; x < (width % 4); x++)
        {

            // make the calculation here
            float real = currRealX;
            float imag = currImagY;

            unsigned itererhelp = 0;

            while ((((real * real) + (imag * imag)) < BORDER_FOR_CALCULATION) && itererhelp < n)
            {

                float xtemp = (real * real) - (imag * imag) + currRealX;
                imag = (((-2) * real * imag) + currImagY);
                real = xtemp;

                itererhelp++;
            }

            if (itererhelp == n)
            {
                itererhelp = -1;
            }

            *(img + x + width * y) = itererhelp;

            currRealX += res;
        }
        currImagY += res;
    }
    return;
}












/*

    FIFTH VERSION OF THE TRIVORN CALCULATION. Uses a mixture of the SIMD functionality of tricorn_V3 and area deletion
    (->uses one area check)


*/

void tricorn_V4(float complex start, size_t width, size_t height, float res, unsigned n, unsigned char *img)
{

    float currRealX = creal(start);
    float currImagY = cimag(start);

    // minus two for calculation
    const __m128 minustwo128 = _mm_set_ps1(-2.0f);
    const __m128 border128 = _mm_set_ps1(BORDER_FOR_CALCULATION);

    for (size_t y = 0; y < height - 1; y++)
    {
        // resets the x part for the next line
        currRealX = creal(start);

        // SIMD loop
        for (size_t xf = 0; xf < width / 4; xf++)
        {

            __m128 realpart = _mm_set_ps(currRealX, currRealX + res, currRealX + (2.0 * res), currRealX + (3.0 * res));
            __m128 imagpart = _mm_set_ps1(currImagY);

            // xx and yy
            __m128 xtimestwo = _mm_mul_ps(realpart, realpart);
            __m128 ytimestwo = _mm_mul_ps(imagpart, imagpart);
            __m128 norming = _mm_add_ps(xtimestwo, ytimestwo);


            int areadeletion = _mm_movemask_ps(_mm_cmpge_ps(norming,_mm_set_ps1(0.05)));
            if(areadeletion==0){
                for (size_t i = 0; i < 4; i++)
                {
                    *(img + (4 * xf) + ((3 - i)) + width * y) = -1;
                
            }
            currRealX = currRealX + 4 * res;
            continue;
            }



            // compares the two float-vectors and should result in 0xffffffff or 0x00000000
            __m128 helpingb = _mm_cmpgt_ps(border128, norming);
            unsigned int maskingAsInt = _mm_movemask_ps(helpingb);

            __m128 real128 = realpart;
            __m128 imag128 = imagpart;

            unsigned results[4] = {0, 0, 0, 0};

            unsigned counter = 0;

            // while there is still something to calc
            while (counter < n && (maskingAsInt != 0))
            {
                // doing the computation
                __m128 tempx = _mm_add_ps(_mm_sub_ps(xtimestwo, ytimestwo), realpart);
                imag128 = _mm_add_ps(_mm_mul_ps(_mm_mul_ps(real128, imag128), minustwo128), imagpart);
                real128 = tempx;

                // norms the number
                xtimestwo = _mm_mul_ps(real128, real128);
                ytimestwo = _mm_mul_ps(imag128, imag128);
                norming = _mm_add_ps(xtimestwo, ytimestwo);



                // adds the results
                int zeh = 0;
                for (int i = 1; i < 9; i *= 2, zeh++)
                {
                    if (maskingAsInt & i)
                    {
                        results[zeh]++;
                    }
                }
                // ands the previous bitmask with the current one and updates it this way
                helpingb = _mm_cmpgt_ps(border128, norming);
                maskingAsInt = maskingAsInt & (_mm_movemask_ps(helpingb));

                counter++;
            }
            for (size_t i = 0; i < 4; i++)
            {
                unsigned char tempresults = results[i];
                if (tempresults == n)
                {
                    *(img + (4 * xf) + ((3 - i)) + width * y) = -1;
                }
                else
                {
                    *(img + (4 * xf) + (3 - i) + width * y) = tempresults;
                }
            }
            currRealX = currRealX + 4 * res;
        }

        // Tthe single loop
        for (size_t x = 0; x < (width % 4); x++)
        {

            // make the calculation here
            float real = currRealX;
            float imag = currImagY;

            unsigned itererhelp = 0;

            while ((((real * real) + (imag * imag)) < BORDER_FOR_CALCULATION) && itererhelp < n)
            {

                float xtemp = (real * real) - (imag * imag) + currRealX;
                imag = (((-2) * real * imag) + currImagY);
                real = xtemp;

                itererhelp++;
            }

            if (itererhelp == n)
            {
                itererhelp = -1;
            }

            *(img + x + width * y) = itererhelp;

            currRealX += res;
        }
        currImagY += res;
    }
    return;
}