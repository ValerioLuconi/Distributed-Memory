/** 
 * @mainpage Distributed Memory Documentation
 *
 * @author Valerio Luconi
 * @version 0.1
 * @date June 2010
 *
 * @section intro Introduction
 * 
 * The Project is divided in two parts:
 * - A Distributed Memory Server and a library which contains an API for clients
 * - Two simple test clients which perform some operations on distributed memory
 *
 * @subsection server Server
 * Distributed Memory Server provides a fixed number of memory blocks. Clients
 * can operate on them using the provided API.
 * Operations are:
 * - Map a block
 * - Unmap a block
 * - Update local block content
 * - Write local content to distributed memory
 * - Unmap block
 * 
 * More than one Server can be instantiated, and they provide different portions
 * of a Distributed Memory.
 *
 * Server and clients interact with a simple protocol.
 * Client Requests:
 * - Map request: message <MAP, ID>
 * - Unmap request: message <UNMAP, ID>
 * - Update request: message <UPDATE, ID>
 * - Write request: message <WRITE, ID, data>
 * - Wait request: message <WAIT, ID>
 *
 * Server replies:
 * - Map reply: message <OK, data>
 * - Unmap reply: message <OK>
 * - Update replies: message <OK, data> or message <UPDATED>
 * - Write reply: message <OK>
 * - Generic error reply: message <ERROR>
 * - Write error reply: message <ERROR, INVALID> or message <ERROR, UNMAPPED>
 *
 * All messages are defined in msg.h file.
 *
 * @subsection test Test
 * One of the two clients will store a file in Distributed Memory (on two
 * Servers). Then it will count words in file and store the resulting number in
 * Distributed Memory. In the meanwhile the other client will wait for the first
 * to store the file, and then will map it in its Local Memory. Then it will
 * count spaces in file and store the resulting number in Distributed Memory.
 * Finally all the two clients will print the two numbers on screen.
 *
 * @section usage Usage
 * To compile Server and client's library digit make from src/ directory.
 *
 * To compile tests digit make from test/ directory.
 * 
 * To compile everything digit make from root directory
 *
 * To execute test digit ./test.sh from root directory.
 */
