#ifndef sampleModule_h
#define sampleModule_h

// BaseLib includes
///
#include "Vector.hpp"
#include "Module.hpp"
#include "SequentialAcces.hpp"
#include "ReportProperty.hpp"

class Marker : public BaseLib::iModule, public BaseLib::Interfaces::SequentialAcces, public BaseLib::Properties::Reportable
{
public:
	Marker();
public: // Interfaces::SequentialAcces
	virtual bool Next(BaseLib::ExceptionHolder& ex);
	virtual BaseLib::Objects::Object* GetObject(BaseLib::ExceptionHolder& ex);
public: // reportable
	virtual void OnChange(BaseLib::Properties::BaseProperty* propoerty);
public: // iModule
	virtual const int GetOutputInterfaces();
	virtual void* AccessInterface(const int& interfaceId);
	virtual bool CheckIntegrity() throw();
protected:
	virtual void innerOpen();
	virtual void innerPrepare();
	virtual void innerClose();
	virtual void ValidateOutputSocket();
private:
	/**
	 * Name of added data.
	 */
	std::string mMarkName;
	/**
	 * Value of added data.
	 */
	int mMarkValue;
	/**
	 * Acces to source module.
	 */
	BaseLib::Interfaces::SequentialAcces* mAcces;
};


#endif // sampleModule_h