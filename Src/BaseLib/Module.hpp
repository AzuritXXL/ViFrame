#ifndef baseLib_module_h
#define baseLib_module_h

/**
 * Contains base module interface.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes 
///
#include <vector>
#include <string>

// Local includes
///
#include "Main.hpp"
#include "Reportable.hpp"
#include "Socket.hpp"
#include "BaseProperty.hpp"
#include "Exception.hpp"

namespace BaseLib {

class iReportable;
struct Description;

/**
 * Module base interface.
 *
 * Module at the beginning does not provide any output socket specification after setting mOutputSocketAdd, 
 * ValidateOutputSocket should be called to store the final output socket description into mOutputSocketPublic.
 * 
 * Pure virtual function are: AccessInterface, CheckIntegrity, innerOpen, innerPrepare, innerClose, ValidateOutputSocket
 */
class iModule
{
public:
	typedef std::vector<Sockets::Socket*> SocketsType;
	typedef std::vector<Properties::BaseProperty*> PropertyType;
	/**
	 * States for CreateTopologyOrder.
	 */
	enum eTopologyStates
	{
		OpenState
		,CloseState
	};
	typedef std::map<iModule*, eTopologyStates> TopologicalOrderStateType;
public:
	iModule();
	virtual  ~iModule();
public:
	/**
	 * Return module id.
	 * @return module id
	 */
	const std::string& GetId() const;
	/**
	 * Set module id.
	 * @param[in] module id
	 */
	void SetId(const std::string& id);
	/**
	 * Set module Reportable class.
	 * @param[in] Reportable class
	 */
	void SetReportable(iReportable* Reportable);
	/**
	 * Get module Reportable class.
	 * @param[in] Reportable class
	 */
	iReportable* GetReportable();
	/**
	 * Return requirements for input sockets.
	 * @return input socket description
	 */
	const SocketsType& GetInputSockets();
	/**
	 * Return output sockets data description.
	 * @return output socket description
	 */
	const Sockets::Socket& GetOutputSocket() const;
	/**
	 * Return pointer to required interface. 
	 * If interface is not suported return nullptr
	 * @param[in] interfaceId Interface id ( value from interface class Identification ).
	 * @return pointer to interface or nullptr
	 */
	virtual void* AccessInterface(const int& interfaceId) = 0;
	/**
	 * Get module properties list.
	 * @return properties
	 */
	const PropertyType& GetProperties();
	/**
	 * Return pointer to property. Return nullptr if index is out of range.
	 * @param[in] index Property index.
	 * @return property or nullptr
	 */
	Properties::BaseProperty* GetProperty(size_t index);
	/**
	 * Return true if module is prepare to be connected as a source.
	 * Is ment to prevent connecting as a source until
	 * enough module sources are connected to determine source output interface.
	 * @return true if module is prepare
	 */
	virtual bool PrepareForConnection();
	/**
	 * Connect to this module another module, on specified socket.
	 * To disconnect module on socket call with module = nullptr.
	 * If socket is out of range false is returned.
	 * Call ValidateOutputSocket, ReportableConnectionChanged and EmitTargetControl
	 * @param[in] module Source module.
	 * @param[in] socket Socket number.
	 * @return true if connection accepted
	 */
	virtual bool Connect(iModule* module, size_t socket);
	/**
	 * Call when, module is connect to some other module as a source.
	 * Does not and should not call ReportConnectionChanged which is called from source 
	 * module Connect method.
	 * When socket is out of range false is returned.
	 * @param[in] module Module to which are we connecter.
	 * @param[in] socket Socket on that we are connected.
	 * @return true if everything goes well
	 */
	virtual bool OnConnect(iModule* module, size_t socket);
	/**
	 * Call on disconnect.
	 * Does not and should not call ReportableConnectionChanged which is called from source 
	 * module Connect method.
	 * @param[in] module Module from which we are disconnected.
	 */
	virtual void OnDisconnect(iModule* module);
	/**
	 * Emit ( should be called ) when source change, module should
	 * recheck socket requirements, and if they are no longer satisfying
	 * then disconnect from source.
	 * Call ValidateOutputSocket, ReportableConnectionChanged,
	 * ReportInterfaceChanged and EmitTargetControl 
	 * if socket is changed.
	 */
	virtual void OnSourceChange();
	/**
	 * Remove all in-out connection of module.
	 * Function calls on OnDisconnect on sources
	 * and does not emit call of ReportConnectionChanged.
	 */
	virtual void CutOff();
	/**
	 * Some module report that he is beeing cutting of, 
	 * so we disconect him. But we dont report anything except
	 * ReportableInterfaceChanged.
	 */
	virtual void OnCutOff(iModule* module);
	/**
	 * Return module which is connected as a source on socket. Is socket index
	 * is out of bound nullptr is returned.
	 * @param[in] socket Socket number.
	 * @return pointer to module or nullptr
	 */
	virtual iModule* GetSocket(const size_t& socket);
	/**
	 * Return module which is connected as a source on socket. Is socket index
	 * is out of bound nullptr is returned.
	 * @param[in] socket Socket number.
	 * @return pointer to module or nullptr
	 */
	virtual const iModule* GetSocket(const size_t& socket) const;
	/**
	 * Last chance for module to check that everything is allright.
	 * @return true if module is prepara for work
	 */
	virtual bool CheckIntegrity() = 0;
	/**
	 * Open module for work. If called multiple times, 
	 * only first calling has some efect.
	 * @param[out] ex Store throwed exception.
	 */
	void Open(::BaseLib::ExceptionHolder& ex);
	/**
	 * Prepare module for work. If called multiple times, 
	 * only first calling has some efect.
	 * @param[out] ex Store throwed exception.
	 */
	void Prepare(::BaseLib::ExceptionHolder& ex);
	/**
	 * Close module after work. If called multiple times, 
	 * only first calling has some efect.
	 */
	void Close();
	/**
	 * Connect module on socket, call OnConnect on source. ( or OnDisconnect on last source ).
	 * Connection is always accepted. Use for assembly load. 
	 * @param[in] module Source module.
	 * @param[in] socket Socket number.
	 * @return false if socket out of range
	 */
	virtual bool ConnectPure(iModule* module, size_t socket);
	/**
	 * Validate module after calling ConnectPure. Mainly check requirments.
	 * Do not call any other function of source modules. Caller must secure 
	 * calling ModuleValidation on modules in propoer order ( topological order ).
	 */
	void ModuleValidation();
	/**
	 * Return topological order of modules in graph made from connections. To full function must be
	 * called on all iModules that are not source for anyone.
	 * @param[in,out] states State of graph, pass empty on begining.
	 * @param[out] topological order
	 * @return false if there is a cycle
	 */
	bool CreateTopologyOrder(TopologicalOrderStateType& states, std::vector<iModule*>& topologyOrder);
	/**
	 * Search for existing connection from source to this module.
	 * @param[in] source Source module.
	 * @return true if connection exist
	 */
	bool FindConnectionFromModule(iModule* source);
	/**
	 * Report module, that some of his properties failed to load on first time
	 * and that we are about to load them for secont time. In time of this call
	 * are all sources of module in valid state.
	 */
	virtual void PropertyLoadSecondWave();
protected:
	/**
	 * Module properties.
	 */
	PropertyType mProperties;
	/**
	 * Describe added/changed items on output objects
	 * via this module.
	 */
	Sockets::Socket mOutputSocketAdd;
	/**
	 * Output socket description used to present to outside word.
	 * Is constructed from mOutputSocketAdd and additional info 
	 * gathered from source modules. In ValidateOutputSocket method.
	 */
	Sockets::Socket mOutputSocketPublic;
	/**
	 * Input sockets description.
	 */
	SocketsType mInputSockets;
	/**
	 * True if module is in valid state. Invalid
	 * state is only after ConnectionPure.
	 */
	bool mValid;
protected:
	/**
	 * Called only once by module. 
	 * All resources and memory should be allocated here.
	 * Function can safely throw BaseLib::Exception and std::exception.
	 */
	virtual void innerOpen() = 0;
	/**
	 * Called only once by module. 
	 * Time consuming prepare operation should be done here.
	 * Function can safely throw BaseLib::Exception and std::exception.
	 */
	virtual void innerPrepare() = 0;
	/**
	 * Called only once by module.
	 * Should release resources and memory.
	 * innerClose should never throw exception.
	 */
	virtual void innerClose() = 0;
	/**
	 * Takes data from mOutputSocketAdd 
	 * and together with information from output socket of 
	 * source module create final module description 
	 * which will be stored in mOutputSocketPublic.
	 * Can be called when modules, that not satisfy socket requirments are connected. 
	 * In that case mValid is false.
	 * 
	 */
	virtual void ValidateOutputSocket() = 0;
	/**
	 * Emit socket requirments control in target module.
	 * If module is in valid state. ( mValid == true )
	 * ( Module that have us as a source )
	 */
	void EmitTargetControl();
	/**
	 * If report class is set report message.
	 * @param[in] type Message type.
	 * @param[in] message Text message.
	 */
	void ReportMessage(iReportable::eMessageType type, std::string message);
	/**
	 * If report class is set report progress. 
	 * @param[in] min Min (start) progress value.
	 * @param[in] max Max (end) progress value.
	 * @param[in] value Actual progress value.
	 */
	void ReportProgress(int min, int max, int value);
	/**
	 * If report class is set report connection changed.
	 */
	void ReportConnectionChanged();
	/*
	 * If report class is set report possible interface.
	 */
	void ReportInterfaceChanged();
private:
	/**
	 * Module id.
	 */
	std::string mModuleId;
	/**
	 * Hold target modules, to call OnSourceChange
	 * on them if necesarly.
	 */
	std::vector<iModule*> mTargets;
	/**
	 * Is module open ?
	 */
	bool mOpen;
	/**
	 * Is module prepare ?
	 */
	bool mPrepare;
	/**
	 * Pointer to Reportable class.
	 */
	iReportable* mReportable;
};

}

/**
 * Return created instance of modules class.
 * @return iModule based class
 */
extern "C" _DYNLINK void* CreateModule();

/**
 * Delete module.
 * @param[in] module to delete
 */
extern "C" _DYNLINK void DeleteModule(void*);

/**
 * Return const pointer to module description.
 * @return description
 */
extern "C" _DYNLINK const BaseLib::Description* GetDescription();

#endif // baseLib_module_h