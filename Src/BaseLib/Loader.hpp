#ifndef baseLib_modules_loader_h
#define baseLib_modules_loader_h

/**
 * Contains partialy pre-implemented class, that should be used
 * as an objectSource.
 *
 * @author
 * @version 1.0
 * @date 20.5.2012
 */

// Local includes
/// 
#include "Object.hpp"
#include "Module.hpp"
#include "SequentialAcces.hpp"

namespace BaseLib {
namespace Modules {

/**
 * Specialized subclased module class that should be used as a data source. 
 * Provides basic pre-implementation that deals with common functionality.
 * As a source module does not have any input socket. Sequential acces is provided.
 *
 * When subclassing, implement: GenerateObject, innerOpen, innerPrepare and innerClose
 *
 */
class Loader : public iModule, public Interfaces::SequentialAcces
{
public:
	Loader();
	virtual ~Loader();
protected:
	/**
	 * Generates a single object and returns a pointer to it.
	 * After last object return nullptr.
	 * @return object or nullptr
	 */
	virtual BaseLib::Objects::Object* GenerateObject() = 0;
public: // Interfaces::SequentialAcces
	virtual bool Next(ExceptionHolder& ex);
	virtual Objects::Object* GetObject(ExceptionHolder& ex);
public: // iModule
	virtual const int GetOutputInterfaces();
	virtual void* AccessInterface(const int& interfaceId);
	virtual bool CheckIntegrity() throw();
protected:
	virtual void ValidateOutputSocket();
private:
	/**
	 * Hold object genereted by GenerateObject until next
	 * calling of the Next, when it is replaced with new object.
	 */
	Objects::Object* mObject;
	/**
	 * True if mObject was passed by GetObject. 
	 * Determine whether to delete mObject on close or not.
	 */
	bool mObjectRelease;
};


} }

#endif // baseLib_modules_loader_h