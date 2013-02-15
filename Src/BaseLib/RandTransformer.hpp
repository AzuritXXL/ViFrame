#ifndef baseLib_modules_randTransformer_h
#define baseLib_modules_randTransformer_h

/**
 * Contains a partialy pre-implemented class that should be used
 * as a data transformer.
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
#include "BaseProperty.hpp"
#include "ReportProperty.hpp"
#include "Exception.hpp"

namespace BaseLib {
namespace Modules {

/**
 * Template for random access transformation module. Random access
 * means that during the transformation the user has access to all data.
 * User defined transformation should be done in Transform. Keep in mind 
 * that if INTYPE == OUTTYPE the data can be transformed which means that the source
 * and ouput data are the same.
 *
 * When subclassing implement: Transform
 *
 * INTYPE - required input data type ( subclass of Objects::Variant )
 * OUTTYPE - output type ( subclass of Objects::Variant )
 */
template <typename INTYPE, typename OUTTYPE> class RandTransformer 
	: public ::BaseLib::iModule, ::BaseLib::Interfaces::RandomAcces , public ::BaseLib::Properties::Reportable
{
public:
	/**
	 * Contains information about the stored object. 
	 * Because of requirments in ::BaseLib::Interfaces::RandomAcces
	 * we need to know whether we own the objects or not so that we either delete them or not.
	 */
	struct ObjectType
	{
	public:
		/**
		 * True if the module is owned by the Transformer.
		 */
		bool Owned;
		/**
		 * Keeped object.
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
	 * Init input and output socket and base properties.
	 * @param[in] outName Name of the primary output data.
	 */
	RandTransformer(const std::string& outName)
		: mOutName(outName), mOutFullName(""), mStorage(), mTransform(false), mChangeMain(false)
	{
		// set specification for output socket
		mOutputSocketAdd.SetInterface(BaseLib::Interfaces::RandomAcces::Identification);
		mOutputSocketAdd.SetPrimary(outName, OUTTYPE().GetType());

		// input socket specfication
		auto mainSocket = new BaseLib::Sockets::Socket("Input", "", BaseLib::Interfaces::RandomAcces::Identification | BaseLib::Interfaces::SequentialAcces::Identification);
		// we set requirements for the main data
		mainSocket->SetPrimary("Main data", INTYPE().GetType());
		// set input socket specification
		mInputSockets.push_back(mainSocket);

		// properties

		// change main value to the newly added value 
		mProperties.push_back(::BaseLib::Properties::CreateReport("Change main", "True if module should change objects main to added data.", (bool&)mChangeMain, this) );

		// if the types are the same enable transforming ( changing main data in place )
		if (OUTTYPE().GetType() == INTYPE().GetType())
		{	// in and out data type are the same we can offer placement transformation
			mTransform = true; // we transform by default
			mProperties.push_back(::BaseLib::Properties::CreateReport("Transforming", "True if module should transform main data and not create new, when checked Change main is ignored.", (bool&)mTransform, this) );
		}
		// validate output socket
		ValidateOutputSocket();
	}
	~RandTransformer()
	{	// just to be sure.. 
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
	{	// we require all sources connected
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
public: // ::BaseLib::Properties::Reportable
	virtual void OnChange(Properties::BaseProperty* prop)
	{	// property change -> .. revalidate OutputSocket 
		ValidateOutputSocket();
		// and report change
		EmitTargetControl();
	}
protected:
	/**
	 * Returns full out name. Can be used to direct access to 
	 * the output data.
	 * @return Full out data name
	 */
	const std::string& GetFullOutName() const
	{
		return mOutFullName;
	}
	/**
	 * Returns size of data that are prepared for transformation.
	 * @return Data size
	 */
	size_t DataSize() const
	{
		return mStorage.size();
	}
	/**
	 * Returns refrence to data with specified index.
	 * Becouse of optimization, no index check is done ! 
	 * @param[in] data Index
	 * @return Reference to data
	 */
	const INTYPE& AccesInData(size_t index)
	{	// we obtain data, recast them to the required type and then return const refrence
		INTYPE& tempValue = *dynamic_cast<INTYPE*>( mStorage[index].Object->GetMain() );
		return tempValue;
	}
	/**
	 * Returns refrence to the output data ( where Transform should store the output ).
	 * The object index should be the same as used in AccesInData to get the data. 
	 * When transforming it is posible that AccesInData and AccesOutData return refrence to the same
	 * data.
	 * No index check is done ! 
	 * @param[in] object Index
	 * @return Reference to data
	 */
	OUTTYPE& AccesOutData(size_t index)
	{	// we return acces to output data
		OUTTYPE* returnValue;
		if (mTransform)
		{	// return main data .. 
			returnValue = dynamic_cast<OUTTYPE*>( mStorage[index].Object->GetMain() );
		}
		else
		{	// return new output data .. ( data exist we create them in innerPrepare )
			returnValue = dynamic_cast<OUTTYPE*>( mStorage[index].Object->GetData().at(mOutFullName) );
		}
		// return dereferenced data 
		return *returnValue;
	}
	/**
	 * Returns acces to the object's storage.
	 * Data required in INTYPE is accesible under object main data. 
	 * Output data are stored under name which can be obtained from GetFullOutName.
	 * @see GetFullOutName 
	 * @return[in] object Storage
	 */
	StorageType& AccesStorage()
	{
		return mStorage;
	}
	/**
	 * Data transformation. Do user defined transformation.	 
	 */
	virtual void Transform() = 0;
protected: // BaseLib::iModule
	virtual void innerOpen()
	{	// test that we set the socket ( to be sure)
		if (GetSocket(0) == nullptr)
		{
			ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
			throw BaseLib::Exception();
		}
		ExceptionHolder ex;
		GetSocket(0)->Open(ex);
		ex.Rethrow();
	}
	virtual void innerPrepare()
	{	// we should have socket .. but it does not cost much to test it .. 
		if (GetSocket(0) == nullptr)
		{
			ReportMessage(BaseLib::iReportable::Error, "Source socket has value nullptr.");
			throw BaseLib::Exception();
		}
		ExceptionHolder ex;
		GetSocket(0)->Prepare(ex);
		ex.Rethrow();

		ReportMessage(BaseLib::iReportable::Information, "Data gathering ...");

		// we known that source can be random or sequential
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
			// if first Next failed check for exception 
			ex.Rethrow();
		}
		// we can close the source here
		GetSocket(0)->Close();

		ReportMessage(BaseLib::iReportable::Information, "Transforming ...");

		if (mStorage.size() == 0)
		{	// no data -> end 
			return;
		}

		if (mTransform)
		{	// dont add any data .. 
		}
		else
		{	// add output data
			// we store the name under which data were added, and belive that for all data it will be the same
			std::begin(mStorage)->Object->Add(mOutName, new OUTTYPE(), mOutFullName);
			// add output data
			std::for_each( std::begin(mStorage)++, std::end(mStorage),
				[&] (std::vector<ObjectType>::reference item)
				{	// we can use mOutName to add .. 
					item.Object->Add(mOutName, new OUTTYPE());
				}
			);
		}

		// run user implemented visualisation
		Transform();

		if (mChangeMain)
		{	// set main and pass
			// mark main output data
			std::for_each( std::begin(mStorage), std::end(mStorage),
				[&] (std::vector<ObjectType>::reference item)
				{	// for setting main we use pure name .. 
					item.Object->SetMain(mOutName);
					// data pass
					item.Object->Pass();
				}
			);
		}
		else
		{	// just pass main 
			std::for_each( std::begin(mStorage), std::end(mStorage),
				[&] (std::vector<ObjectType>::reference item)
				{
					item.Object->Pass();
				}
			);
		}
	}
	virtual void innerClose()
	{
		if (GetSocket(0) != nullptr)
		{
			GetSocket(0)->Close();
		}
		else
		{	// on closing we don't report anything ( errors )
		}
		Clear();
	}
	virtual void ValidateOutputSocket()
	{	// we start with socket that we promise we will have
		mOutputSocketPublic = mOutputSocketAdd;
		if (GetSocket(0) == nullptr)
		{	// we have no source, so add nothing			
		}
		else
		{	// add data from source socket
			if (mTransform)
			{	// we just transform the main data
/*				if (mChangeMain)
				{	// main data are transformed, and should be marked as main .. but they already are .. 
*/					mOutputSocketPublic.Copy(GetSocket(0)->GetOutputSocket());
					mOutputSocketPublic.IncreasePassCounter();
/*				}
				else
				{	// main data transformed, but not marked as main, nonsense
					mOutputSocketPublic.Copy(GetSocket(0)->GetOutputSocket());
					mOutputSocketPublic.IncreasePassCounter();
				}
*/			}
			else
			{	// we add to our data data from source
				// we have out data ( we add ) stored in OutputSocketDescription as Primary
				if (mChangeMain)
				{	// add data and set them as main ( like visualisation )
					mOutputSocketPublic.Copy(GetSocket(0)->GetOutputSocket());
					mOutputSocketPublic.IncreasePassCounter();
					mOutputSocketPublic.Add(mOutputSocketAdd);
				}
				else
				{	// add data and dont set them as main 
					mOutputSocketPublic.Copy(GetSocket(0)->GetOutputSocket());
					mOutputSocketPublic.IncreasePassCounter();
					// now add data
					mOutputSocketPublic.AddData(mOutputSocketAdd.GetPrimaryName(), mOutputSocketAdd.GetPrimaryType());
				}
			}
		}
		// output interface as always .. 
		mOutputSocketPublic.SetInterface(BaseLib::Interfaces::RandomAcces::Identification);
	}
private:
	/**
	 * Output data name. Without level specification.
	 * Used to generate output objects.
	 */
	std::string mOutName;
	/**
	 * Output data name. With level specification, 
	 * can be used to acces output objects.
	 */
	std::string mOutFullName;
	/**
	 * Object storage.
	 */
	StorageType mStorage;
	/**
	 * True if objects main data are changed. 
	 */
	bool mTransform;
	/**
	 * False if we dont change objects main data.
	 * We add secondary data.
	 */
	bool mChangeMain;
private:
	/**
	 * Go through all the data and delete those in our ownership.
	 */
	void Clear()
	{
		std::for_each(std::begin(mStorage), std::end(mStorage),
			[&] (ObjectType& item)
			{
				if (item.Owned)
				{	// object is in our posession we will delete it
					delete item.Object;
				}
			}
		);
		// all data deleted .. release it
		mStorage.clear();
	}
};


} }

#endif // baseLib_modules_randTransformer_h