#ifndef eny_workThread_h
#define eny_workThread_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standrtt libraries include
///
#include <vector>

// Qt includes
///
#include <qlist.h>
#include <qthread.h>

// Local include
///
#include "MessageItem.h"

// Forward declarations
///
namespace BaseLib
{
	class iModule;
	namespace Objects 
	{
		class Object;
	}
	namespace Interfaces
	{
		class RandomAcces;
		class SequentialAcces;
	}
}
namespace DataModels
{
	namespace Output
	{
		class DataItem;
		class VirtualItem;
	}
}

namespace Logic {

/**
 * After work is done, object from mObjects should be retrieve, 
 * and mObjects should be clear, becouse stored object
 * are delete on the begin on each run.
 * 
 * If there is no data ( GetObjects.size() == 0) there is no need to releas 
 * any data. Specialy mRoot (GetRoot) will stay at nullptr value.
 */
class WorkThread : public QThread
{
	Q_OBJECT
public:
	typedef std::vector<BaseLib::Objects::Object*> ObjectsType;
	typedef std::vector<DataModels::Output::DataItem*> DataItemTypes;
public:
	WorkThread();
	~WorkThread();
public:
	/**
	 * @return refrence to model items
	 */
	DataItemTypes& GetDataItems();
	/**
	 * @return refrence to key names
	 */
	QList<QString>& GetKeys();
	/**
	 * @return refrence to key names which belong to integral data
	 */
	QList<QString>& GetIntegralKeys();
	/**
	 * Return pointer to root model view item.
	 * WorkThread lose pointer after return, so to prevent
	 * memory leak caller must store and release returned pointer.
	 * @return pointer to root
	 */
	DataModels::Output::VirtualItem* GetRoot();
	/**
	 * @return stored objects
	 */
	ObjectsType& GetObjects();
	/**
	 * Set main run module.
	 * @param[in] module
	 */
	void SetModule(BaseLib::iModule* module);
	/**
	 * @return main modul
	 */
	BaseLib::iModule* GetModule();
signals:
	/**
	 * Signal which ThreadWorker use to report messages.
	 * @param[in] message to show
	 */
	void siAddMessage(const DataModels::Message::Item& dataMessage);
private:
	/**
	 * Main module which will be run.
	 */
	BaseLib::iModule* mModule;
	/**
	 * Objects storage.
	 */
	ObjectsType mObjects;
	/**
	 * Store data items.
	 */
	DataItemTypes mDataItems;
	/**
	 * Root item for output model.
	 */
	DataModels::Output::VirtualItem* mRoot;
	/**
	 * Names of data
	 */
	QList<QString> mKeys;
	/**
	 * Names of data that has integral type.
	 */
	QList<QString> mIntegralKeys;
private: // QThread
	void run();
private:
	/**
	 * Delete data in mObjects
	 */
	void Clear();
	/**
	 * Read data from random acces interface.
	 * @param[in] random acces
	 */
	void ReadRandom(BaseLib::Interfaces::RandomAcces* random);
	/**
	 * Read data from sequential acces interface.
	 * @param[in] sequential acces
	 */
	void ReadSequential(BaseLib::Interfaces::SequentialAcces* sequential);
	/**
	 * Create simple data model, used data from mOjects.
	 * Model root is stored in mRoot, any previous data in mRroot 
	 * will be lost.
	 */
	void CreateModel();
};

}

#endif // eny_workThread_h