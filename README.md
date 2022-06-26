# Image-Processing-Sudoku-Solver in C

## Taiatu Iulian Marius - 312CB

`Task 1`:
        I read from json file input and I computed the output file name.
        After that, I divided the pixel array into 3 matrices (red, green and blue),
        computed the data string and wrote the output file (.bmp), taking into
        consideration the padding pixels for each row.

`Task 2`:
        I computed the output file name and divided the pixel array 
        into 3 matrices (red, green and blue). For this task, I flipped the
        image horizontally first and then mirrored the flipped image.
        I computed the data string and wrote the output file (.bmp), taking 
        into consideration the padding pixels for each row.

`Task 3`:
        I used 3 auxiliary matrices (red_reindexed, green_reindexed, blue_reindexed).
        I changed the order of reading the pixels for a easier way of manipulating the
        sudoku matrix after all. For digit recognition, I counted the pink pixels from
        each square and with a little bit of differentiation between some digits,
        I computed the sudoku digits matrix. After this, I checked the validity of sudoku
        and printed the output to the proper json file.

`Task 4 + Bonus`:
        Taking into account the fact that the algorithm is the same for
        both tasks (backtracking), I combined those tasks into one function.
        First of all, I read from bmp input file: Fileheader, Infoheader and pixels data.
        I used, again, 3 matrices for holding the pixels color. I transposed the pixel array
        into those 3 matrices (red, green and blue) jumping over padding pixels.
        I manipulated the pixel matrices, reindexed as above and did the same algorithm
        as in the Task 3 for digit recognition and generating the sudoku_digits matrix.
        I made a matrix (freq_matrix) for holding the information where were the blank
        squares from beginning input. I solved the sudoku and here comes the differentiating part:

        If the input was for task4:
            I wrote magenta digits on the output file, because at this task (4) it is guaranteed 
            that the input sudokus are only doable sudokus.
            So, I did the same thing as above: Manipulate the pixel matrices and write data 
            into bmp output file
        If the input was for task bonus:
            I verified if the sudoku is valid:
                If isn't valid:
                    Print the bmp file completed with 'X' symbol
                If is valid:
                    Print magenta digits on it. I did the same thing as above: Manipulate the 
                    pixel matrices and write data into bmp output file


You can find more explanations for each function in the source code. Thank you! :)
