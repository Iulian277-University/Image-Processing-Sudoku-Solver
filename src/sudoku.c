#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "bmp_header.h"
#include "cJSON.h"

#define SUDOKU_ROWS 9
#define SUDOKU_COLS 9

unsigned short ReadLE2(FILE *fp);
unsigned int ReadLE4(FILE *fp);
bmp_fileheader *ReadBMFileHeader(FILE *fp);
bmp_infoheader *ReadBMInfoHeader(FILE *fp);
void readJsonInput(bmp_fileheader *bmpFileHeader, bmp_infoheader *bmpInfoHeader, char input_json_file_name[25], int *pixel_array, int *len, int buffer_size);
void allocateMatrix(int ***matrix, int height, int width);
void rotateMatrix(int **matrix, bmp_infoheader *bmpInfoHeader);
void matchRGB(int **red, int **green, int **blue, unsigned char *img, int *len, int *pixel_array, bmp_fileheader *bmpFileHeader, bmp_infoheader *bmpInfoHeader);
void computeDataString(int **red, int **green, int **blue, unsigned char *img, bmp_infoheader *bmpInfoHeader);
void writeFileHeader(bmp_fileheader *bmpFileHeader, FILE *output_file);
void writeInfoHeader(bmp_infoheader *bmpInfoHeader, FILE *output_file);
void writeTheImage(bmp_fileheader *bmpFileHeader, bmp_infoheader *bmpInfoHeader, unsigned char *img, char output_file_name[24]);
void swap(int *a, int *b);
int checkSudoku(int **sudoku_digits);
int sameRow(int **sudoku, int pos_x, int pos_y, int num);
int sameColumn(int **sudoku, int pos_x, int pos_y, int num);
int sameSquare(int **sudoku, int pos_x, int pos_y, int num);
int solveSudoku(int **sudoku, int start_x, int start_y);
void writeJsonOutput(int **sudoku_digits, char output_file_name[24], char output_file_index[3]);
void countPinkPixels(int **sudoku_digits, int **green_reindexed, bmp_infoheader *bmpInfoHeader);
void computeMask1(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask2(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask3(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask4(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask5(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask6(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask7(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask8(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMask9(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void computeMaskX(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8]);
void drawDigit(int **red_reindexed, int **green_reindexed, int **blue_reindexed, int which_row, int which_col, int mask_red[8][8], int mask_green[8][8], int mask_blue[8][8]);
void task1(int **red, int **green, int **blue, unsigned char *img, int *pixel_array, int *len, bmp_infoheader *bmpInfoHeader, 
            bmp_fileheader *bmpFileHeader, char output_file_index[3], char *input_file_name, int count_total_chars_json);
void task2(int **red, int **green, int **blue, unsigned char *img, int *pixel_array, int *len,
            bmp_infoheader *bmpInfoHeader, bmp_fileheader *bmpFileHeader, char output_file_index[3]);
void task3(int **sudoku_digits, int **red, int **green, int **blue, unsigned char *img,
            bmp_infoheader *bmpInfoHeader, bmp_fileheader *bmpFileHeader, char output_file_index[3]);
void task4_bonus(int **sudoku_digits, char input_file_name_task5[34], char output_file_index[3], char _4bonus[2]);


int main(int argc, char *argv[])
{
    bmp_fileheader *bmpFileHeader = (bmp_fileheader *) calloc(1, sizeof(bmp_fileheader));
    bmp_infoheader *bmpInfoHeader = (bmp_infoheader *) calloc(1, sizeof(bmp_infoheader));

    // Get the total characters from input a json input file (for allocating an aproximative memory)
    FILE *json_input_file = fopen("input/task1/board01.json", "r");

    int count_total_chars_json = 0;
    for(char ch=getc(json_input_file); ch!=EOF; ch=getc(json_input_file))
        count_total_chars_json++;

    fclose(json_input_file);

    // Allocate the pixels data
    int len = 0;
    int *pixel_array = (int *) calloc(count_total_chars_json, sizeof(int));

    int **sudoku_digits; // 9 x 9 matrix
    allocateMatrix(&sudoku_digits, SUDOKU_ROWS, SUDOKU_COLS);

    int height = 73, width = 73;

    // Allocate RGB matrices
    int **red = NULL, **green = NULL, **blue = NULL;
    allocateMatrix(&red, height, width);
    allocateMatrix(&green, height, width);
    allocateMatrix(&blue, height, width);

    // Allocate the string to be written in the bmp output
    unsigned char *img = (unsigned char *) malloc(3 * height * width * sizeof(unsigned char));
    memset(img, 0, 3 * height * width);

    // Extract the number from input file name
    char output_file_index[3];
    for(int i=0; i<strlen(argv[1])-1; ++i)
    {
        if(isdigit(argv[1][i]) && isdigit(argv[1][i+1])) // 2 consecutive digits
        {
            output_file_index[0] = argv[1][i];
            output_file_index[1] = argv[1][i+1];
            break;
        }
    }
    output_file_index[2] = '\0';

    // Task 123
    if(strcmp(argv[2], "123") == 0)
    {
        char input_file_name[35] = "input/task1/board";
        input_file_name[17] = output_file_index[0];
        input_file_name[18] = output_file_index[1];
        strcat(input_file_name, ".json");

        task1(red, green, blue, img, pixel_array, &len, bmpInfoHeader, bmpFileHeader, output_file_index, input_file_name, count_total_chars_json);
        task2(red, green, blue, img, pixel_array, &len, bmpInfoHeader, bmpFileHeader, output_file_index);
        task3(sudoku_digits, red, green, blue, img, bmpInfoHeader, bmpFileHeader, output_file_index);
    }

    // Task 4
    if(strcmp(argv[2], "4") == 0)
        task4_bonus(sudoku_digits, argv[1], output_file_index, "4");

    // Task 5 (Bonus)
    if(strcmp(argv[2], "bonus") == 0)
        task4_bonus(sudoku_digits, argv[1], output_file_index, "5");


    // Free memory
    free(img);
    free(pixel_array);
    
    for(int row=0; row<height; ++row)
        free(red[row]), free(green[row]), free(blue[row]);
    free(red), free(green), free(blue);

    for(int i=0; i<SUDOKU_ROWS; ++i)
        free(sudoku_digits[i]);
    free(sudoku_digits);

    free(bmpFileHeader);
    free(bmpInfoHeader);

    return 0;
}

// Read 2 bytes in little endian
unsigned short ReadLE2(FILE *fp)
{
    unsigned char buffer[2];
    unsigned short result = 0;

    fread(buffer, 1, 2, fp);
    for (int i=1; i>=0; --i)
        result = (result << 8) | (unsigned short) buffer[i];

    return result;
}

// Read 4 bytes in little endian
unsigned int ReadLE4(FILE *fp)
{
    unsigned char buffer[4];
    unsigned int result = 0;

    fread(buffer, 1, 4, fp);
    for (int i=3; i>=0; --i)
        result = (result << 8) | (unsigned int) buffer[i];

    return result;
}

// Read file header from (bmp) file
bmp_fileheader *ReadBMFileHeader(FILE *fp)
{
    bmp_fileheader *fileHeader;
    char filetype[3] = {'\0', '\0', '\0'};
    unsigned int filesize;
    unsigned short reserved1;
    unsigned short reserved2;
    unsigned long offset;

    // File type (2 bytes)
    fread(&filetype, 1, 2, fp);

    // File size (4 bytes)
    filesize = (unsigned int) ReadLE4(fp);

    // Reserved 1 (2 bytes)
    fread(&reserved1, 2, 1, fp);

    // Reserved 2 (2 bytes)
    fread(&reserved2, 2, 1, fp);

    // Offset (4 bytes)
    offset = (unsigned long) ReadLE4(fp);

    fileHeader = (bmp_fileheader *) calloc(1, sizeof(bmp_fileheader));
    fileHeader->fileMarker1 = filetype[0];
    fileHeader->fileMarker2 = filetype[1];
    fileHeader->bfSize  = filesize;
    fileHeader->unused1 = reserved1;
    fileHeader->unused2 = reserved2;
    fileHeader->imageDataOffset = offset;

    return fileHeader;
}

// Read info header from (bmp) file
bmp_infoheader *ReadBMInfoHeader(FILE *fp)
{
    bmp_infoheader *infoHeader;
    unsigned int headersize;
    int width;
    int height;
    unsigned short planes;
    unsigned short bitcount;
    unsigned int compression;
    unsigned int size_image;
    int x_pix_per_meter;
    int y_pix_per_meter;
    unsigned int clr_used;
    unsigned int clr_important;

    // Header size (4 bytes)
    headersize = (unsigned int) ReadLE4(fp);

    // Width (4 bytes)
    width = (int) ReadLE4(fp);

    // Height (4 bytes)
    height = (int) ReadLE4(fp);

    // Planes (2 bytes)
    planes = (unsigned short) ReadLE2(fp);

    // Bit Count (2 bytes)
    bitcount = (unsigned short) ReadLE2(fp);

    // Compression (4 bytes)
    compression = (unsigned int) ReadLE4(fp);

    // Size image (4 bytes)
    size_image = (unsigned int) ReadLE4(fp);

    // X pix per meter (4 bytes)
    x_pix_per_meter = (int) ReadLE4(fp);

    // Y pix per meter (4 bytes)
    y_pix_per_meter = (int) ReadLE4(fp);

    // Color used (4 bytes)
    clr_used = (unsigned int) ReadLE4(fp);

    // Color important (4 bytes)
    clr_important = (unsigned int) ReadLE4(fp);

    infoHeader = (bmp_infoheader *) calloc(1, sizeof(bmp_infoheader));
    infoHeader->biSize = headersize;
    infoHeader->width = width;
    infoHeader->height = height;
    infoHeader->planes = planes;
    infoHeader->bitPix = bitcount;
    infoHeader->biCompression = compression;
    infoHeader->biSizeImage = size_image;
    infoHeader->biXPelsPerMeter = x_pix_per_meter;
    infoHeader->biYPelsPerMeter = y_pix_per_meter;
    infoHeader->biClrUsed = clr_used;
    infoHeader->biClrImportant = clr_important;

    return infoHeader;
}

// Parse the json input
void readJsonInput(bmp_fileheader *bmpFileHeader, bmp_infoheader *bmpInfoHeader, char input_json_file_name[25],
                    int *pixel_array, int *len, int buffer_size)
{
    FILE *input_json_file = NULL;
    input_json_file = fopen(input_json_file_name, "r");

    if(input_json_file == NULL)
        fclose(input_json_file), exit(404);

    char *buffer = (char *) calloc(buffer_size, sizeof(char) + 1); // For null terminator
    fread(buffer, buffer_size, 1, input_json_file);

    // Read the whole JSON file
    cJSON *root = cJSON_Parse(buffer);
    if(root == NULL)
        exit(404);

    // Read file header
    cJSON *file_header = cJSON_GetObjectItem(root, "file_header"); // Read the file_header field from JSON file
    if(file_header == NULL)
        exit(404);

    cJSON *signature = cJSON_GetObjectItem(file_header, "signature");
    char sign[3];
    strcpy(sign, signature->valuestring);
    bmpFileHeader->fileMarker1 = (unsigned char) sign[0];
    bmpFileHeader->fileMarker2 = (unsigned char) sign[1];

    cJSON *file_size = cJSON_GetObjectItem(file_header, "file_size");
    bmpFileHeader->bfSize = (unsigned int) file_size->valueint;

    bmpFileHeader->unused1 = (unsigned short) 0;
    bmpFileHeader->unused2 = (unsigned short) 0;
    
    cJSON *offset = cJSON_GetObjectItem(file_header, "offset");
    bmpFileHeader->imageDataOffset = (unsigned int) offset->valueint;

    // Read info_header
    cJSON *info_header = cJSON_GetObjectItem(root, "info_header"); // Read the file_header field from JSON file
    if(info_header == NULL)
        exit(404);

    cJSON *biSize = cJSON_GetObjectItem(info_header, "size");
    bmpInfoHeader->biSize = (unsigned int) biSize->valueint;
    
    cJSON *width = cJSON_GetObjectItem(info_header, "width");
    bmpInfoHeader->width = (signed int) width->valueint;

    cJSON *height = cJSON_GetObjectItem(info_header, "height");
    bmpInfoHeader->height = (signed int) height->valueint;
        
    cJSON *planes = cJSON_GetObjectItem(info_header, "planes");
    bmpInfoHeader->planes = (unsigned short) planes->valueint;

    cJSON *bitPix = cJSON_GetObjectItem(info_header, "bit_count");
    bmpInfoHeader->bitPix = (unsigned short) bitPix->valueint;

    cJSON *biCompression = cJSON_GetObjectItem(info_header, "compression");
    bmpInfoHeader->biCompression = (unsigned int) biCompression->valueint;

    cJSON *biSizeImage = cJSON_GetObjectItem(info_header, "image_size");
    bmpInfoHeader->biSizeImage = (unsigned int) biSizeImage->valueint;

    cJSON *biXPelsPerMeter = cJSON_GetObjectItem(info_header, "x_pixels_per_meter");
    bmpInfoHeader->biXPelsPerMeter = (int) biXPelsPerMeter->valueint;

    cJSON *biYPelsPerMeter = cJSON_GetObjectItem(info_header, "y_pixels_per_meter");
    bmpInfoHeader->biYPelsPerMeter = (int) biYPelsPerMeter->valueint;

    cJSON *biClrUsed = cJSON_GetObjectItem(info_header, "colors_used");
    bmpInfoHeader->biClrUsed = (unsigned int) biClrUsed->valueint;

    cJSON *biClrImportant = cJSON_GetObjectItem(info_header, "colors_important");
    bmpInfoHeader->biClrImportant = (unsigned int) biClrImportant->valueint;
    
    // Read bitmap array (pixels data)
    cJSON *iterator = NULL;
    cJSON *bitmap = cJSON_GetObjectItem(root, "bitmap");

    if(!cJSON_IsArray(bitmap))
        exit(404);

    *len = 0;
    cJSON_ArrayForEach(iterator, bitmap)
        pixel_array[(*len)++] = iterator->valueint;

    free(buffer);
    cJSON_Delete(root);
    fclose(input_json_file);
}

// Allocate matrix and check validity
void allocateMatrix(int ***matrix, int height, int width)
{
    // Allocate the matrix and check validity of allocation
    *matrix = (int **) calloc(height, sizeof(int *));
    if(matrix == NULL)
        exit(404);
    for(int i=0; i<height; ++i)
    {
        (*matrix)[i] = (int *) calloc(width, sizeof(int));
        if((*matrix)[i] == NULL)
            exit(404);
    }
}

// Rotate matrix 90 degrees clockwise
void rotateMatrix(int **matrix, bmp_infoheader *bmpInfoHeader)
{
    // Rotate 90 degrees
    for (int row=0; row<bmpInfoHeader->height/2; ++row)
	{
        for (int col=row; col<bmpInfoHeader->width-row-1; ++col)
		{
            // Swap elements of each cycle in clockwise direction
            int temp = matrix[row][col];
            matrix[row][col] = matrix[bmpInfoHeader->height - 1 - col][row];
            matrix[bmpInfoHeader->height - 1 - col][row] = matrix[bmpInfoHeader->height - 1 - row][bmpInfoHeader->height - 1 - col];
            matrix[bmpInfoHeader->height - 1 - row][bmpInfoHeader->height - 1 - col] = matrix[col][bmpInfoHeader->height - 1 - row];
            matrix[col][bmpInfoHeader->height - 1 - row] = temp;
        }
    }
}

// Transpose the pixels data into 3 matrices (R, G, B)
void matchRGB(int **red, int **green, int **blue, unsigned char *img, int *len, int *pixel_array, 
                bmp_fileheader *bmpFileHeader, bmp_infoheader *bmpInfoHeader)
{
    // Match for each color, the proper color index
    int row = 0, col = 0;
    for(int i=0; i<*len; )
    {
        blue[row][col] = pixel_array[i++];
        green[row][col] = pixel_array[i++];
        red[row][col] = pixel_array[i++];
        col++;
        if(col == bmpInfoHeader->width)
            row++, col = 0;
    }
    
    // Rotate 90 degrees
    rotateMatrix(red, bmpInfoHeader);
    rotateMatrix(green, bmpInfoHeader);
    rotateMatrix(blue, bmpInfoHeader);
}

// Compute the final string for writting
void computeDataString(int **red, int **green, int **blue, unsigned char *img, bmp_infoheader *bmpInfoHeader)
{
    // Compute the string which will be written in the bmp output
    for(int i=0; i<bmpInfoHeader->height; ++i)
    {
        for(int j=0; j<bmpInfoHeader->width; ++j)
        {
            int x = i, y = (bmpInfoHeader->height - 1) - j;
            int r = red[i][j], g = green[i][j], b = blue[i][j];
            
            img[(x+y*bmpInfoHeader->width)*3 + 2] = (unsigned char) (r);
            img[(x+y*bmpInfoHeader->width)*3 + 1] = (unsigned char) (g);
            img[(x+y*bmpInfoHeader->width)*3 + 0] = (unsigned char) (b);
        }
    }
}

// Write file header to output file
void writeFileHeader(bmp_fileheader *bmpFileHeader, FILE *output_file)
{
    // Write file header
    fwrite(&bmpFileHeader->fileMarker1, 1, 1, output_file);
    fwrite(&bmpFileHeader->fileMarker2, 1, 1, output_file);

    fwrite(&bmpFileHeader->bfSize, 4, 1, output_file);

    fwrite(&bmpFileHeader->unused1, 2, 1, output_file);
    fwrite(&bmpFileHeader->unused2, 2, 1, output_file);

    fwrite(&bmpFileHeader->imageDataOffset, 4, 1, output_file);
}

// Write info header to output file
void writeInfoHeader(bmp_infoheader *bmpInfoHeader, FILE *output_file)
{
    // Write info header
    fwrite(&bmpInfoHeader->biSize, 4, 1, output_file);

    fwrite(&bmpInfoHeader->width, 4, 1, output_file);
    fwrite(&bmpInfoHeader->height, 4, 1, output_file);

    fwrite(&bmpInfoHeader->planes, 2, 1, output_file);
    fwrite(&bmpInfoHeader->bitPix, 2, 1, output_file);
    
    // Write compression
    fwrite(&bmpInfoHeader->biCompression, 4, 1, output_file);
    fwrite(&bmpInfoHeader->biSizeImage, 4, 1, output_file);

    fwrite(&bmpInfoHeader->biXPelsPerMeter, 4, 1, output_file);
    fwrite(&bmpInfoHeader->biYPelsPerMeter, 4, 1, output_file);

    fwrite(&bmpInfoHeader->biClrUsed, 4, 1, output_file);
    fwrite(&bmpInfoHeader->biClrImportant, 4, 1, output_file);
}

// Write bmp image to output file
void writeTheImage(bmp_fileheader *bmpFileHeader, bmp_infoheader *bmpInfoHeader, unsigned char *img, char output_file_name[24])
{
    FILE *output_file;
    output_file = fopen(output_file_name, "wb");

    // Write File Header
    writeFileHeader(bmpFileHeader, output_file);
    
    // Write Info Header
    writeInfoHeader(bmpInfoHeader, output_file);

    // Write Pixel Data
    unsigned char bmppad[3] = {0, 0, 0};
    for(int i=0; i<bmpInfoHeader->height; ++i)
    {
        fwrite(img + (bmpInfoHeader->width * (bmpInfoHeader->height-i-1)*3), 3, bmpInfoHeader->width, output_file); // Pixels
        fwrite(bmppad, 1, (4-(bmpInfoHeader->width*3)%4)%4, output_file); // Padding for each row
    }

    fclose(output_file);
}

// Swap 2 pointers
void swap(int *a, int *b)
{
    int temp = *a;
    *a = *b;
    *b = temp;
}

// Check if a sudoku game is valid
int checkSudoku(int **sudoku_digits)
{
    // Check the sum on each row
    for(int row=0; row<SUDOKU_ROWS; ++row)
    {
        int row_sum = 0;
        for(int col=0; col<SUDOKU_COLS; ++col)
            row_sum += sudoku_digits[row][col];
        if(row_sum != 45)
            return 0; // Isn't valid
    }

    // Check the sum on each column
    for(int col=0; col<SUDOKU_COLS; ++col)
    {
        int col_sum = 0;
        for(int row=0; row<SUDOKU_ROWS; ++row)
            col_sum += sudoku_digits[row][col];
        if(col_sum != 45)
            return 0; // Isn't valid 
    }

    // Check the sum on each box (square)
    int curr_row_idx = 0;
    int curr_col_idx = 0;
    for(int square_rows=0; square_rows<9; ++square_rows) // 3 squares per row
    {
        int square_sum = 0;
        for(int row=curr_row_idx; row<curr_row_idx + 3; ++row)
            for(int col=curr_col_idx; col<curr_col_idx + 3; ++col)
                square_sum += sudoku_digits[row][col];

        if(square_sum != 45)
            return 0; // Isn't valid 

        curr_col_idx += 3;
        if(curr_col_idx == SUDOKU_COLS)
            curr_col_idx = 0, curr_row_idx += 3;
    }

    return 1; // Is valid
}

// Check if a digit is already on a specific row
int sameRow(int **sudoku, int pos_x, int pos_y, int num)
{
    for(int row=0; row<9; ++row)
        if(sudoku[row][pos_y] == num)
            return 1;

    return 0;
}

// Check if a digit is already on a specific column
int sameColumn(int **sudoku, int pos_x, int pos_y, int num)
{
    for(int col=0; col<9; ++col)
        if(sudoku[pos_x][col] == num)
            return 1;
        
    return 0;
}

// Check if a digit is already on a specific square
int sameSquare(int **sudoku, int pos_x, int pos_y, int num)
{
    // Set pos_x value
    if(pos_x < 3)
        pos_x = 0;
    else if(pos_x < 6)
        pos_x = 3;
    else
        pos_x = 6;

    // Set pos_y value
    if(pos_y < 3)
        pos_y = 0;
    else if(pos_y < 6)
        pos_y = 3;
    else
        pos_y = 6;

    // Now, we are searching in a specific square
    for(int row=pos_x; row<pos_x+3; ++row)
    {
        for(int col=pos_y; col<pos_y+3; ++col)
        {
            if(sudoku[row][col] == num)
                return 1;
        }
    }

    return 0;
}

// Solve a sudoku given as input (Backtracking)
int solveSudoku(int **sudoku, int start_x, int start_y) // start_x, start_y: starting positions for searching in the sudoku(_digits) matrix
{
    int num = 1; // Start with the number one and try to put in on the table
    int start_tx = 0; // temp_start_x
    int start_ty = 0; // temp_start_y

    if(sudoku[start_x][start_y] != 0)
    {
        if(start_x == 8 && start_y == 8)
            return 1;

        if(start_x < 8)
            start_x++;
        else if(start_y < 8)
        {
            start_x = 0;
            start_y++;
        }

        if(solveSudoku(sudoku, start_x, start_y))
            return 1;
        else
            return 0;
    }

    if(sudoku[start_x][start_y] == 0)
    {
        while(num < 10)
        {
            if(!sameRow(sudoku, start_x, start_y, num) && !sameColumn(sudoku, start_x, start_y, num) && !sameSquare(sudoku, start_x, start_y, num))
            {
                sudoku[start_x][start_y] = num;

                if(start_x == 8 && start_y == 8)
                    return 1;

                if(start_x < 8)
                    start_tx = start_x + 1;
                else if(start_y < 8)
                {
                    start_tx = 0;
                    start_ty = start_y + 1;
                }

                if(solveSudoku(sudoku, start_tx, start_ty))
                    return 1;
            }
            num++;
        }
        sudoku[start_x][start_y] = 0;
        return 0; // We've tried every single number
    }

    return 0;
}

// Write data to json file
void writeJsonOutput(int **sudoku_digits, char output_file_name[24], char output_file_index[3])
{
    FILE *output_json_file;
    output_json_file = fopen(output_file_name, "w");

    // Create the main object
    cJSON *root = cJSON_CreateObject();

    // Add the content to the json file
    char board_number[12] = "board";
    board_number[5] = output_file_index[0];
    board_number[6] = output_file_index[1];
    strcat(board_number, ".json");
    cJSON *input_file = cJSON_CreateString(board_number);
    cJSON_AddItemToObject(root, "input_file", input_file);

    cJSON *win_state = cJSON_CreateString("Win!");
    cJSON *loss_state = cJSON_CreateString("Loss :(");

    if(checkSudoku(sudoku_digits))
        cJSON_AddItemToObject(root, "game_state", win_state), cJSON_Delete(loss_state);
    else
        cJSON_AddItemToObject(root, "game_state", loss_state), cJSON_Delete(win_state);
    
    char *output_string;
    output_string = cJSON_Print(root);
    fprintf(output_json_file, "%s\n", output_string);

    free(output_string);
    cJSON_Delete(root);
}

// Count pink pixels (digit color pixels)
void countPinkPixels(int **sudoku_digits, int **green_reindexed, bmp_infoheader *bmpInfoHeader)
{
    int curr_row_idx = 0, curr_col_idx = 0;
    int pink_pixels_counter = 0;

    // Count for pink pixels and based on the number of pixels
    // from each square and compute the sudoku_digits matrix
    int i = 0, j = 0;
    for(int square=0; square<81; ++square)
    {
        int could_be_2 = 0, could_be_5 = 0, could_be_6 = 0;
        for(int row=curr_row_idx; row<curr_row_idx + 7; ++row)
        {
            for(int col=curr_col_idx; col<curr_col_idx + 7; ++col)
            {
                if(green_reindexed[row][col] == 175)
                {
                    pink_pixels_counter++;
                    if((row == curr_row_idx + 5) && (col == curr_col_idx + 2))
                        could_be_2 = 1, could_be_6 = 1;
                    if((row == curr_row_idx + 3) && (col == curr_col_idx + 2))
                        could_be_5 = 1;
                }
            }
        }
        if(pink_pixels_counter == 5)
            sudoku_digits[i][j++] = 1;
        else if(pink_pixels_counter == 9)
            sudoku_digits[i][j++] = 7;
        else if(pink_pixels_counter == 11)
            sudoku_digits[i][j++] = 4;
        else if(pink_pixels_counter == 19)
            sudoku_digits[i][j++] = 8;
        else if(pink_pixels_counter == 18)
        {
            if(could_be_6 == 1)
                sudoku_digits[i][j++] = 6;
            else
                sudoku_digits[i][j++] = 9;
        }
        else if(pink_pixels_counter == 17)
        {
            if(could_be_2 == 1)
                sudoku_digits[i][j++] = 2;
            else if(could_be_5 == 1)
                sudoku_digits[i][j++] = 5;
            else
                sudoku_digits[i][j++] = 3;
        }
        else
            sudoku_digits[i][j++] = 0; // Empty square

        if(j == SUDOKU_COLS)
            i++, j = 0;

        pink_pixels_counter = 0;
        if(curr_col_idx + 8 <= bmpInfoHeader->width - 1)
            curr_col_idx += 8;
        else
            curr_col_idx += 7;

        if(curr_col_idx == bmpInfoHeader->width - 1)
            curr_col_idx = 0, curr_row_idx += 8;
    }
}

// Masks for each digit (1 - 9) and 'X' symbol
void computeMask1(int mask_1_red[8][8], int mask_1_green[8][8], int mask_1_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_1_red[i][j] = 255;
            mask_1_green[i][j] = 255;
            mask_1_blue[i][j] = 255;
        }
    }

    mask_1_green[2][6] = 0;
    mask_1_green[3][6] = 0;
    mask_1_green[4][6] = 0;
    mask_1_green[5][6] = 0;
    mask_1_green[6][6] = 0;

}

void computeMask2(int mask_2_red[8][8], int mask_2_green[8][8], int mask_2_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_2_red[i][j] = 255;
            mask_2_green[i][j] = 255;
            mask_2_blue[i][j] = 255;
        }
    }
    mask_2_green[2][2] = 0;
    mask_2_green[2][3] = 0;
    mask_2_green[2][4] = 0;
    mask_2_green[2][5] = 0;
    mask_2_green[2][6] = 0;

    mask_2_green[3][6] = 0;

    mask_2_green[4][2] = 0;
    mask_2_green[4][3] = 0;
    mask_2_green[4][4] = 0;
    mask_2_green[4][5] = 0;
    mask_2_green[4][6] = 0;

    mask_2_green[5][2] = 0;

    mask_2_green[6][2] = 0;
    mask_2_green[6][3] = 0;
    mask_2_green[6][4] = 0;
    mask_2_green[6][5] = 0;
    mask_2_green[6][6] = 0;
}

void computeMask3(int mask_3_red[8][8], int mask_3_green[8][8], int mask_3_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_3_red[i][j] = 255;
            mask_3_green[i][j] = 255;
            mask_3_blue[i][j] = 255;
        }
    }
    mask_3_green[2][2] = 0;
    mask_3_green[2][3] = 0;
    mask_3_green[2][4] = 0;
    mask_3_green[2][5] = 0;
    mask_3_green[2][6] = 0;

    mask_3_green[3][6] = 0;

    mask_3_green[4][2] = 0;
    mask_3_green[4][3] = 0;
    mask_3_green[4][4] = 0;
    mask_3_green[4][5] = 0;
    mask_3_green[4][6] = 0;

    mask_3_green[5][6] = 0;

    mask_3_green[6][2] = 0;
    mask_3_green[6][3] = 0;
    mask_3_green[6][4] = 0;
    mask_3_green[6][5] = 0;
    mask_3_green[6][6] = 0;
}

void computeMask4(int mask_4_red[8][8], int mask_4_green[8][8], int mask_4_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_4_red[i][j] = 255;
            mask_4_green[i][j] = 255;
            mask_4_blue[i][j] = 255;
        }
    }
    mask_4_green[2][2] = 0;
    mask_4_green[2][6] = 0;

    mask_4_green[3][2] = 0;
    mask_4_green[3][6] = 0;

    mask_4_green[4][2] = 0;
    mask_4_green[4][3] = 0;
    mask_4_green[4][4] = 0;
    mask_4_green[4][5] = 0;
    mask_4_green[4][6] = 0;

    mask_4_green[5][6] = 0;

    mask_4_green[6][6] = 0;
}

