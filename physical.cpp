/*---------------------------------------------------------------------------------------------------
-- SOURCE FILE: physical.cpp 
--
-- PROGRAM :    DUMB TERMINAL
-- 
-- FUNCTIONS:
-- DWORD WINAPI receiveMessages(LPVOID voider);
-- DWORD WINAPI create_thread_read(HANDLE hComm, HWND hwnd, char buffer[1], LPDWORD nHandle);
-- static DWORD WINAPI displayMessage(HWND hwnd, char buffer);
-- VOID sendMessagesSimple(HANDLE hComm, WPARAM wParam);
--
-- DATE: October 3, 2018
--
-- DESIGNER: Segal Au, A01000835
--
-- PROGRAMMER: Segal Au, A01000835
--
-- NOTES:
-- Handles creation of thread for reading input from serial port, as well as writing to serial port from
-- application. 
---------------------------------------------------------------------------------------------------*/


#include <windows.h>
#include <stdio.h>
#include "physical.h"
#include "SkyeTekAPI.h"
#include "SkyeTekProtocol.h"

static unsigned k = 0;
HANDLE hThreadRead;
DWORD readThreadId;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:       create_thread_read
--
-- INTERFACE :	   DWORD WINAPI create_thread_read(HANDLE hComm, HWND hwnd, char buffer[1], LPDWORD nRead)
--
-- DATE:           October 3rd, 2018
--
-- DESIGNER:       Segal Au, A01000835
--
-- PROGRAMMER:     Segal Au, A01000835
--
-- NOTES:          Creates thread to read inputs from serial port.
----------------------------------------------------------------------------------------------------------------------*/

DWORD WINAPI create_thread_read(HANDLE hComm, HWND hwnd, char buffer[1], LPDWORD nRead) {
	threadStructReadPoint threader = new threadStructRead(hComm, hwnd, buffer, nRead); 
	hThreadRead = CreateThread(NULL, 0, receiveMessages, (LPVOID)threader, 0, &readThreadId);
	if (!hThreadRead) {
		OutputDebugString("hThread for read wasn't initialized");
	}

	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:       receiveMessages
--
-- INTERFACE :	   DWORD WINAPI receiveMessages(LPVOID voider)
--
-- DATE:           October 3rd, 2018
--
-- DESIGNER:       Segal Au, A01000835
--
-- PROGRAMMER:     Segal Au, A01000835
--
-- NOTES:          Continuously reads input from serial port and sends inputs received to display (char by char). 
----------------------------------------------------------------------------------------------------------------------*/

DWORD WINAPI receiveMessages(LPVOID voider) {
	
	threadStructReadPoint threader = (threadStructReadPoint)voider;
	HANDLE hComm = (HANDLE)threader->hComm;
	const HWND hwnd = (HWND)threader->hwnd;
	/*char buffer[1] = threader->buffer;*/
	char buffer;
	DWORD nRead;

	
	
	while (1) {

		//Test if we can initialize the serial port, if so we can start reading?	
		COMMTIMEOUTS cto;
		GetCommTimeouts(hComm, &cto);
		cto.ReadIntervalTimeout = MAXDWORD;
		cto.ReadTotalTimeoutConstant = 0;
		cto.ReadTotalTimeoutMultiplier = 0;

		SetCommTimeouts(hComm, &cto);
		ReadFile(hComm, &buffer, 1, &nRead, NULL);
		if (nRead > 0) {
			displayMessage(hwnd, buffer);
		}
	}
	
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:       displayMessage
--
-- INTERFACE :	   static DWORD WINAPI displayMessage(HWND hwnd, char buffer)
--
-- DATE:           October 3rd, 2018
--
-- DESIGNER:       Segal Au, A01000835
--
-- PROGRAMMER:     Segal Au, A01000835
--
-- NOTES:          Displays character from buffer to window of application.
----------------------------------------------------------------------------------------------------------------------*/


static DWORD WINAPI displayMessage(HWND hwnd, char buffer) {

	HDC hdc;
	PAINTSTRUCT paintstruct;
	char str[80] = "";

	hdc = GetDC(hwnd);		 // get device context
	sprintf_s(str, "%c", (char)buffer); // Convert char to string
	TextOut(hdc, 13 * k, 0, str, strlen(str)); // output character	
	k++; // increment the screen x-coordinate
	ReleaseDC(hwnd, hdc); // Release device context
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION:       sendMessagesSimple
--
-- INTERFACE :	   VOID sendMessagesSimple(HANDLE hComm, WPARAM wParam)
--
-- DATE:           October 3rd, 2018
--
-- DESIGNER:       Segal Au, A01000835
--
-- PROGRAMMER:     Segal Au, A01000835
--
-- NOTES:          Handles writing to serial port. Run when character is inputted into application.
----------------------------------------------------------------------------------------------------------------------*/


VOID sendMessagesSimple(HANDLE hComm, WPARAM wParam) {
	char str[80];
	LPDWORD nWrite = 0; 
	char charToSend = wParam;

	if (!WriteFile(hComm, &charToSend, 1, nWrite, NULL)) {
		OutputDebugString("Write file failed!");
	}
}
