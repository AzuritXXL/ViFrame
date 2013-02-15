#ifndef uniqStorage_Module_h
#define uniqStorage_Module_h

// Std includes
#include <map>

// BaseLib includes
///
#include "Object.hpp"
#include "Vector.hpp"
#include "Module.hpp"
#include "SequentialAcces.hpp"
#include "RandomAcces.hpp"

class Uniq : public BaseLib::iModule, public BaseLib::Interfaces::RandomAcces
{
public:
	Uniq();
protected:
	/**
	 * Load data from source and make shure that they are uniq.
	 */
	void LoadData();
public: // Interfaces::RandomAcces
	virtual size_t GetCount() const;
	virtual BaseLib::Objects::Object* GetObject(const size_t& index, BaseLib::ExceptionHolder& ex);
	virtual void ClearOutState() throw();
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
	struct Record
	{
		/**
		 * Object age.
		 */
		int Age;
		/**
		 * Index of object in storage.
		 */
		int Position;
		/**
		 * Default ctor.
		 */
		Record() : Age(-1) , Position(-1) {};
		/**
		 * Data ctor.
		 */
		Record(int age, int position) : Age(age), Position(position) {};
	};
	struct ObjectType
	{
		/**
		 * True if owned.
		 */
		mutable bool Owned;
		/**
		 * Holded object.
		 */
		BaseLib::Objects::Object* Object;
		/**
		 * Ctor.
		 */
		ObjectType() 
			: Owned(true) , Object(nullptr) { };
		/**
		 * Ctor with data.
		 */
		ObjectType(BaseLib::Objects::Object* object)
			: Owned(true) , Object(object) { };
		/**
		 * Copy ctor.
		 */
		ObjectType(const ObjectType& object)
		{	// take ownership
			Owned = true;
			object.Owned = false;
			// copy object
			Object = object.Object;
		}
		/**
		 * Dtor;
		 */
		~ObjectType()
		{
			// if we owe object, delete it
			if (Owned && Object != nullptr)
			{
				delete Object;
			}
		}

	};
private:
	/**
	 * Acces to source module.
	 */
	BaseLib::Interfaces::SequentialAcces* mAcces;
	/**
	 * Used to register 
	 */
	std::map<std::string, Record> mRegister;
	/**
	 * Object storage. First value represent ownership, if 
	 * true object is owned by module.
	 */
	std::vector<ObjectType> mStorage;
	/**
	 * If true object with same name and lowe id are removed.
	 */
	bool mRemoveLower;
};


#endif // uniqStorage_Module_h