void computeMask5(int mask_5_red[8][8], int mask_5_green[8][8], int mask_5_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_5_red[i][j] = 255;
            mask_5_green[i][j] = 255;
            mask_5_blue[i][j] = 255;
        }
    }

    mask_5_green[2][2] = 0;
    mask_5_green[2][3] = 0;
    mask_5_green[2][4] = 0;
    mask_5_green[2][5] = 0;
    mask_5_green[2][6] = 0;

    mask_5_green[3][2] = 0;

    mask_5_green[4][2] = 0;
    mask_5_green[4][3] = 0;
    mask_5_green[4][4] = 0;
    mask_5_green[4][5] = 0;
    mask_5_green[4][6] = 0;

    mask_5_green[5][6] = 0;

    mask_5_green[6][2] = 0;
    mask_5_green[6][3] = 0;
    mask_5_green[6][4] = 0;
    mask_5_green[6][5] = 0;
    mask_5_green[6][6] = 0;
}

void computeMask6(int mask_6_red[8][8], int mask_6_green[8][8], int mask_6_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_6_red[i][j] = 255;
            mask_6_green[i][j] = 255;
            mask_6_blue[i][j] = 255;
        }
    }

    mask_6_green[2][2] = 0;
    mask_6_green[2][3] = 0;
    mask_6_green[2][4] = 0;
    mask_6_green[2][5] = 0;
    mask_6_green[2][6] = 0;

    mask_6_green[3][2] = 0;

    mask_6_green[4][2] = 0;
    mask_6_green[4][3] = 0;
    mask_6_green[4][4] = 0;
    mask_6_green[4][5] = 0;
    mask_6_green[4][6] = 0;

    mask_6_green[5][2] = 0;
    mask_6_green[5][6] = 0;

    mask_6_green[6][2] = 0;
    mask_6_green[6][3] = 0;
    mask_6_green[6][4] = 0;
    mask_6_green[6][5] = 0;
    mask_6_green[6][6] = 0;
}

