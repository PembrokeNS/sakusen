 - Attempted to make this compile with VC2005 Express. Since it is legal and a little less antiquated. I hope running it doesn't break John's laptop.
 - To make Windows programs compile with VC2005, you must:
 Step 1: Install Visual C++ Express.

If you haven't done so already, install Visual C++ Express.



Step 2: Install the Microsoft Platform SDK. 

Install the Platform SDK over the Web from the Download Center. Follow the instructions and install the SDK for the x86 platform.



Step 3: Update the Visual C++ directories in the Projects and Solutions section in the Options dialog box. 

Add the paths to the appropriate subsection:

Executable files: C:\Program Files\Microsoft Platform SDK\Bin 

Include files: C:\Program Files\Microsoft Platform SDK\include 

Library files: C:\Program Files\Microsoft Platform SDK\lib

Note: Alternatively, you can update the Visual C++ Directories by modifying the VCProjectEngine.dll.express.config file located in the \vc\vcpackages subdirectory of the Visual C++ Express install location. Please make sure that you also delete the file "vccomponents.dat" located in the "%USERPROFILE%\Local Settings\Application Data\Microsoft\VCExpress\8.0" if it exists before restarting Visual C++ Express Edition. 



Step 4: Update the corewin_express.vsprops file.

One more step is needed to make the Win32 template work in Visual C++ Express. You need to edit the corewin_express.vsprops file (found in C:\Program Files\Microsoft Visual Studio 8\VC\VCProjectDefaults) and

Change the string that reads:

AdditionalDependencies="kernel32.lib" to

AdditionalDependencies="kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib"



Step 5: Generate and build a Win32 application to test your paths. 

In Visual C++ Express, the Win32 Windows Application type is disabled in the Win32 Application Wizard. To enable that type, you need to edit the file AppSettings.htm file located in the folder “%ProgramFiles%\Microsoft Visual Studio 8\VC\VCWizards\AppWiz\Generic\Application\html\1033\".

In a text editor comment out lines 441 - 444 by putting a // in front of them as shown here:

// WIN_APP.disabled = true;
// WIN_APP_LABEL.disabled = true; 
// DLL_APP.disabled = true; 
// DLL_APP_LABEL.disabled = true; 

Save and close the file and open Visual C++ Express. 

From the File menu, click New Project. In the New Project dialog box, expand the Visual C++ node in the Product Types tree and then click Win32. Click on the Win32 Console Application template and then give your project a name and click OK. In the Win32 Application Wizard dialog box, make sure that Windows application is selected as the Application type and the ATL is not selected. Click the Finish button to generate the project. 

As a final step, test your project by clicking the Start button in the IDE or by pressing F5. Your Win32 application should build and run. 

In order to get command-line compiling working (for STLport), you must also edit your vcvars32.bat file to include the Platform SDK, specifically, if the Platform SDK is installed in "C:\Program Files\Microsoft Platform SDK\", you need to add:

"C:\Program Files\Microsoft Platform SDK\Bin;" to the PATH= line
"C:\Program Files\Microsoft Platform SDK\Include" to the INCLUDE= line
"C:\Program Files\Microsoft Platform SDK\Lib" to the LIB= line

By default, vcvars32.bat (or the file by that name that does the work; there are several) is in "C:\Program Files\Microsoft Visual Studio 8\Common7\Tools". Good luck.