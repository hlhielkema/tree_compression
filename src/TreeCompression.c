// Compression algorithm
// (C) Hielke Hielkema
// HielkeHielkema93@gmail.com

/*---- Imports ---------------------------------------*/
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <time.h>

/*---- Constant macro's ---------------------------------------*/
#define TRUE 1
#define FALSE 0
#define CHAR_COUNT_BUFFER_SIZE 255
#define OUTPUT_BUFFER_SIZE 1000
#define PRINT_CHAR_COUNTS 0
#define PRINT_INPUT 1
#define PRINT_COMPRES_OUTPUT 1
#define PRINT_DECOMPRES_OUTPUT 1
#define PRINT_COMPRES_RATE 1
#define TEST_CASE 0 // 0, 1 or 2
#define PRINT_RUN_TIME 1

/*---- Other macro's ---------------------------------------*/
#define INITTIMER clock_t startm, stopm
#define START startm = clock()
#define STOP stopm = clock()
#define PRINTTIME printf("%2.3f ms used by the processor.", (((double)stopm-startm) * 1000)/CLOCKS_PER_SEC);

/*---- Typedefs ---------------------------------------*/
typedef struct { char _value; int _count, _path, _pathLen; } CharFrequency;

/*---- Prototypes ---------------------------------------*/
void compress_string(const char * input, char * outputBuffer, int * outputbufferSize);
void decompress_string(const char * input, char * outputBuffer, int * outputbufferSize);
void analyse_charachters(const char * input, CharFrequency ** charFrequencys, int * charFrequencysSize);
void creater_header(char * outputBuffer, int * outputbufferSize, CharFrequency ** charFrequencys, int charFrequencysSize);
void create_content(const char * input, CharFrequency ** charFrequencys, int charFrequencysSize, char * outputBuffer, int * outputbufferSize);
void read_header(const char * input, CharFrequency ** charFrequencys, int * charFrequencysSize);
void read_content(const char * input, char * output, int * output_c, CharFrequency ** charFrequencys, int charFrequencysSize);
void set_tree_path_for_id(CharFrequency * charFrequencys, int id);
void write_bit(char * output, int offset, int bPointer, char value);
char read_bit(const char * input, int offset, int bPointer);
int offset_for_layer(int layer);
int comp (const void * elem1, const void * elem2);
int ipow(int x, int p);

/*---- Main method ---------------------------------------*/

/**
 * Main method
 */
int main() {
	// Initialize the time measure system
	INITTIMER;

	// Select the input string
	#if TEST_CASE == 0
	const char * input = "Lorem ipsum dolor sit amet, consectetuer adipiscing elit. Aenean commodo ligula eget dolor. Aenean massa. Cum sociis natoque penatibus et magnis dis parturient montes, nascetur ridiculus mus. Donec quam felis, ultricies nec, pellentesque eu, pretium quis, sem. Nulla consequat massa quis enim. Donec pede justo, fringilla vel, aliquet nec, vulputate eget, arcu. In enim justo, rhoncus ut, imperdiet a, venenatis vitae, justo. Nullam dictum felis eu pede mollis pretium. Integer tincidunt. Cras dapibus. Vivamus elementum semper nisi. Aenean vulputate eleifend tellus. Aenean leo ligula, porttitor eu, consequat vitae, eleifend ac, enim. Aliquam lorem ante, dapibus in, viverra quis, feugiat a, tellus. Phasellus viverra nulla ut metus varius laoreet. Quisque rutrum. Aenean imperdiet. Etiam ultricies nisi vel augue. Curabitur ullamcorper ultricies nisi. Nam eget dui.";
	#endif
	#if TEST_CASE == 1
	const char * input = "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA";
	#endif
	#if TEST_CASE == 2
	const char * input = "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat.";
	#endif

	// Print the input string
	#if PRINT_INPUT
	printf("\n-- Input string --\n\n\"%s\"\n", input);
	#endif

	// Start the time measure system
	START;

	// Compress string
	char * compOutputBuffer = malloc(sizeof(char) * OUTPUT_BUFFER_SIZE);
	int compoutputbufferSize = 0;
	compress_string(input, compOutputBuffer, (int*)&compoutputbufferSize);

	// Stop the time measure system
	STOP;

	// Print the compression output
	#if PRINT_COMPRES_OUTPUT
	int i;
	printf("\n-- Compression output --\n\n\"");
	for(i = 0; i < compoutputbufferSize; i++) {
		if (isprint(compOutputBuffer[i]))
			printf("%c", compOutputBuffer[i]);
		else
			printf("*");
	}
	printf("\"\n\n");
	#endif

	// Print the execution time
	#if PRINT_RUN_TIME
	PRINTTIME;
	printf("\n\n");
	#endif

	// Print the compression rate
	#if PRINT_COMPRES_RATE
	int len = -1;
	while(input[++len] != '\0');
	float rate = (float)compoutputbufferSize / (float)len;
	printf("The size of the compressed string is %5.2f%% of the size of the input string. (%d vs %d)\n", (rate * 100), compoutputbufferSize, len);
	#endif

	// Start the time measure system
	START;

	// Decompress string
	char * decompOutputBuffer = malloc(sizeof(char) * OUTPUT_BUFFER_SIZE);
	int decompoutputbufferSize = 0;
	decompress_string(compOutputBuffer, decompOutputBuffer, (int*)&decompoutputbufferSize);

	// Stop the time measure system
	STOP;

	// Print the decompressed string
	#if PRINT_DECOMPRES_OUTPUT
	printf("\n-- Decompression output --\n\n\"");
	for(i = 0; i < decompoutputbufferSize; i++) {
		printf("%c", decompOutputBuffer[i]);
	}
	printf("\"\n\n");
	#endif

	// Print the execution time
	#if PRINT_RUN_TIME
	PRINTTIME;
	printf("\n\n");
	#endif

	// Free resources
	free(compOutputBuffer);
	free(decompOutputBuffer);
}