void computeMask7(int mask_7_red[8][8], int mask_7_green[8][8], int mask_7_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_7_red[i][j] = 255;
            mask_7_green[i][j] = 255;
            mask_7_blue[i][j] = 255;
        }
    }
    mask_7_green[2][2] = 0;
    mask_7_green[2][3] = 0;
    mask_7_green[2][4] = 0;
    mask_7_green[2][5] = 0;
    mask_7_green[2][6] = 0;

    mask_7_green[2][6] = 0;
    mask_7_green[3][6] = 0;
    mask_7_green[4][6] = 0;
    mask_7_green[5][6] = 0;
    mask_7_green[6][6] = 0;
}

void computeMask8(int mask_8_red[8][8], int mask_8_green[8][8], int mask_8_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_8_red[i][j] = 255;
            mask_8_green[i][j] = 255;
            mask_8_blue[i][j] = 255;
        }
    }

    mask_8_green[2][2] = 0;
    mask_8_green[2][3] = 0;
    mask_8_green[2][4] = 0;
    mask_8_green[2][5] = 0;
    mask_8_green[2][6] = 0;

    mask_8_green[3][2] = 0;
    mask_8_green[3][6] = 0;

    mask_8_green[4][2] = 0;
    mask_8_green[4][3] = 0;
    mask_8_green[4][4] = 0;
    mask_8_green[4][5] = 0;
    mask_8_green[4][6] = 0;

    mask_8_green[5][2] = 0;
    mask_8_green[5][6] = 0;

    mask_8_green[6][2] = 0;
    mask_8_green[6][3] = 0;
    mask_8_green[6][4] = 0;
    mask_8_green[6][5] = 0;
    mask_8_green[6][6] = 0;
}

