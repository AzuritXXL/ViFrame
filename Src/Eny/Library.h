#ifndef eny_library_h
#define eny_library_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Windows includes 
///
#ifdef WIN32
	#include <Windows.h>
#endif

// Standart includes
///
#include <string>

// BaseLib icludes
///
#include "Module.hpp"
#include "Description.hpp"

namespace Logic {

// Forward declaration
///
class ModuleHolder;

/**
 * Hold and manage dynamic loaded libraries (module sources).
 */
class Library
{
public:
	Library(const std::string& filepath, const std::string& id);
	/**
	 * If module is loaded call Unload, any exception is supressed.
	 */
	~Library();
public:
	/**
	 * @return true if library is loaded
	 */
	bool GetIsLoaded() const throw();
	/**
	 * @return library id
	 */
	const std::string& GetId() const throw();
	/**
	 * Load library.
	 * @exception std::exception
	 */
	void Load();
	/**
	 * Unload library.
	 * @exception std::exception
	 */
	void UnLoad();
	/**
	 * Create module holder from library.
	 * Load library if need.
	 * @param[in] new module id
	 * @return new module holder
	 * @exception std::exception propagate from Load function
	 */
	ModuleHolder* Create(const std::string& moduleId);
	/**
	 * Load library if need.
	 * Delete/release module in module holder created by library. Module holder stay valid, 
	 * so to complete release module holder must be deleted.
	 * @param[in] pointer to module holder
	 * @exception std::exception propagate from Load function
	 */
	void Release(ModuleHolder* holder);
	/**
	 * Load library if need.
	 * @return module description
	 * @exception std::exception propagate from Load function
	 */
	const BaseLib::Description& GetDescription();
	/**
	 * @return number of existing dependency
	 */
	int GetDependencyCount();
private:
	typedef void* (*ModuleCtor)();
	typedef void  (*ModuleDtor)(void*);
	typedef const BaseLib::Description* (*ModuleDescription)();
private:
#ifdef WIN32
	/**
	 * Library holder.
	 */
	HMODULE mLibrary;
#endif
	/**
	 * Pointer to function which create module.
	 */
	ModuleCtor fceCreate;
	/**
	 * Pointer to function which destroy module.
	 */
	ModuleDtor fceDestroy;
	/**
	 * Pointer to description function.
	 */
	ModuleDescription fceDescription;
private:
	/**
	 * True if library is loaded.
	 */
	bool mIsLoaded;
	/**
	 * Library file name.
	 */
	std::string mFilePath;
	/**
	 * Library identification.
	 */
	std::string mId;
	/**
	 * Number of existing modules created by library.
	 */
	int mDependencyCount;
};

}

#endif // eny_library_h