/*---- Compress string functions ---------------------------------------*/

/**
 * Compress a string
 * 
 * @param input input string
 * @param outputBuffer output string buffer
 * @param outputbufferSize output string buffer size
 */
void compress_string(const char * input, char * outputBuffer, int * outputbufferSize) {
	// Analyse the frequency of charachters and sort them
	CharFrequency ** charFrequencys = (CharFrequency**)malloc(sizeof(CharFrequency*) * CHAR_COUNT_BUFFER_SIZE);
	int charFrequencySize;
	analyse_charachters(input, charFrequencys, (int *)&charFrequencySize);
		
	// Set the path
	int i;
	for(i = 0; i < charFrequencySize; i++)
		set_tree_path_for_id(charFrequencys[i], i);

	// Write the charachter tree to the output buffer
	creater_header(outputBuffer, outputbufferSize, charFrequencys, charFrequencySize);

	// Write the content to the output buffer
	create_content(input, charFrequencys, charFrequencySize, outputBuffer, outputbufferSize);

	// Free resources
	free(charFrequencys);
}

/**
 * Analyse the frequency of charachters in a string
 *
 * @param input input string
 * @param charFrequencys charcher frquency buffer
 * @param CharFrequencySize charcher frquency buffer size
 */
void analyse_charachters(const char * input, CharFrequency ** charFrequencys, int * CharFrequencySize) {
	int input_p = 0, i;
	char ch, found;

	// Count all charachters in the string
	while(input[input_p] != '\0') {
		ch = input[input_p++];
		found = FALSE;
		for(i = 0; i < (*CharFrequencySize); i++) {
			if (ch == charFrequencys[i]->_value) {
				charFrequencys[i]->_count++;
				found = TRUE;
				break;
			}
		}
		if (found != TRUE) {
			charFrequencys[(*CharFrequencySize)++] = malloc(sizeof(CharFrequency));
			charFrequencys[(*CharFrequencySize)-1]->_value = ch;
			charFrequencys[(*CharFrequencySize)-1]->_count = 1;
		}
	}

	// Add \0
	charFrequencys[(*CharFrequencySize)++] = malloc(sizeof(CharFrequency));
	charFrequencys[(*CharFrequencySize)-1]->_value = '\0';
	charFrequencys[(*CharFrequencySize)-1]->_count = 0;

	// Sort the list
	qsort((void*)charFrequencys, *CharFrequencySize, sizeof(CharFrequency*), comp);
	
	// Print the frequency list
	#if PRINT_CHAR_COUNTS
	for(i = 0; i < *CharFrequencySize; i++)
		printf("Char:\'%c\' has frequency:%d\n", charFrequencys[i]->_value, charFrequencys[i]->_count);
	#endif
}

