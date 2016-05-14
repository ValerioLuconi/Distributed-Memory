/**
 * @file msg.h
 * @brief Header file containing communication protocol messages defines.
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 */

#ifndef MSG_H
#define MSG_H

/**
 * @def MAP
 * Map request type.
 */
#define MAP		0
/**
 * @def UNMAP
 * Unmap request type.
 */
#define UNMAP		1
/**
 * @def UPDATE
 * Update request type.
 */
#define UPDATE		2
/**
 * @def WRITE
 * Write request type.
 */
#define WRITE		3
/**
 * @def WAIT
 * Wait request type.
 */
#define WAIT		4

/**
 * @def OK
 * Ok response type.
 */
#define OK		5
/**
 * @def ERROR
 * Error response type.
 */
#define ERROR		6
/**
 * @def UPDATED
 * Updated response type (only for UPDATE requests).
 */
#define UPDATED		7

/**
 * @def UNMAPPED
 * Unmapped error reason (only for WRITE requests).
 */
#define UNMAPPED	8
/**
 * @def INVALID
 * Invalid error reason (only for WRITE requests).
 */
#define INVALID		9

#endif // MSG_H