void computeMask9(int mask_9_red[8][8], int mask_9_green[8][8], int mask_9_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_9_red[i][j] = 255;
            mask_9_green[i][j] = 255;
            mask_9_blue[i][j] = 255;
        }
    }

    mask_9_green[2][2] = 0;
    mask_9_green[2][3] = 0;
    mask_9_green[2][4] = 0;
    mask_9_green[2][5] = 0;
    mask_9_green[2][6] = 0;

    mask_9_green[3][2] = 0;
    mask_9_green[3][6] = 0;

    mask_9_green[4][2] = 0;
    mask_9_green[4][3] = 0;
    mask_9_green[4][4] = 0;
    mask_9_green[4][5] = 0;
    mask_9_green[4][6] = 0;

    mask_9_green[5][6] = 0;

    mask_9_green[6][2] = 0;
    mask_9_green[6][3] = 0;
    mask_9_green[6][4] = 0;
    mask_9_green[6][5] = 0;
    mask_9_green[6][6] = 0;
}

void computeMaskX(int mask_X_red[8][8], int mask_X_green[8][8], int mask_X_blue[8][8])
{
    for(int i=1; i<=7; ++i)
    {
        for(int j=1; j<=7; ++j)
        {
            mask_X_red[i][j] = 255;
            mask_X_green[i][j] = 255;
            mask_X_blue[i][j] = 255;
        }
    }

    mask_X_green[2][2] = 0;
    mask_X_blue[2][2] = 0;
    mask_X_green[2][6] = 0;
    mask_X_blue[2][6] = 0;

    mask_X_green[3][3] = 0;
    mask_X_blue[3][3] = 0;
    mask_X_green[3][5] = 0;
    mask_X_blue[3][5] = 0;

    mask_X_green[4][4] = 0;
    mask_X_blue[4][4] = 0;

    mask_X_green[5][3] = 0;
    mask_X_blue[5][3] = 0;
    mask_X_green[5][5] = 0;
    mask_X_blue[5][5] = 0;

    mask_X_green[6][2] = 0;
    mask_X_blue[6][2] = 0;
    mask_X_green[6][6] = 0;
    mask_X_blue[6][6] = 0;
}

