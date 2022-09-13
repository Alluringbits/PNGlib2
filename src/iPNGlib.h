#pragma once

#include "PNGlib.h"
#include <fstream>

namespace PNG{
	class iPNG : public basic_PNG{
		public:
			/**
			 * @brief Default empty constructor.
			 */
			iPNG() noexcept : basic_PNG("", &PNGifs), PNGifs{}{};
			/**
			 * @brief Constructor for the image file. It automatically calls open().
			 *
			 * @param f Filename of the image, it can be given at construction or at the call of the open() function
			 * 	  if the empty constructor has been used.
			 * @param flags The flags are needed to select how to display error messages.
			 *
			 * The library can be set to
			 * 	  automatically print error messages onto the terminal in different ways. The first character
			 * 	  indicates which messages to display: \n 
			 * 	  	-q Quiet mode. Prints no messages. \n 
			 * 	  	-s Soft Mode. Displays only Errors, no warnings or other messages. \n 
			 * 	  	-v Verbose Mode. Displays every kind of message. In reality only q and s are checked. 
			 * 	  	   Any character that isn't one of those two will default to this. \n 
			 * 	  The second character indicates the Display text formatting: \n 
			 * 	  	-s Short Message. The Message is displayed in a short format, e.g.: \n 
			 * 	  		Warning: Ancillary chunk with name "cHRM" is not supported. \n 
			 * 	  	-l Long Message. The message is displayed in a long format, e.g.: \n 
			 * 	  	  	Message Type:  Error \n 
			 *		  	Message Code:  1 \n 
			 *		  	Description :  noexist.png: Fatal I/O Error. The file either does not exist or it is inaccessible. \n 
			 *		   Any character that isn't one of these two will default on this. \n 
			 *
			 * 	  If ANSI colour codes are supported, each type of message will have different colour schemes.
			 *
			 */
			iPNG(std::string_view f, std::string_view flags = "ss") :  basic_PNG(f, &PNGifs, flags), PNGifs{}{(*this).open(f);};
			//read PNGlib.h for explanation
			iPNG(const iPNG &t) = delete;
			iPNG & operator=(const iPNG &t) = delete;

			/**
			 * @brief Default move constructor
			 *
			 * @param t iPNG variable to move.
			 */
			iPNG(iPNG && t) noexcept = default;
			/**
			 * @brief Default operator = move constructor.
			 *
			 * @param t iPNG variable to move.
			 *
			 * @return iPNG variable.
			 */
			iPNG& operator=(iPNG && t) noexcept = default;

			/**
			 * @brief Opens the image file and checks for errors/problems. Errors if the image has already been opened.
			 *
			 * @param t Filename, implicit conversion from const char * allowed
			 *
			 * @return Output Message.
			 */
			const message & open(std::string_view t) PNGEXC override;
			/**
			 * @brief Closes the image file.
			 *
			 * @return Output Message.
			 */
			const message & close() override;

			/**
			 * @brief Loads the image. Starts the process of reading chunks, verifying information, decompression,
			 * 	  defiltering and loads all the information inside the class (in all the appropriate variables).
			 *
			 *
			 * @return Output Message.
			 */
			const message & load();
			const message & repair(); //runs load() if not loaded already attempts to repair  chunks by calculating the correct CRC or chunk length
			void test2(){
			};

			/**
			 * @brief iPNG destructor. Calls PNG::iPNG::close() to try to close the file automatically before it gets destroyed (if it wasn't already closed).
			 */
			~iPNG() noexcept override {if(!isClosed) (*this).close();bPNGios = nullptr;};
		private:
			//conversion function for 4 bytes of chars to single integer	
			//file stream interface variables
			std::ifstream PNGifs;
			
			/**
			 * @brief Reads the chunk starting at the position left off by the previous functions.
			 *
			 * @return Output Message.
			 */
			const message& readChunk();
			/**
			 * @brief Reads the IHDR starting chunk.
			 *
			 * @return Output Message.
			 */
			const message& readIHDR();
			/**
			 * @brief Reads the eventual PLTE critical chunk.
			 *
			 * @return Output Message.
			 */
			const message& readPLTE();
			/**
			 * @brief Reads the critical IDAT chunk.
			 *
			 * @return Output Message.
			 */
			const message& readIDAT();
			/**
			 * @brief Reads the upcoming Ancillary chunk
			 *
			 * @param i Index position to identify the type of ancillary chunk. 1 to 10 are Singular Ancillary chunks, 10 to 14 Multiple Ancillary chunks.
			 *
			 * @return Output Message.
			 */
			const message& readAncillary(int i);
			/* const message& readAncM(int i); */
			/**
			 * @brief Decompresses the current IDAT stream.
			 *
			 * @return Output Message.
			 */
			const message& decompr();
			/**
			 * @brief Defilters the current uncompressed IDAT stream.
			 *
			 * @return Output Message.
			 */
			const message& defilter();

			/**
			 * @brief Filesize obtained through std::ifstream
			 */
			std::streamoff fs{};
			/**
			 * @brief Used to check if the file has already been loaded before.
			 */
			bool isLoaded{false};
	};
}
