#ifndef baseLib_modules_visualiser_h
#define baseLib_modules_visualiser_h

/**
 * Contains partialy pre-implemented class, that should be used
 * as a visualiser.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <string>
#include <vector>
#include <assert.h>

// Local includes
///
#include "Object.hpp"
#include "Module.hpp"
#include "Socket.hpp"
#include "RandomAcces.hpp"
#include "SequentialAcces.hpp"
#include "Exception.hpp"

namespace BaseLib {
namespace Modules {

/**
 * Template for visualisation modules. Gather source data
 * and provide access to them to do the job. 
 * It is guaranteed that the source and output data are different.
 *
 * When subclassing implement: Visualise
 * 
 * INTYPE - required input data type ( subclass of Objects::Variant )
 * OUTTYPE - output type ( subclass of Objects::Variant )
 */
template <typename INTYPE, typename OUTTYPE> class Visualiser 
	: public ::BaseLib::iModule, ::BaseLib::Interfaces::RandomAcces
{
public:
	/**
	 * Contains information about the stored object. 
	 * Because of requirements in ::BaseLib::Interfaces::RandomAcces
	 * we need to know if we own the object or not and possibly delete it.
	 */
	struct ObjectType
	{
	public:
		/**
		 * True if modules is owned by Transforme.
		 */
		bool Owned;
		/**
		 * Holded object.
		 */
		BaseLib::Objects::Object* Object;
	public:
		ObjectType(bool owned, BaseLib::Objects::Object* object)
			: Owned(owned), Object(object) 
		{ };
	};
	/**
	 * Type of used storage.
	 */
	typedef std::vector<ObjectType> StorageType;
public:
	/**
	 * Init input and output socket and base propoerties.
	 * @param[in] outName Name of primary output data.
	 */
	Visualiser(const std::string& outName)
		: mOutName(outName), mOutFullName("")
	{
		// set specification for output socket
		mOutputSocketAdd.SetInterface(BaseLib::Interfaces::RandomAcces::Identification);
		mOutputSocketAdd.SetPrimary(outName, OUTTYPE().GetType());

		// input socket specfication
		auto mainSocket = new BaseLib::Sockets::Socket("Input", "", BaseLib::Interfaces::RandomAcces::Identification | BaseLib::Interfaces::SequentialAcces::Identification);
		// we set requirment for main data
		mainSocket->SetPrimary("Main data", INTYPE().GetType());
		// set input socket specification
		mInputSockets.push_back(mainSocket);

		// validate output socket
		ValidateOutputSocket();
	}
	virtual ~Visualiser()
	{	
		Clear();
	}
public: // BaseLib::iModule
	void* AccessInterface(const int& interfaceId)
	{
		if (interfaceId == BaseLib::Interfaces::RandomAcces::Identification)
		{
			return dynamic_cast<BaseLib::Interfaces::RandomAcces*>(this);
		}
		return nullptr;
	}
	virtual bool CheckIntegrity()
	{
		if (GetSocket(0) == nullptr)
		{
			ReportMessage(BaseLib::iReportable::Error, "Not all source connected.");
			return false;
		}
		return GetSocket(0)->CheckIntegrity();
	}
public: // BaseLib::Interfaces::RandomAcces
	virtual size_t GetCount() const
	{
		return mStorage.size();
	}
	virtual BaseLib::Objects::Object* GetObject(const size_t& index, ExceptionHolder& ex)
	{	// all work is done in innerPrapeer we just return results .. 
		if (index < 0 || index >= mStorage.size())
		{
			return nullptr;
		}
		// mark object ( we give up ownership)
		mStorage[index].Owned = false;
		return mStorage[index].Object;
	}
	virtual void ClearOutState()
	{	// all objects are back in our ownership
		std::for_each( std::begin(mStorage), std::end(mStorage),
			[&] (std::vector<ObjectType>::reference item)
			{
				item.Owned = true;
			}
		);
	}
protected:
	/**
	 * Return full out name. Can be used to direct access to 
	 * output data.
	 * @return Full out name.
	 */
	const std::string& GetFullOutName() const
	{
		return mOutFullName;
	}
	/**
	 * Return size of data that are prepared for visualisation.
	 * @return Data size.
	 */
	size_t DataSize() const
	{
		return mStorage.size();
	}
	/**
	 * Return refrence to data with specific index. 
	 * No indexs checks are done !
	 * @param[in] index Object index.
	 * @return Reference to data.
	 */
	const INTYPE& AccesInData(size_t index)
	{	// we obtain data, recast them to required type and then return const refrence
		return *dynamic_cast<INTYPE*>( mStorage[index].Object->GetMain() );
	}
	/**
	 * Return refrence to the output data ( where Visualise should store output data ).
	 * Object index should be the same as used in AccesInData to get data.
	 * @param[in] index Object index.
	 * @return Reference to the data.
	 */
	OUTTYPE& AccesOutData(size_t index)
	{	// we return acces to the output data
		return *dynamic_cast<OUTTYPE*>( mStorage[index].Object->GetData().at(mOutFullName) );
	}
	/**
	 * Return acces to the object storage.
	 * Data required in INTYPE is under object main data.
	 * Output data can be accedsed as secondary data using the name from GetFullOutName.
	 * @see GetFullOutName
	 * @return[in] object storage
	 */
	StorageType& AccesStorage()
	{
		return mStorage;
	}
	/**
	 * Do the visualisation work.
	 */
	virtual void Visualise() = 0;
protected: // BaseLib::iModule
	virtual void innerOpen()
	{	// test that we have set the socket( for safety reasons)
		if (GetSocket(0) == nullptr)
		{
			ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
			throw std::exception();
		}
		ExceptionHolder ex;
		GetSocket(0)->Open(ex);
		ex.Rethrow();
	}
	virtual void innerPrepare()
	{
		if (GetSocket(0) == nullptr)
		{
			ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
			throw std::exception();
		}
		ExceptionHolder ex;
		GetSocket(0)->Prepare(ex);
		ex.Rethrow();

		ReportMessage(BaseLib::iReportable::Information, "Data gathering ...");

		// we known that the source can be either random or sequent
		auto sourceInterface = GetSocket(0)->GetOutputSocket().GetInterface();
		if ( (sourceInterface & BaseLib::Interfaces::RandomAcces::Identification) == BaseLib::Interfaces::RandomAcces::Identification)
		{	// random acces			
			auto acces = reinterpret_cast<BaseLib::Interfaces::RandomAcces*>
				(GetSocket(0)->AccessInterface(BaseLib::Interfaces::RandomAcces::Identification));
			assert(acces);
			// get data
			ExceptionHolder ex;
			size_t count = acces->GetCount();
			for (size_t i = 0; i < count; ++i)
			{
				mStorage.push_back(ObjectType(true, acces->GetObject(i, ex)));
				ex.Rethrow();
			}
		}
		else // sequential acces
		{
			auto acces = reinterpret_cast<BaseLib::Interfaces::SequentialAcces*>
				(GetSocket(0)->AccessInterface(BaseLib::Interfaces::SequentialAcces::Identification));
			assert(acces);
			// get data
			ExceptionHolder ex;
			while (acces->Next(ex))
			{
				ex.Rethrow();
				mStorage.push_back(ObjectType(true,acces->GetObject(ex)));
				ex.Rethrow();
			}
			// if the first Next failed check for an exception 
			ex.Rethrow();
		}
		// we can close the source here
		GetSocket(0)->Close();

		ReportMessage(BaseLib::iReportable::Information, "Visualisation ...");

		if (mStorage.size() == 0)
		{	// no data end 
			return;
		}
		// we store the name under which the data were added, and belive that for all data it will be the same
		std::begin(mStorage)->Object->Add(mOutName, new OUTTYPE(), mOutFullName);
		// add output data
		std::for_each( std::begin(mStorage)++, std::end(mStorage),
			[&] (std::vector<ObjectType>::reference item)
			{	// we can use mOutName to add .. 
				item.Object->Add(mOutName, new OUTTYPE());
			}
		);		

		// run user implemented visualisation
		Visualise();

		// mark main output data
		std::for_each( std::begin(mStorage), std::end(mStorage),
			[&] (std::vector<ObjectType>::reference item)
			{
				item.Object->SetMain(mOutName);
				// data pass
				item.Object->Pass();
			}
		);
	}
	virtual void innerClose()
	{
		if (GetSocket(0) != nullptr)
		{
			GetSocket(0)->Close();
		}
		else
		{	// on closing we dont report anything		
		}
		// delete data
		Clear();
	}
	virtual void ValidateOutputSocket()
	{	// we start with the socket that we promised we will 

		if (GetSocket(0) == nullptr)
		{	// we have no source, so add nothing
			mOutputSocketPublic.Reset();
		}
		else
		{	// add data from source socket
			mOutputSocketPublic.Copy(GetSocket(0)->GetOutputSocket());
			mOutputSocketPublic.IncreasePassCounter();
			mOutputSocketPublic.Add(mOutputSocketAdd);
		}
		// set interface
		mOutputSocketPublic.SetInterface(BaseLib::Interfaces::RandomAcces::Identification);
	}
private:
	/**
	 * Object storage.
	 */
	StorageType mStorage;
	/**
	 * Output data name.
	 * Used to create output subobjects.
	 */
	std::string mOutName;
	/**
	 * Full output data name.
	 * Can be used to access the output data.
	 */
	std::string mOutFullName;
private:
	/**
	 * Delete the stored data.
	 */
	void Clear()
	{
		std::for_each(std::begin(mStorage), std::end(mStorage),
			[&] (ObjectType& item)
			{
				if (item.Owned)
				{	// object is in our posession and we will delete it
					delete item.Object;
				}
			}
		);
		// all data deleted .. we can release 
		mStorage.clear();
	}
};

} }

#endif // baseLib_modules_visualiser_h