void drawDigit(int **red_reindexed, int **green_reindexed, int **blue_reindexed, int which_row, int which_col, 
                int mask_red[8][8], int mask_green[8][8], int mask_blue[8][8])
{
    int i_start, j_start;
    i_start = 1 + 8 * which_row;
    j_start = 1 + 8 * which_col;

    int i_mask = 1, j_mask = 1;
    for(int i=i_start; i<=i_start+6; ++i)
    {
        for(int j=j_start; j<=j_start+6; ++j)
        {
            red_reindexed[i][j] = mask_red[i_mask][j_mask];
            green_reindexed[i][j] = mask_green[i_mask][j_mask];
            blue_reindexed[i][j] = mask_blue[i_mask][j_mask];
            j_mask++;
        }
        i_mask++;
        j_mask = 1;
    }
}

// Preliminay image
void task1(int **red, int **green, int **blue, unsigned char *img, int *pixel_array, int *len, bmp_infoheader *bmpInfoHeader, 
            bmp_fileheader *bmpFileHeader, char output_file_index[3], char *input_file_name, int count_total_chars_json)
{
    // Read from json file input
    readJsonInput(bmpFileHeader, bmpInfoHeader, input_file_name, pixel_array, len, count_total_chars_json);

    // Compute the output file name
    char output_file_name[24] = "output_task1_board";
    output_file_name[18] = output_file_index[0];
    output_file_name[19] = output_file_index[1];
    strcat(output_file_name, ".bmp");

    // Compute RGB matrices and the output data (pixels) string
    matchRGB(red, green, blue, img, len, pixel_array, bmpFileHeader, bmpInfoHeader);
    computeDataString(red, green, blue, img, bmpInfoHeader);
    
    // Write the output file
    writeTheImage(bmpFileHeader, bmpInfoHeader, img, output_file_name);

}

