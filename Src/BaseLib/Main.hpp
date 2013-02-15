#ifndef main_h
#define main_h

/**
 * Library main header. Contains compiler and platform depended definition.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// windows specific
#ifdef _WIN32

	// windows dll specific
	#ifdef _WINDLL
		#define __dll__
	#endif

	// dynamic linking 
	#ifdef __dll__
		#define _DYNLINK __declspec( dllexport)
	#else
		#define _DYNLINK __declspec( dllimport)
	#endif

#else	// _WIN32

	// nothing
	#define _DYNLINK

#endif


#endif	// main_h