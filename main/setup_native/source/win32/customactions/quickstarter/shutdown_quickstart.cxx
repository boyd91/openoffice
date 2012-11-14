/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include "quickstarter.hxx"
#include <sfx2/qswin32.h>

static BOOL CALLBACK EnumWindowsProc( HWND hWnd, LPARAM lParam )
{
	MSIHANDLE	hMSI = static_cast< MSIHANDLE >( lParam );
	CHAR	szClassName[sizeof(QUICKSTART_CLASSNAMEA) + 1];

	int	nCharsCopied = GetClassName( hWnd, szClassName, sizeof( szClassName ) );

	if ( nCharsCopied && !stricmp( QUICKSTART_CLASSNAMEA, szClassName ) )
	{
		DWORD	dwProcessId;

		if ( GetWindowThreadProcessId( hWnd, &dwProcessId ) )
		{
			std::string	sImagePath = GetProcessImagePath( dwProcessId );
			std::string	sOfficeImageDir = GetOfficeInstallationPath( hMSI ) + "program\\";

			if ( !strnicmp( sImagePath.c_str(), sOfficeImageDir.c_str(), sOfficeImageDir.length() ) )
			{
				UINT	uMsgShutdownQuickstart = RegisterWindowMessageA( SHUTDOWN_QUICKSTART_MESSAGEA );

				if ( uMsgShutdownQuickstart )
					SendMessageA( hWnd, uMsgShutdownQuickstart, 0, 0 );
				

				HANDLE	hProcess = OpenProcess( SYNCHRONIZE, FALSE, dwProcessId );

				if ( hProcess )
				{
					WaitForSingleObject( hProcess, 30000 ); // Wait at most 30 seconds for process to terminate
					CloseHandle( hProcess );
				}

				return FALSE;
			}

		}
	}
	
	return TRUE;
}


extern "C" UINT __stdcall ShutDownQuickstarter( MSIHANDLE hMSI )
{
	EnumWindows( EnumWindowsProc, hMSI );

    return ERROR_SUCCESS;
}