// Mirror the image
void task2(int **red, int **green, int **blue, unsigned char *img, int *pixel_array, int *len,
            bmp_infoheader *bmpInfoHeader, bmp_fileheader *bmpFileHeader, char output_file_index[3])
{
    // Compute the output file name
    char output_file_name[24] = "output_task2_board";
    output_file_name[18] = output_file_index[0];
    output_file_name[19] = output_file_index[1];
    strcat(output_file_name, ".bmp");

    // Compute RGB matrices
    matchRGB(red, green, blue, img, len, pixel_array, bmpFileHeader, bmpInfoHeader);

    // Flip the image horizontally
    for(int row=0; row<bmpInfoHeader->height; ++row)
    {
        int start = 0;
        int end = bmpInfoHeader->width - 1;
        while(start < end)
        {
            swap(&red[start][row], &red[end][row]);
            swap(&green[start][row], &green[end][row]);
            swap(&blue[start][row], &blue[end][row]);

            start++;
            end--;
        }
    }
    
    // Mirror the flipped image
    for(int square=0; square<=9; ++square)
    {
        int cnt1 = 7;
        for(int col=1; col<=7; ++col)
        {
            for(int row=7*square+1; row<=7*(square+1); ++row)
            {
                swap(&red[col][row], &red[bmpInfoHeader->width-cnt1-1][row]);
                swap(&green[col][row], &green[bmpInfoHeader->width-cnt1-1][row]);
                swap(&blue[col][row], &blue[bmpInfoHeader->width-cnt1-1][row]);
            }
            cnt1--;
        }

        int cnt2 = 16;
        for(int col=8; col<=14; ++col)
        {
            for(int row=7*square+1; row<=7*(square+1); ++row)
            {
                swap(&red[col][row], &red[bmpInfoHeader->width-cnt2-1][row]);
                swap(&green[col][row], &green[bmpInfoHeader->width-cnt2-1][row]);
                swap(&blue[col][row], &blue[bmpInfoHeader->width-cnt2-1][row]);
            }
            cnt2--;
        }

        int cnt3 = 25;
        for(int col=15; col<=22; ++col)
        {
            for(int row=7*square+1; row<=7*(square+1); ++row)
            {
                if(red[col][row] != 0 && red[bmpInfoHeader->width-cnt3-1][row] != 0)
                {
                    swap(&red[col][row], &red[bmpInfoHeader->width-cnt3-1][row]);
                    swap(&green[col][row], &green[bmpInfoHeader->width-cnt3-1][row]);
                    swap(&blue[col][row], &blue[bmpInfoHeader->width-cnt3-1][row]);
                }
            }
            cnt3--;
        }

        int cnt4 = 33;
        for(int col=23; col<=30; ++col)
        {
            for(int row=7*square+1; row<=7*(square+1); ++row)
            {
                if(red[col][row] != 0 && red[bmpInfoHeader->width-cnt4-1][row] != 0)
                {
                    swap(&red[col][row], &red[bmpInfoHeader->width-cnt4-1][row]);
                    swap(&green[col][row], &green[bmpInfoHeader->width-cnt4-1][row]);
                    swap(&blue[col][row], &blue[bmpInfoHeader->width-cnt4-1][row]);
                }
            }
            cnt4--;
        }
    }

    // Compute the output data (pixels) string
    computeDataString(red, green, blue, img, bmpInfoHeader);

    // Write the output file
    writeTheImage(bmpFileHeader, bmpInfoHeader, img, output_file_name);

}

// Check sudoku validity
void task3(int **sudoku_digits, int **red, int **green, int **blue, unsigned char *img,
            bmp_infoheader *bmpInfoHeader, bmp_fileheader *bmpFileHeader, char output_file_index[3])
{
    // Compute the output file name
    char output_file_name[25] = "output_task3_board";
    output_file_name[18] = output_file_index[0];
    output_file_name[19] = output_file_index[1];
    strcat(output_file_name, ".json");

    int **red_reindexed, **green_reindexed, **blue_reindexed;
    allocateMatrix(&red_reindexed, bmpInfoHeader->height, bmpInfoHeader->width);
    allocateMatrix(&green_reindexed, bmpInfoHeader->height, bmpInfoHeader->width);
    allocateMatrix(&blue_reindexed, bmpInfoHeader->height, bmpInfoHeader->width);
    
    // Reindex the rgb matrices (for an easier approach): Read the matrix row by row starting from top-left
    int row_new_idx = bmpInfoHeader->height - 1; // Last row
    int col_new_idx = 0; // First column
    for(int col=0; col<bmpInfoHeader->width; ++col)
    {
        for(int row=0; row<bmpInfoHeader->height; ++row)
        {
            red_reindexed[row_new_idx][col_new_idx] = red[col][row];
            green_reindexed[row_new_idx][col_new_idx] = green[col][row];
            blue_reindexed[row_new_idx][col_new_idx] = blue[col][row];
            row_new_idx--;
        }
        row_new_idx = bmpInfoHeader->height - 1; // Reset the row index
        col_new_idx++; // Increment the col index
    }

    // Count the number of pink pixels for each square
    countPinkPixels(sudoku_digits, green_reindexed, bmpInfoHeader);    

    // Print to json file
    writeJsonOutput(sudoku_digits, output_file_name, output_file_index);

    // Free memory
    for(int row=0; row<bmpInfoHeader->width; ++row)
        free(red_reindexed[row]), free(green_reindexed[row]), free(blue_reindexed[row]);
    free(red_reindexed), free(green_reindexed), free(blue_reindexed);

}

