/** @file RailWayMaker.c
* @author  Yair Escott <yair.95@gmail.com>
*
* @brief a System that finds the cheapest track for a given track length
*/

// ------------------------------ includes ------------------------------
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <ctype.h>


/**
 * @def struct Track
 * @brief a struct which contains a type of train track
 */
typedef struct Track
{
    int length;
    char left;
    char right;
    int price;
}track;

//parser parses the inout file and calls the info checker when parsed*/
int fileParser(char* filename);
//returns the output file to the user
void output(char* string, int* val, int period);
//checks that the first line of the file is valid
int trackLengthCheck(char line[]);
//checks that the 2nd line of the file if valid
int numOfConnectionsCheck(char line[]);
//checks that the 3rd line of the file is valid
int connectionsCheck(char line[], char** connectionsArr);
//checks that the line with the track information is valid
int trackCheck(char line[], char** connectionsArr, track** tracksArr);
// adds the connection to the array of chars
void charPush(char*** arr, int index, char* value, int *size, int *capacity);
// adds the track to the array of tracks
void trackPush(track*** tracksarr, track* newtrack);
//finds the track with the cheapest price
void cheapest(char* connectionsArr, track* tracksArr);
// checks if a char is a number
int isNumeric (const char * s);

// -------------------------- const definitions -------------------------

//the line counter for the file
int gLine = 1;
// the minimum price
int gMin = 0;
//the length of the desired track
int gLength;
// the number of chars that represent connections
int gNumOfConnections;
//number of connection types
int gNumOfTracks = 0;
//the capacity of the track array
int gTrackCapacity = 0;

// the amount of desired arguments
#define ARGC 2
// output for when the file dosnt exist
#define NO_FILE "File doesn't exists."
// output for when the file is empty
#define EMPTY_FILE "File is empty."
// output for when there is an error in a certain line
#define INVALID_INPUT "Invalid input in line: "
// output for when the minimal price has been recovered
#define OUTPUT "The minimal price is: "
// the initial capacity of the arrays
#define INITIAL_CAPACITY 4
// output for when the number of arguments is wrong
#define BAD_ARGS_NUM "Usage: RailWayPlanner <InputFile>"
// the maximum length of a line
#define LINELENGTH 1024

// ------------------------------ functions -----------------------------

/**
 * @brief the main function receives the arguments and starts off at the right method
 * @param argc the number of arguments
 * @param argv the array containing the file path
 * @return EXITFAILURE if unsuccessful and EXITSUCCESS otherwise
 */
int main(int argc, char* argv[1])
{
    if(argc == ARGC)
    {
        return fileParser(argv[1]);
    }
    else
    {
        output(BAD_ARGS_NUM, NULL, 0);
        return EXIT_FAILURE;
    }
}

/**
 * @brief parses through the lines of the input file and calls functions accordingly
 * @return EXITFAILURE if an error occured and EXITSUCCESS if not
 */
int fileParser(char* filename)
{
    track* tracks = (track*)malloc(INITIAL_CAPACITY * sizeof(track));
    if(tracks == NULL)
    {
        return EXIT_FAILURE;
    }
    char* connections = (char*) malloc(INITIAL_CAPACITY);
    if(connections == NULL)
    {
        return EXIT_FAILURE;
    }
    FILE* file;
    file = fopen(filename, "r");
    if(file == NULL)
    {
        output(NO_FILE, NULL, 0);
        return EXIT_FAILURE;
    }
    fseek(file, 0, SEEK_END);
    unsigned long len = (unsigned long)ftell(file);
    if (len <= 0)
    {
        output(EMPTY_FILE, NULL, 0);
        return EXIT_FAILURE;
    }
    else
    {
        rewind(file);
        char line[LINELENGTH];
        while (fgets(line, LINELENGTH, file)!= NULL)
        {
            if (gLine == 1)
            {
                if (trackLengthCheck(line))
                {
                    output(INVALID_INPUT, &gLine, 1);
                    return EXIT_FAILURE;
                }
                gLine += 1;
                continue;
            }
            if (gLine == 2)
            {
                if (numOfConnectionsCheck(line))
                {
                    output(INVALID_INPUT, &gLine, 1);
                    return EXIT_FAILURE;
                }
                gLine += 1;
                continue;
            }
            if (gLine == 3)
            {
                if (connectionsCheck(line, &connections))
                {
                    output(INVALID_INPUT, &gLine, 1);
                    return EXIT_FAILURE;
                }
                gLine += 1;
                continue;
            }
            else
            {
                if (trackCheck(line, &connections, &tracks))
                {
                    output(INVALID_INPUT, &gLine, 1);
                    return EXIT_FAILURE;
                }
                gLine += 1;
                continue;
            }
        }
        cheapest(connections, tracks);
        output(OUTPUT, &gMin, 0);
    }
    free(connections);
    connections = NULL;
    free(tracks);
    tracks = NULL;
    fclose(file);
    return EXIT_SUCCESS;
}

