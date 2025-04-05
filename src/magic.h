/**
 * INFO0027: - Programming Techniques (Algorithmics)
 *  Project 1: Bytestream mapper
 * 
 * @file magic.h
 * @brief Interface for MAGIC ADT
 * @author Boustani Mehdi -- Albashityalshaier Abdelkader
 * @version 0.1
 * @date 04/04/2025
 *
 * Contains the interface and public API of the MAGIC ADT used for bytestream mapping
 * 
 */

#ifndef MAGIC_H
#define MAGIC_H

/**
 * @enum MAGICDirection
 * @brief Enum to define the mapping direction of the byte stream.
 */
// typedef enum {STREAM_IN_OUT = 0, STREAM_OUT_IN = 1} MAGICDirection;
enum MAGICDirection { STREAM_IN_OUT=0, STREAM_OUT_IN=1 };

/**
 * @struct magic
 * @brief Opaque data structure representing the MAGIC ADT.
 */
typedef struct magic *MAGIC;

/**
 * @brief Initializes the data structure used for MAGIC
 * 
 * This function creates and initializes an instance of the MAGIC ADT.
 * 
 * @return Pointer to the newly created instance of MAGIC ADT
 */
MAGIC MAGICinit();

/**
 * @brief Removes bytes from the input stream.
 * 
 * This function indicates the removal of a range of bytes in the input bytestream
 * 
 * @param m Pointer to the MAGIC instance
 * @param pos Start of the range to remove
 * @param length Number of bytes to remove
 * 
 */
void MAGICremove(MAGIC m, int pos, int length);

/**
 * @brief Adds bytes to the input stream
 * 
 * This function indicates the addition of bytes into stream
 * 
 * @param m Pointer to MAGIC instance
 * @param pos Start at which the bytes are added 
 * @param length Number of bytes to add
 * 
 */
void MAGICadd(MAGIC m, int pos, int length);

/**
 * @brief Maps a byte position between input and output streams
 * 
 * This function queries and returns the mapping of byte position in stream direction
 * 
 * @param m Pointer to MAGIC instance
 * @param direction Mapping direction
 * @param pos Position to map
 * 
 * @return Mapped byte position in the given direction
 */
int MAGICmap(MAGIC m, enum MAGICDirection direction, int pos);

/**
 * @brief Destroys the MAGIC instance
 * 
 * This function destroys and disposes of a given MAGIC instance
 * 
 * @param m Pointer to MAGIC instance 
 */
void MAGICdestroy(MAGIC m);

#endif