// Complete the sudoku
void task4_bonus(int **sudoku_digits, char input_file_name_task5[34], char output_file_index[3], char _4bonus[2])
{
    bmp_fileheader *bmpFileHeader;
    bmp_infoheader *bmpInfoHeader;

    FILE *fp = fopen(input_file_name_task5, "rb");
    if(fp == NULL)
        fclose(fp), exit(404);
        
    bmpFileHeader = ReadBMFileHeader(fp);
    bmpInfoHeader = ReadBMInfoHeader(fp);

    unsigned char *pixel_array = (unsigned char *) calloc(bmpInfoHeader->biSizeImage, sizeof(unsigned char));
    fread(pixel_array, 1, bmpInfoHeader->biSizeImage, fp);

    int **red, **green, **blue;
    allocateMatrix(&red, bmpInfoHeader->height, bmpInfoHeader->width);
    allocateMatrix(&green, bmpInfoHeader->height, bmpInfoHeader->width);
    allocateMatrix(&blue, bmpInfoHeader->height, bmpInfoHeader->width);

    // Match for each color, the proper color index, ignoring the padding pixels
    int row = 0, col = 0;
    int i_cnt = 0;
    int padded = 0;
    int padded_orig = 0;
    for(int i=0; i<3*bmpInfoHeader->width*bmpInfoHeader->height; )
    {
        if(i % (3 * bmpInfoHeader->width * (i_cnt + 1) + i_cnt) == 0)
            padded++, i++;
        else
        {
            blue[row][col] = pixel_array[i++];
            green[row][col] = pixel_array[i++];
            red[row][col] = pixel_array[i++];
            col++;
            if(col == bmpInfoHeader->width)
                row++, col = 0;
        }
        if(padded > padded_orig)
        {
            padded_orig = padded;
            i_cnt++;
        }
    }

    // Rotate 90 degrees
    rotateMatrix(red, bmpInfoHeader);
    rotateMatrix(green, bmpInfoHeader);
    rotateMatrix(blue, bmpInfoHeader);

    int **red_reindexed, **green_reindexed, **blue_reindexed;
    allocateMatrix(&red_reindexed, bmpInfoHeader->height, bmpInfoHeader->width);
    allocateMatrix(&green_reindexed, bmpInfoHeader->height, bmpInfoHeader->width);
    allocateMatrix(&blue_reindexed, bmpInfoHeader->height, bmpInfoHeader->width);

    // Reindex the rgb matrices
    int row_new_idx = 0; // First row
    int col_new_idx = 0; // First column
    for(int col=0; col<bmpInfoHeader->width; ++col)
    {
        for(int row=0; row<bmpInfoHeader->height; ++row)
        {
            red_reindexed[row_new_idx][col_new_idx] = red[col][row];
            green_reindexed[row_new_idx][col_new_idx] = green[col][row];
            blue_reindexed[row_new_idx][col_new_idx] = blue[col][row];
            row_new_idx++;
        }
        row_new_idx = 0; // Reset the row index
        col_new_idx++; // Increment the col index
    }

    // Count the number of pink pixels for each square (81 steps)
    countPinkPixels(sudoku_digits, green_reindexed, bmpInfoHeader);

    // Solve sudoku
    int **freq_matrix; // Store the 0 value where we should complete the sudoku with magenta digits
    allocateMatrix(&freq_matrix, SUDOKU_ROWS, SUDOKU_COLS);

    // Fill the freq_matrix with zeros
    for(int row=0; row<SUDOKU_ROWS; ++row)
        memset(freq_matrix[row], 0, sizeof(int) * SUDOKU_COLS);

    // Put 1 where the digits are already on the sudoku matrix
    for(int row=0; row<SUDOKU_ROWS; ++row)
    {
        for(int col=0; col<SUDOKU_COLS; ++col)
        {
            if(sudoku_digits[row][col])
                freq_matrix[row][col] = 1;
        }
    }

    solveSudoku(sudoku_digits, 0, 0);

    if(_4bonus[0] == '4') // Task 4
    {
        // Write magenta digits
        int mask_1_red[8][8], mask_1_green[8][8], mask_1_blue[8][8];
        int mask_2_red[8][8], mask_2_green[8][8], mask_2_blue[8][8];
        int mask_3_red[8][8], mask_3_green[8][8], mask_3_blue[8][8];
        int mask_4_red[8][8], mask_4_green[8][8], mask_4_blue[8][8];
        int mask_5_red[8][8], mask_5_green[8][8], mask_5_blue[8][8];
        int mask_6_red[8][8], mask_6_green[8][8], mask_6_blue[8][8];
        int mask_7_red[8][8], mask_7_green[8][8], mask_7_blue[8][8];
        int mask_8_red[8][8], mask_8_green[8][8], mask_8_blue[8][8];
        int mask_9_red[8][8], mask_9_green[8][8], mask_9_blue[8][8];
        computeMask1(mask_1_red, mask_1_green, mask_1_blue);
        computeMask2(mask_2_red, mask_2_green, mask_2_blue);
        computeMask3(mask_3_red, mask_3_green, mask_3_blue);
        computeMask4(mask_4_red, mask_4_green, mask_4_blue);
        computeMask5(mask_5_red, mask_5_green, mask_5_blue);
        computeMask6(mask_6_red, mask_6_green, mask_6_blue);
        computeMask7(mask_7_red, mask_7_green, mask_7_blue);
        computeMask8(mask_8_red, mask_8_green, mask_8_blue);
        computeMask9(mask_9_red, mask_9_green, mask_9_blue);

        // Draw magenta digits on the table
        for(int row=0; row<SUDOKU_ROWS; ++row)
        {
            for(int col=0; col<SUDOKU_COLS; ++col)
            {
                if(freq_matrix[row][col] == 0)
                {
                    // Function to draw magenta digit (sudoku_digits[row][col]) at row (row) and column (col)
                    if(sudoku_digits[row][col] == 1)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_1_red, mask_1_green, mask_1_blue);
                    if(sudoku_digits[row][col] == 2)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_2_red, mask_2_green, mask_2_blue);
                    if(sudoku_digits[row][col] == 3)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_3_red, mask_3_green, mask_3_blue);
                    if(sudoku_digits[row][col] == 4)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_4_red, mask_4_green, mask_4_blue);
                    if(sudoku_digits[row][col] == 5)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_5_red, mask_5_green, mask_5_blue);
                    if(sudoku_digits[row][col] == 6)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_6_red, mask_6_green, mask_6_blue);
                    if(sudoku_digits[row][col] == 7)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_7_red, mask_7_green, mask_7_blue);
                    if(sudoku_digits[row][col] == 8)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_8_red, mask_8_green, mask_8_blue);
                    if(sudoku_digits[row][col] == 9)
                        drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_9_red, mask_9_green, mask_9_blue);
                }
            }
        }

        // Get back to the original indexing for preparing to write the output file
        row_new_idx = 0; // First row
        col_new_idx = 0; // First column
        for(int col=0; col<bmpInfoHeader->width; ++col)
        {
            for(int row=0; row<bmpInfoHeader->height; ++row)
            {
                red[row_new_idx][col_new_idx] = red_reindexed[col][row];
                green[row_new_idx][col_new_idx] = green_reindexed[col][row];
                blue[row_new_idx][col_new_idx] = blue_reindexed[col][row];
                row_new_idx++;
            }
            row_new_idx = 0; // Reset the row index
            col_new_idx++; // Increment the col index
        }

        // Write the output file: Compute the output file name
        char output_file_name[24] = "output_task4_board";
        output_file_name[18] = output_file_index[0];
        output_file_name[19] = output_file_index[1];
        strcat(output_file_name, ".bmp");

        FILE *output_file = fopen(output_file_name, "wb");
        if(output_file_index == NULL)
            fclose(output_file), exit(404); 

        // Write file header
        writeFileHeader(bmpFileHeader, output_file);

        // Write info header
        writeInfoHeader(bmpInfoHeader, output_file);

        // Write Pixel Data
        unsigned char *img = (unsigned char *) calloc(3 * bmpInfoHeader->width * bmpInfoHeader->height, sizeof(unsigned char));
        memset(img, 0, 3 * bmpInfoHeader->width * bmpInfoHeader->height);

        for(int i=0; i<bmpInfoHeader->width; ++i)
        {
            for(int j=0; j<bmpInfoHeader->height; ++j)
            {
                int x = i, y = (bmpInfoHeader->height - 1) - j;
                int r = red[i][j], g = green[i][j], b = blue[i][j];
                
                img[(x+y*bmpInfoHeader->width)*3 + 2] = (unsigned char) (r);
                img[(x+y*bmpInfoHeader->width)*3 + 1] = (unsigned char) (g);
                img[(x+y*bmpInfoHeader->width)*3 + 0] = (unsigned char) (b);
            }
        }
        
        unsigned char bmppad[3] = {0, 0, 0};
        for(int i=bmpInfoHeader->height-1; i>=0 ;--i)
        {
            fwrite(img + (bmpInfoHeader->width * (bmpInfoHeader->height-i-1)*3), 3, bmpInfoHeader->width, output_file); // Pixels
            fwrite(bmppad, 1, (4-(bmpInfoHeader->width*3)%4)%4, output_file); // Padding for each row
        }
        fclose(output_file);
        free(img);
    }
    else if(_4bonus[0] == '5') // Task bonus: Verify the sudoku this time: If isn't valid, print 'X' board
    {
        // Check sudoku validity
        if(checkSudoku(sudoku_digits) == 0) // Isn't valid
        {
            // Print the table with X on it
            int mask_X_red[8][8], mask_X_green[8][8], mask_X_blue[8][8];
            computeMaskX(mask_X_red, mask_X_green, mask_X_blue);
            for(int row=0; row<SUDOKU_ROWS; ++row)
            {
                for(int col=0; col<SUDOKU_COLS; ++col)
                {
                    drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col,
                    mask_X_red, mask_X_green, mask_X_blue);
                }
            }
        }
        else // Is valid
        {
            // Print magenta digits on it
            int mask_1_red[8][8], mask_1_green[8][8], mask_1_blue[8][8];
            int mask_2_red[8][8], mask_2_green[8][8], mask_2_blue[8][8];
            int mask_3_red[8][8], mask_3_green[8][8], mask_3_blue[8][8];
            int mask_4_red[8][8], mask_4_green[8][8], mask_4_blue[8][8];
            int mask_5_red[8][8], mask_5_green[8][8], mask_5_blue[8][8];
            int mask_6_red[8][8], mask_6_green[8][8], mask_6_blue[8][8];
            int mask_7_red[8][8], mask_7_green[8][8], mask_7_blue[8][8];
            int mask_8_red[8][8], mask_8_green[8][8], mask_8_blue[8][8];
            int mask_9_red[8][8], mask_9_green[8][8], mask_9_blue[8][8];
            computeMask1(mask_1_red, mask_1_green, mask_1_blue);
            computeMask2(mask_2_red, mask_2_green, mask_2_blue);
            computeMask3(mask_3_red, mask_3_green, mask_3_blue);
            computeMask4(mask_4_red, mask_4_green, mask_4_blue);
            computeMask5(mask_5_red, mask_5_green, mask_5_blue);
            computeMask6(mask_6_red, mask_6_green, mask_6_blue);
            computeMask7(mask_7_red, mask_7_green, mask_7_blue);
            computeMask8(mask_8_red, mask_8_green, mask_8_blue);
            computeMask9(mask_9_red, mask_9_green, mask_9_blue);

            // Draw magenta digits on the table
            for(int row=0; row<SUDOKU_ROWS; ++row)
            {
                for(int col=0; col<SUDOKU_COLS; ++col)
                {
                    if(freq_matrix[row][col] == 0)
                    {
                        // Function to draw magenta digit (sudoku_digits[row][col]) at row (row) and column (col)
                        if(sudoku_digits[row][col] == 1)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_1_red, mask_1_green, mask_1_blue);
                        if(sudoku_digits[row][col] == 2)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_2_red, mask_2_green, mask_2_blue);
                        if(sudoku_digits[row][col] == 3)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_3_red, mask_3_green, mask_3_blue);
                        if(sudoku_digits[row][col] == 4)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_4_red, mask_4_green, mask_4_blue);
                        if(sudoku_digits[row][col] == 5)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_5_red, mask_5_green, mask_5_blue);
                        if(sudoku_digits[row][col] == 6)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_6_red, mask_6_green, mask_6_blue);
                        if(sudoku_digits[row][col] == 7)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_7_red, mask_7_green, mask_7_blue);
                        if(sudoku_digits[row][col] == 8)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_8_red, mask_8_green, mask_8_blue);
                        if(sudoku_digits[row][col] == 9)
                            drawDigit(red_reindexed, green_reindexed, blue_reindexed, row, col, mask_9_red, mask_9_green, mask_9_blue);
                    }
                }
            }
        }

        // Get back to the original indexing for preparing to write the output file
        row_new_idx = 0; // First row
        col_new_idx = 0; // First column
        for(int col=0; col<bmpInfoHeader->width; ++col)
        {
            for(int row=0; row<bmpInfoHeader->height; ++row)
            {
                red[row_new_idx][col_new_idx] = red_reindexed[col][row];
                green[row_new_idx][col_new_idx] = green_reindexed[col][row];
                blue[row_new_idx][col_new_idx] = blue_reindexed[col][row];
                row_new_idx++;
            }
            row_new_idx = 0; // Reset the row index
            col_new_idx++; // Increment the col index
        }

        // Write the output file
        // Compute output file name
        char output_file_name[24] = "output_bonus_board";
        output_file_name[18] = output_file_index[0];
        output_file_name[19] = output_file_index[1];
        strcat(output_file_name, ".bmp");

        FILE *output_file = fopen(output_file_name, "wb");

        // Write file header
        writeFileHeader(bmpFileHeader, output_file);

        // Write info header
        writeInfoHeader(bmpInfoHeader, output_file);

        // Write Pixel Data
        unsigned char *img = (unsigned char *) malloc(3 * bmpInfoHeader->width * bmpInfoHeader->height * sizeof(unsigned char));
        memset(img, 0, 3 * bmpInfoHeader->width * bmpInfoHeader->height);

        for(int i=0; i<bmpInfoHeader->width; ++i)
        {
            for(int j=0; j<bmpInfoHeader->height; ++j)
            {
                int x = i, y = (bmpInfoHeader->height - 1) - j;
                int r = red[i][j], g = green[i][j], b = blue[i][j];
                
                img[(x+y*bmpInfoHeader->width)*3 + 2] = (unsigned char) (r);
                img[(x+y*bmpInfoHeader->width)*3 + 1] = (unsigned char) (g);
                img[(x+y*bmpInfoHeader->width)*3 + 0] = (unsigned char) (b);
            }
        }
        
        unsigned char bmppad[3] = {0, 0, 0};
        for(int i=bmpInfoHeader->height-1; i>=0 ;--i)
        {
            fwrite(img + (bmpInfoHeader->width * (bmpInfoHeader->height-i-1)*3), 3, bmpInfoHeader->width, output_file);
            fwrite(bmppad, 1, (4-(bmpInfoHeader->width*3)%4)%4, output_file); // Padding for each row
        }
        fclose(output_file);
        free(img);
    }

    // fclose(output_file);
    fclose(fp);

    // Free memory
    // free(img);
    free(pixel_array);

    for(int row=0; row<bmpInfoHeader->width; ++row)
        free(red[row]), free(green[row]), free(blue[row]);
    free(red), free(green), free(blue);

    for(int row=0; row<bmpInfoHeader->width; ++row)
        free(red_reindexed[row]), free(green_reindexed[row]), free(blue_reindexed[row]);
    free(red_reindexed), free(green_reindexed), free(blue_reindexed);

    for(int row=0; row<SUDOKU_ROWS; ++row)
        free(freq_matrix[row]);
    free(freq_matrix);

    free(bmpFileHeader);
    free(bmpInfoHeader);
}