/**
 * @brief checks if a string is a number
 * @param string the string
 * @return true if is a number and false if not
 */
int isNumeric (const char * string)
{
    if (string == NULL || *string == '\0' || isspace(*string))
    {
        return 0;
    }
    char * p;
    strtod (string, &p);
    return *p == '\0' || *p == '\n';
}

/**
 * @brief creates a file and puts the relevant output into it
 * @param string the output
 * @param val the line number if needed
 */
void output(char* string, int* val, int period)
{
    FILE * fPtr;
    fPtr = fopen("railway_planner_output.txt", "w");
    fputs(string, fPtr);
    if(val != NULL)
    {
        fprintf(fPtr, "%d", *val);
        if(period == 1)
        {
            fprintf(fPtr, "%c", '.');

        }
    }
    fclose(fPtr);
}

/**
 * @brief checks the first line of the input file
 * @param line the lline itself
 * @return EXIT_FAILURE if there is an error and EXIT_SUCCESS if not
 */
int trackLengthCheck(char line[])
{
    for (int i = 0; i < (int) strlen(line) -1; i++)
    {
        if (!isdigit(line[i]) && line[i] != '\n')
        {
            return EXIT_FAILURE;
        }
    }
    long num = atoi(line);
    if(num >= 0)
    {
        gLength = (int) num;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/**
 * @brief checks the second line of the input file
 * @param line the line itself
 * @return EXIT_FAILURE if there is an error and EXIT_SUCCESS if not
 */
int numOfConnectionsCheck(char line[])
{
    for (int i = 0; i < (int) strlen(line) - 1; i++)
    {
        if (!isdigit(line[i]) && line[i] != '\n')
        {
            return EXIT_FAILURE;
        }
    }
    long num = atoi(line);
    if(num > 0)
    {
        gNumOfConnections = (int) num;
        return EXIT_SUCCESS;
    }
    return EXIT_FAILURE;
}

/**
 * @brief checks the third line of the input file
 * @param line the line itself
 * @param connectionsArr the array to add the connections for future use
 * @return EXIT_FAILURE if there is an error and EXIT_SUCCESS if not
 */
int connectionsCheck(char line[], char** connectionsArr)
{
    int size = 0;
    int capacity = INITIAL_CAPACITY;
    char* token = strtok(line, ",");
    for(int i = 0; i < gNumOfConnections; i++)
    {
        if((int)strlen(token) != 1)
        {
            char* token2 = strtok(token, "\n");
            if((int)strlen(token2) != 1)
            {
                return EXIT_FAILURE;
            }
            charPush(&connectionsArr, i, token2 , &size, &capacity);
            continue;
        }
        charPush(&connectionsArr, i, token, &size, &capacity);
        token = strtok(NULL, ",");
    }
    return EXIT_SUCCESS;
}

/**
 * @brief checks the fourth line of the input file
 * @param line the line itself
 * @param connectionsArr the arrray of char connections
 * @param tracksArr the array which we will add the tracks to
 * @return EXIT_FAILURE if there is an error and EXIT_SUCCESS if not
 */
int trackCheck(char line[], char** connectionsArr, track** tracksArr)
{
    int leftFlag = 1;
    int rightFlag = 1;
    track newtrack;
    char* token;
    token = strtok(line, ",");
    char* left = token;
    char left2 = token[0];
    token = strtok(NULL, ",");
    char* right = token;
    char right2 = token[0];
    token = strtok(NULL, ",");
    char* length = token;
    token = strtok(NULL, "\n");
    char* price = token;
    if(!isNumeric(length) || !isNumeric(price) || (int)strlen(left) != 1 || (int)strlen(right) != 1)
    {
        return EXIT_FAILURE;
    }
    newtrack.price = atoi(price);
    newtrack.length = atoi(length);
    newtrack.left = (char) left2;
    newtrack.right = (char) right2;

    //check if the connections are valid
    for(int i = 0; i < gNumOfConnections; i++)
    {
        if(newtrack.left == (*connectionsArr)[i])
        {
            leftFlag = 0;
        }
        if(newtrack.right == (*connectionsArr)[i])
        {
            rightFlag = 0;
        }
        if(rightFlag == 0 && leftFlag == 0)
        {
            break;
        }
    }
    if(rightFlag == 1 || leftFlag == 1)
    {
        return EXIT_FAILURE;
    }
    if(newtrack.length <= 0 || newtrack.price <= 0)
    {
        return EXIT_FAILURE;
    }
    trackPush(&tracksArr, &newtrack);
    return EXIT_SUCCESS;
}

/**
 * @brief adds a track to the track array using memory allocation
 * @param tracksarr the array which we will add the tracks to
 * @param index the index in the array
 * @param newtrack the new track
 * @param size number of tracks in the array
 * @param capacity the capacity of the array
 */
void trackPush(track*** tracksarr, track* newtrack)
{
    if(gNumOfTracks == gTrackCapacity)
    {
        **tracksarr = (track*) realloc(**tracksarr, sizeof(track) * (4 + gNumOfTracks));
        gTrackCapacity += 4;
    }
    (**tracksarr)[gNumOfTracks] = *newtrack;
    gNumOfTracks += 1;
}

/**
 * @brief adds a char to the connections array using memory allocation
 * @param arr the connections array
 * @param index the index in the array
 * @param value the connection
 * @param size the number of chars in the array
 * @param capacity the capacity of the array
 */
void charPush(char*** arr, int index, char* value, int *size, int *capacity)
{
    if(*size == *capacity)
    {
        **arr = (char*) realloc(**arr, sizeof(char) * (4 + *size));
        *capacity += sizeof(char) * 4;
    }
    (**arr)[index] = *value;
    *size = *size + 1;
}

/**
 * @brief finds the cheapest track
 * @param connectionsArr the array of connections
 * @param tracksArr the array of tracks
 */
void cheapest(char* connectionsArr, track* tracksArr)
{
    int** table;
    table = (int**) malloc((gLength + 1)*sizeof(int*));
    for(int h = 0; h < gLength + 1; h++)
    {
        table[h] = (int*) malloc(gNumOfConnections*sizeof(int));
    }
    for(int r = 0; r < gLength + 1; r++)
    {
        for(int y = 0; y < gNumOfConnections; y++)
        {
            table[r][y] = -11;
        }
    }
    for(int i = 0; i < gLength + 1; i++)
    {
        for(int j = 0; j<gNumOfConnections; j++)
        {
            if(i == 0)
            {
                table[i][j] = 0;
            }
            else
            {
                int tempCheapest = 0;
                for(int k = 0; k<gNumOfTracks; k++)
                {
                    if(i != 1)
                    {
                        if(tracksArr[k].length == i &&
                           tracksArr[k].right == connectionsArr[j] &&
                           (tempCheapest >= tracksArr[k].price || tempCheapest == 0))
                        {
                            tempCheapest = tracksArr[k].price;
                            continue;
                        }
                        else if(tracksArr[k].length < i)
                        {
                            for (int l = 0; l < gNumOfConnections; l++)
                            {
                                if (table[i - tracksArr[k].length][l] != INT_MAX &&
                                    tracksArr[k].right == connectionsArr[j] &&
                                    tracksArr[k].left == connectionsArr[l] && (tempCheapest == 0
                                    || table[i - tracksArr[k].length][l] + tracksArr[k].price <
                                    tempCheapest))
                                {
                                    if (table[i - tracksArr[k].length][l] + tracksArr[k]
                                        .price < 0)
                                    {
                                        tempCheapest = INT_MAX;
                                    }
                                    else
                                    {
                                        tempCheapest = table[i - tracksArr[k].length][l] +
                                                       tracksArr[k].price;
                                    }
                                }
                            }
                        }
                    }
                    else
                    {
                        if(tracksArr[k].right == connectionsArr[j] && tracksArr[k].length == 1)
                        {
                            if (tempCheapest > tracksArr[k].price || tempCheapest == 0)
                            {
                                tempCheapest = tracksArr[k].price;
                            }
                        }
                    }
                }
                if(tempCheapest == 0)
                {
                    table[i][j] = INT_MAX;
                }
                else
                {
                    table[i][j] = tempCheapest;
                }
            }
        }
    }
    int min = 0;
    for(int m = 0; m < gNumOfConnections; m++)
    {
        if(min == 0)
        {
            if (table[gLength][m] != INT_MAX)
            {
                gMin = table[gLength][m];
                min = gMin;
            }
            else
            {
                gMin = -1;
            }
        }
        else
        {
            if(table[gLength][m] < min)
            {
                gMin = table[gLength][m];
                min = gMin;
            }
        }
    }
    for(int a = 0; a < gLength + 1; a++)
    {
        free(table[a]);
        table[a] = NULL;
    }
    free(table);
    table = NULL;
}