========================================================================
       DYNAMIC LINK LIBRARY : libsakusen_comms
========================================================================


AppWizard has created this libsakusen_comms DLL for you.  

This file contains a summary of what you will find in each of the files that
make up your libsakusen_comms application.

libsakusen_comms.dsp
    This file (the project file) contains information at the project level and
    is used to build a single project or subproject. Other users can share the
    project (.dsp) file, but they should export the makefiles locally.

libsakusen_comms.cpp
    This is the main DLL source file.

	When created, this DLL does not export any symbols. As a result, it 
	will not produce a .lib file when it is built. If you wish this project
	to be a project dependency of some other project, you will either need to 
	add code to export some symbols from the DLL so that an export library 
	will be produced, or you can check the "doesn't produce lib" checkbox in 
	the Linker settings page for this project. 

	MSVC specific changes: 
	
	errorutils.cpp now links against winsock.h, since that is where the socket error codes are defined.