/**
 * Create the header for the compressed string
 *
 *
 * @param outputBuffer the output buffer
 * @param outputbufferSize the output buffer size
 * @param CharFrequencys the sets of char-count combinations
 * @param CharFrequency_c the size of the CharFrequencys array
 */
void creater_header(char * outputBuffer, int * outputbufferSize, CharFrequency ** charFrequencys, int CharFrequency_c) {
	int i;

	// Add the header size in items
	outputBuffer[(*outputbufferSize)++] = (char)CharFrequency_c;

	// Set the amount of bytes used to store the lenght
	char bitsNeeded;
	int len = charFrequencys[CharFrequency_c-1]->_pathLen;
	if(len == 0)         bitsNeeded = 0;
	else if (len <= 1)   bitsNeeded = 1;
	else if (len <= 3)   bitsNeeded = 2;
	else if (len <= 7)   bitsNeeded = 3;
	else if (len <= 15)   bitsNeeded = 4;
	outputBuffer[(*outputbufferSize)++] = bitsNeeded;

	// Add the charachers
	for(i = 0; i < CharFrequency_c; i++)
		outputBuffer[(*outputbufferSize)++] = charFrequencys[i]->_value;
}

/**
 * Create the content of the compressed string
 *
 * @param input input string
 * @param charFrequencys char frequency's array
 * @param CharFrequencysSize char frequency's array size
 * @param outputBuffer output buffer
 * @param outputbufferSize output buffer size
 */
void create_content(const char * input, CharFrequency ** charFrequencys, int CharFrequencysSize, char * outputBuffer, int * outputbufferSize) {
	int i, j, path = -1, pathTmp, pathLen = -1, pathLenTmp;
	char ch;
	int bPointer = 0;

	for(i = 0; i == 0 || input[i-1] != '\0'; i++) {
		char ch = input[i];

		// Find the id
		for(j = 0; j < CharFrequencysSize; j++) {
			if (charFrequencys[j]->_value == ch) {
				path = charFrequencys[j]->_path;
				pathLen = charFrequencys[j]->_pathLen;
				break;
			}
		}

		// Write the the path len
		j = outputBuffer[1];
		bPointer += outputBuffer[1];
		pathLenTmp = pathLen;
		while(j-- > 0) {
			write_bit(outputBuffer, *outputbufferSize, --bPointer, pathLenTmp % 2);
			pathLenTmp /= 2;
		}
		bPointer += outputBuffer[1];

		// Write the path
		pathTmp = path;
		while(pathLen-- > 0) {
			write_bit(outputBuffer, *outputbufferSize, bPointer++, pathTmp % 2);
			pathTmp /= 2;
		}
	}

	*outputbufferSize += bPointer / 8;
	if ((bPointer % 8) > 0) {
		*outputbufferSize++;
	}
}

/*---- Decompress string functions ---------------------------------------*/

/**
 * Decompress a string
 *
 * @param input input string
 * @param outputBuffer output string buffer
 * @param outputbufferSize output string buffer size
 */
void decompress_string(const char * input, char * outputBuffer, int * outputbufferSize) {
	// Create a buffer for the charachter frequency's
	CharFrequency ** charFrequencys = (CharFrequency**)malloc(sizeof(CharFrequency*) * CHAR_COUNT_BUFFER_SIZE);
	int CharFrequency_c;

	// Read the header
	read_header(input, charFrequencys, (int *)&CharFrequency_c);

	// Read the content
	read_content(input, outputBuffer, outputbufferSize, charFrequencys, CharFrequency_c);

	// Free resources
	free(charFrequencys);
}

/**
 * Read the header of a compressed string
 *
 * header format(in bits):
 * [8] header size
 * [8] amount of bits for tree path length
 *
 * for each node:
 * [8] charachter
 *
 * @param input input string
 * @Param charFrequencys output charachter frequency's
 * @param charFrequencysSize output charachter frequency's size
 */
