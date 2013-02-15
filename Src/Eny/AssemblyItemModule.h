#ifndef eny_assemblyItemModule_h
#define eny_assemblyItemModule_h

/**
 *
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Qt includes 
///
#include <QtGui/QGraphicsScene>
#include <QtGui/QGraphicsPixmapItem>

// Local includes
///
#include "MessageModel.h"
#include "ModuleHolder.h"

// BaseLibe includes
///
#include "Module.hpp"
#include "Reportable.hpp"

namespace Assembly {

// Forward declaration
///
class ItemConnection;

/**
 * Class provide graphics representation to iModule. Also accept 
 * messages from contained iModule.
 */
class ItemModule : public QObject, public QGraphicsPixmapItem, public BaseLib::iReportable
{
public:
	enum {
		Identification = 1
	};
	Q_OBJECT
public:
	/**
	 * Create new ItemModule.
	 * @param[in] module Holded module.
	 * @param[in] messageModel Message model into which add messages.
	 * @param[in] scene QGraphicsScene
	 */
	ItemModule(Logic::ModuleHolder* module, DataModels::Message::Model& messageModel, QGraphicsScene* scene);
	/**
	 * Release graphics by call SceneDestroy, and then release holded ModuleHolder.
	 * So release both graphic and logic content.
	 */
	~ItemModule();
public:
	/**
	 * Return mConnectionDeleted and set it to false.
	 * @return bool true if any connection was deleted from last call
	 */
	bool TestConnectionDeleted();
	/**
	 * Return holded module id.
	 * @return module id
	 */
	QString GetId() const throw();
	/**
	 * @return holded module
	 */
	BaseLib::iModule* GetModule() const throw();
	/**
	 * @return module holder
	 */
	Logic::ModuleHolder& GetHolder() const throw();
	/**
	 * Called when graphics representation position is changed, 
	 * update related connections.
	 */
	void OnPositionChanged();
	/**
	 * Add conection to list of connections.
	 * Before add connection try to call FindConnection and it there exist 
	 * connection between modules just AddSocket on connection.
	 * @param[in] connection New graphics representation for connection.
	 */
	void AddConnection(ItemConnection* connection);	
	/**
	 * Remove graphics representation of connection, 
	 * secure also remove connection from other ItemModule if need.
	 * Graphics representation is removed from scene.
	 * @param[in] otherSide Other side of connection.
	 * @param[in] socket Socket number.
	 */
	void RemoveConnection(ItemModule* otherSide, size_t socket);
	/**
	 * Remove graphics representation of connection, 
	 * secure also remove connection from other ItemModule if need.
	 * Graphics representation is removed from scene.
	 * @param[in] otherSide Other side of connection.
	 * @param[in] socket Socket number.
	 */
	void RemoveConnection(BaseLib::iModule* otherSide, size_t socket);
	/**
	 * Construct/destroy connection according to connection in holded module ( acces via ModuleHolder ).
	 * Work only with connection where ItemModule is as a target.
	 * @param[in] modules Acces to all modules.
	 */
	void RepairConnection(std::map<std::string, ItemModule*>& modules);
	/**
	 * Call when parent Scene is destroyd, release all graphics content.
	 */
	void OnSceneDestroy();
	/**
	 * Find connection from ItemModule to Itemmodule and return it
	 * @param[in] target Target side of conneciton.
	 * @return connection or nullptr if there is no such connection
	 */
	ItemConnection* FindConnection(ItemModule* target);
	/**
	 * Call when calculation end. 
	 * Reset pixmap (from computing state).
	 */
	void OnCalculationEnd();
public: // iReportable
	virtual void Message(BaseLib::iModule* sender, BaseLib::iReportable::eMessageType type, std::string message);
	virtual void Progress(BaseLib::iModule* sender, int min, int max, int value);
	virtual void ConnectionChanged(BaseLib::iModule* sender);
	virtual void InterfaceChanged(BaseLib::iModule* sender);
protected:
	/**
	 * Create ItemModule Graphics representation.
	 */
	void CreateGraphics();
	/**
	 * Remove socket from connection socket. If connection has no more socket then remove
	 * graphis representation of connection. Passer pointer is then deleted so after call is invalid.
	 * Is called by RemoveConnection.
	 * @param[in] connection Connection from which remove.
	 * @param[in] socket Number of socket to remove.
	 */
	void InnerRemoveConnection(ItemConnection* connection, size_t socket);
	/**
	 * Remove connectionfrom ItemModules and delete it.
	 * Use only when check that connection does not represent any logic connection.
	 * @param[in] connection Connection.
	 */
	void InnerRemoveConnection(ItemConnection* connection);
protected slots:
	/**
	 * Report that holded module interface change, and id should be revalid.
	 */
	void slInterfaceChanged();
	/**
	 * Check progress change and redraw object.
	 * @param[in] progress Progress.
	 */
	void slProgressChange(double progress);
private:
	typedef std::vector<ItemConnection*> ConnectionsType;
private:
	/**
	 * Holded module.
	 */
	Logic::ModuleHolder* mModule;
	/**
	 * Message model.
	 */
	DataModels::Message::Model& mMessageModel;
	/**
	 * Image for valid representation.
	 */
	QPixmap mPixmapValid;
	/**
	 * Image for not completly valid module.
	 */
	QPixmap mPixmapInvalid;
	/**
	 * Related connections.
	 */
	ConnectionsType mConnections;
	/**
	 * Last rendered progress.
	 */
	double mLastProgress;
	/**
	 * True if in ConnectionChanged connection
	 * was deleted.
	 */
	bool mConnectionDeleted;
};

}

#endif // eny_assemblyItemModule_h
