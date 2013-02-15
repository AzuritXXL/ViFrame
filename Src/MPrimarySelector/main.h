#ifndef primarySelector_h
#define primarySelector_h

/**
 * Contains main module class definition. It's ment to be used
 * as a part of visualisation pathway.
 *
 * @author 
 * @version 1.0
 * @date 1.5.2012
 */

// BaseLib includes
//
#include "Object.hpp"
#include "Module.hpp"
#include "ReportProperty.hpp"
#include "RandomAcces.hpp"
#include "SequentialAcces.hpp"
#include "IntSelectionReport.hpp"

/**
 * Class provide ability to change objects main data, according to user 
 * selection.
 */
class PrimarySelector : public BaseLib::iModule, 
	BaseLib::Interfaces::SequentialAcces, BaseLib::Interfaces::RandomAcces, BaseLib::Properties::Reportable
{
public:
	PrimarySelector();
public: // BaseLib::iModule
	virtual const int GetOutputInterfaces();
	void* AccessInterface(const int& interfaceId);
	virtual bool PrepareForConnection();
	virtual bool CheckIntegrity();
	virtual void PropertyLoadSecondWave();
protected:
	virtual void innerOpen();
	virtual void innerPrepare();
	virtual void innerClose();
	virtual void ValidateOutputSocket();	
public: // BaseLib::Interfaces::SequentialAcces
	virtual bool Next(BaseLib::ExceptionHolder& ex);
	virtual BaseLib::Objects::Object* GetObject(BaseLib::ExceptionHolder& ex);
public: // BaseLib::Interfaces::RandomAcces
	virtual size_t GetCount() const;
	virtual BaseLib::Objects::Object* GetObject(const size_t& index, BaseLib::ExceptionHolder& ex);
	virtual void ClearOutState();
public: // BaseLib::Properties::Reportable
	virtual void OnChange(BaseLib::Properties::BaseProperty* prop);
protected:
	/**
	 * Type to store object. Bool value is true if module own object 
	 * and so should delete it when close.
	 */
	typedef std::pair<bool, BaseLib::Objects::Object*> ObjectType;
protected:
	/**
	 * Promerty to select primary atribute name.
	 */
	BaseLib::Properties::IntSelectionReport* mPrimaryAtt;
	/**
	 * Index of new primary att ( index lead into mPrimaryAtt.Values )
	 */
	int mPrimaryAttIndex;
	/**
	 * Store indentification for new primary.
	 */
	BaseLib::Sockets::Socket::PrimaryType mNewPrimary;
	/**
	 * Socket used to generate values for mPrimaryAtt.
	 * Used to obtain full name.
	 */
	BaseLib::Sockets::Socket mPrimarySourceSocket;
	/**
	 * Acces to sequential acces to source.
	 */
	BaseLib::Interfaces::SequentialAcces* mSequentialSourceAcces;
	/**
	 * Acces to random interface of source.
	 */
	BaseLib::Interfaces::RandomAcces* mRandomSourceAcces;
	/**
	 * String used to modify main value. Is use as it is, 
	 * so it must have level prefix. (like 0.maindata )
	 */
	std::string mPrimaryAttName;
private:
	/**
	 * Update possible propoerty selection.
	 */
	void UpdateProperty();
	/**
	 * Update mNewPrimary according to data in 
	 * mPrimarySourceSocket and mPrimaryAttIndex.
	 */
	void UpdatePrimary();
};

extern "C" _DYNLINK void* CreateModule();

extern "C" _DYNLINK void DeleteModule(void*);

extern "C" _DYNLINK const BaseLib::Description* GetDescription();

#endif // primarySelector_h