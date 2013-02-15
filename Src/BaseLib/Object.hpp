#ifndef baseLib_objects_object_h
#define baseLib_objects_object_h

/**
 * Contains data object class.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <map>
#include <string>

// Local includes
///
#include "Vector.hpp"
#include "Variable.hpp"

namespace BaseLib {
namespace Objects {

/**
 * Class for keeping set of named data. Used as
 * a base data object passed in between modules.
 *
 * The object contains logic to prevent name collision.
 * Each name is prefixed with a counter that should increase as
 * the object is forwarded from one module to another. 
 *
 * Object also prevents from assigned or copying because of 
 * holding pointers to dynamiclay allocated data.
 */
class Object
{
public:
	typedef std::map<std::string, Variant*> DataType;
public:
	/**
	 * Create a new empty object.
	 * @param Object name
	 */
	Object(const std::string& name);
	/**
	 * Delete object data.
	 */
	~Object();
public:
	/**
	 * Return objects name.
 	 * @return Objects name
	 */
	const std::string& GetName();
	/**
	 * Set pointer to main data.
	 * Name is prefixed with actual passCount.
	 * @param[in] name Main data name.
	 */
	void SetMain(const std::string& name);
	/**
	 * Set pointer to main data.
	 * Name is used as is, withnout prefixing. 
	 * @param[in] Main data name
	 */
	void SetMainPure(const std::string& name);
	/**
	 * Returns pointer to main data.
	 * @return Pointer to main data
	 */
	Variant* GetMain();
	/**
	 * Returns pointer to all stored data.
	 * @return data storage
	 */
	const DataType& GetData() const;
	/**
	 * Adds data with given name. If there exists data under the same name, 
	 * the original data are deleted. Prefixes data with mPassCounter.
	 * @param[in] name Data name.
	 * @param[in] data Pointer to new data.
	 */
	void Add(const std::string& name, Variant* data);
	/**
	 * Add data under certain name. If there is data under the same name 
	 * original data are deleted. Do not prefix data with mPassCounter.
	 * @param[in] data name
	 * @param[in] pointer to new data
	 */
	void AddPure(const std::string& name, Variant* data);
	/**
	 * Adds data with given name. If there exists data under the same name,
	 * original data are deleted. Prefixes data with mPassCounter.
	 * Returns full name under which the object can be accesed.
	 * @param[in] name Data name.
	 * @param[in] data Pointer to the new data.
	 * @param[out] fullName Returns inserted objects full name.
	 */
	void Add(const std::string& name, Variant* data, std::string& fullName);
	/**
	 * Removes data with given name. If the data are marked as 
	 * main data, then randomly selects new main data.
	 * @param[in] name Data name.
	 */
	void Remove(const std::string& name);
	/**
	 * Removes pointers to all stored data. Without deleting them.
	 */
	void GiveUp();
	/**
	 * Called when an object is passed from one module to another
	 * ( when the work on an object in a module is completed ).
	 */
	void Pass();
	/**
	 * Allows to directly set the pass counter.
	 * Use of this function can lead to attribute
	 * name collision and to memory leaks.
	 * @param[in] value New pass counter.
	 */
	void SetPass(int value);
	/**
	 * Finds a full name for required data.
	 * @param[in] name Short name, under which the object is stored.
	 * @param[in] type Data type.
	 * @return Full key value, empty string if nothing was found.
	 */
	std::string Find(const std::string& name, int type) const;
private:
	Object(const Object& data);
	const Object& operator= (const Object& data);
private:
	/**
	 * Counts steps through object's life.
	 * Number of passes between modules.
	 */
	int mPassCounter;
	/**
	 * Object name.
	 */
	std::string mName;
	/**
	 * Stores object data.
	 */
	DataType mData;
	/**
	 * Stores pointer to main data.
	 */
	Variant* mMainData;
};

} }

#endif // baseLib_objects_object_h