void read_header(const char * input, CharFrequency ** charFrequencys, int * charFrequencysSize) {
	int i, ch_c = 0;

	// Read the header
	for(i = 0; i < input[0]; i++) {
		char ch = input[i+2];
		charFrequencys[(*charFrequencysSize)++] = malloc(sizeof(CharFrequency));
		charFrequencys[(*charFrequencysSize)-1]->_value = ch;
		set_tree_path_for_id(charFrequencys[(*charFrequencysSize)-1], ch_c++);
	}

	#if PRINT_CHAR_COUNTS
	for(i = 0; i < *charFrequencysSize; i++)
		printf("Char:\'%c\' has path:%d with len:%d\n", charFrequencys[i]->_value, charFrequencys[i]->_path, charFrequencys[i]->_pathLen);
	#endif
}

/**
 * Read the content of a compressed string
 *
 * @param input input string
 * @param output output buffer
 * @param outputSize output buffer size
 * @param charFrequencys charachter frequency's
 * @param CharFrequencysSize charachter frequency's length
 */
void read_content(const char * input, char * output, int * outputSize, CharFrequency ** charFrequencys, int CharFrequencysSize) {
	int i;

	// Calculate the header size
	int headerSize = input[0] + 2;
	int input_p = 0;
	
	while(TRUE) {
		int pathLen = 0;
		int path = 0;

		// Read the tree len
		for(i = 0; i < input[1]; i++) {
			pathLen += read_bit(input, headerSize, input_p++) * ipow(2,  input[1] - 1 - i);
		}

		// Read the tree path
		for(i = 0; i < pathLen; i++) {
			path += read_bit(input, headerSize, input_p++) * ipow(2, i);
		}

		// Look for the path in the char table
		char ch = 'X';
		for(i = 0; i < CharFrequencysSize; i++) {
			if(charFrequencys[i]->_path == path && charFrequencys[i]->_pathLen == pathLen) {
				ch = charFrequencys[i]->_value;
				break;
			}
		}

		// Write the char to the output buffer
		output[(*outputSize)++] = ch;

		//printf("<%d,%d>%c\n", pathLen, path, ch);

		if(ch == '\0') {
			return;
		}
	}
}


/*---- Help functions ---------------------------------------*/

/**
 * Integer power
 * 
 * @return x^p
 */
int ipow(int x, int p) {
	if (p == 0) return 1;
	return (p == 1) ? x:(x * ipow(x, p-1));
}

/**
 * Calculate the tree path for an id
 */
void set_tree_path_for_id(CharFrequency * charFrequency, int id) {
	// Calculate the path length
	charFrequency->_pathLen = -1;
	while(TRUE)
		if (id + 1 <= offset_for_layer(++charFrequency->_pathLen))
			break;
	
	// Calculate the path
	if (charFrequency->_pathLen > 0)
		charFrequency->_path = id - offset_for_layer(charFrequency->_pathLen-1);;
}

/**
 * Calculate how many nodes are on and above a layer
 */
int offset_for_layer(int layer) {
	return (layer == 0) ? 1:(offset_for_layer(layer-1) + ipow(2, layer));
}

/**
 * Compare a CharFrequency to a CharFrequency (used for qsort)
 */
int comp (const void * elem1, const void * elem2) {
    return (*((CharFrequency**)elem2))->_count - (*((CharFrequency**)elem1))->_count;
}

/*---- Bitwise functions ---------------------------------------*/

/**
 * Set one bit in an array of chars.
 *
 * @param output output buffer
 * @param offset offset in the buffer
 * @param bPointer bit index
 * @param value bit value to set
 */
void write_bit(char * output, int offset, int bPointer, char value) {
	if (value)
		output[offset + (bPointer/8)] |= 0x1 << (7 - (bPointer%8));
	else
		output[offset + (bPointer/8)] &= ~(0x1 << (7 - (bPointer%8)));
}

/**
 * Read one bit from a byte array.
 *
 * @param input input char*
 * @param offset offset for char*
 * @param bPointer bit index
 * @return value of the bit
 */
char read_bit(const char * input, int offset, int bPointer) {
	return (input[offset + (bPointer/8)] & (0x1 << 7 - (bPointer % 8))) != 0;
}
