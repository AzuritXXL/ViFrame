#ifndef eny_moduleHolder_h
#define eny_moduleHolder_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart libraries include
///
#include <string>

// BaseLib include
///
#include "Module.hpp"

namespace Logic {

// Forward declaration
///
class Library;

/**
 * Wrap extern module.
 */
class ModuleHolder
{
public:
	// Library can acces our protected memeber, to delete them
	friend class Library;
public:
	/**
	 * Create module holder.
	 * @param library that create object
	 * @param module to hold
	 */
	ModuleHolder(Library* library, BaseLib::iModule* module);
	/**
	 * Destroy ModuleHolder. Does not release holded module !
	 * Call release to release it.
	 */
	~ModuleHolder();
public:
	/**
	 * Return holded module id.
	 * @param module id
	 */
	const std::string& GetId() const;
	/**
	 * @return pointer to creator library
	 */
	Library* GetLibrary();
	/**
	 * @return pointer to holded module
	 */
	BaseLib::iModule* GetModule();
	/**
	 * Try to release holded module through Creator library.
	 * @exception std::exception
	 */
	void Release();
private:
	/**
	 * Pointer to library that createt module holder.
	 */
	Library* mCreator;
	/**
	 * Holded module.
	 */
	BaseLib::iModule* mModule;
};

};

#endif // eny_moduleHolder_h