#ifndef merger_main_h
#define merger_main_h

/**
 * Contains main module class definition. It's ment to be used
 * as a part of visualisation pathway.
 *
 * @author 
 * @version 1.0
 * @date 1.5.2012
 */

// BaseLib includes
///
#include "Module.hpp"
#include "ReportProperty.hpp"
#include "SequentialAcces.hpp"

/**
 * Class subclass base module class and add abylity to merge more data
 * source into one. Provide check of sources and match atribute into 
 * common output name.
 */
class Merger : public BaseLib::iModule, BaseLib::Interfaces::SequentialAcces, BaseLib::Properties::Reportable
{
public:
	Merger();
public: // BaseLib::iModule
	virtual bool Connect(iModule* module, size_t socket);
	virtual const int GetOutputInterfaces();
	void* AccessInterface(const int& interfaceId);
	virtual bool PrepareForConnection();
	virtual bool CheckIntegrity();
public: // BaseLib::Interfaces::SequentialAcces
	virtual bool Next(BaseLib::ExceptionHolder& ex);
	virtual BaseLib::Objects::Object* GetObject(BaseLib::ExceptionHolder& ex);
public: // BaseLib::Properties::Reportable
	virtual void OnChange(BaseLib::Properties::BaseProperty* prop);
protected:
	/**
	 * Dictionary. Under name _PRIMARY store name for new main value.
	 */
	typedef std::map<std::string, std::string> InnerNameMapperType;
	/**
	 * Type for name mapping from source to target.
	 */
	typedef std::vector< InnerNameMapperType > NameMapperType;
	/**
	 * Describe data structure for merging socket specification.
	 */
	struct InnerMergerSocket
	{
		/**
		 * Data level.
		 */
		int Level;
		/**
		 * Number inSockets which use this.
		 */
		int NumberOfUsage;
		/**
		 * Data type.
		 */
		int Type;
	public:
		/**
		 * Create new InnerMergerSocket, NumberOfUsage is se to 1.
		 * @param[in] level Data leve.
		 * @param[in] type Type.
		 */
		InnerMergerSocket(int level, int type)
			: Level(level), NumberOfUsage(1), Type(type)
		{ };
	};
	typedef std::map< std::string, std::vector<InnerMergerSocket> > MergerSocket;
protected:
	/**
	 * Number of input socket.
	 */
	int mSocketCount;
	/**
	 * Index of active socket ( module ).
	 */
	int mActiveModule;
	/**
	 * Name mapper. Map name from input socket to output.
	 */
	NameMapperType mNameMapper;
	/**
	 * Pass count for objects.
	 */
	int mPassCount;
protected: // BaseLib::iModule
	virtual void innerOpen();
	virtual void innerPrepare();
	virtual void innerClose();
	virtual void ValidateOutputSocket();
private:
	/**
	 * Count connected input sockets.
	 * @param[in] number of connected
	 */
	int CountConnected() const;
	/**
	 * Create string name from param.
	 * @param[in] level Level index.
	 * @param[in] name Name.
	 * @return string level << '.' << name
	 */
	std::string CreateName(int level, const std::string& name) const;
	/**
	 * Add data from inSocket to outSocket.
	 * Translation information are stored in translationMap.
	 * @param[in] inSocket Added data.
	 * @param[in] outSocket Original data.
	 * @param[in] translationMap Translation map from inSocket to outSocket.
	 */
	void MergeSecondary(const BaseLib::Sockets::Socket::DataType& inSocket, MergerSocket& outSocket, InnerNameMapperType& translationMap) const;

};

extern "C" _DYNLINK void* CreateModule();

extern "C" _DYNLINK void DeleteModule(void*);

extern "C" _DYNLINK const BaseLib::Description* GetDescription();

#endif // merger_main_h