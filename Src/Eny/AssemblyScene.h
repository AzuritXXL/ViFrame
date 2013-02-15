#ifndef eny_assemblyScene_h
#define eny_assemblyScene_h

/**
 *
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart library includes
///
#include <map>
#include <vector>

// Qt includes 
///
#include <qaction.h>
#include <qobject.h>
#include <QtGui\QGraphicsScene.h>

// Local includes
///
#include "LibraryStorage.h"
#include "MessageModel.h"

namespace Assembly {

// Forward declaration
///
class ItemModule;
class ItemConnection;

/**
 * Specialized scene for Assembly objects.
 *
 */
class Scene : public QGraphicsScene
{
	Q_OBJECT
public:
	typedef std::map<std::string, ItemModule*> ModulesType;
public:
	Scene(Logic::LibraryStorage& libraries, DataModels::Message::Model& messages, QWidget *parent);
	~Scene();
public:
	/**
	 * Set scene size minimal size.
	 * Size is determined from stored modules.
	 */
	void SetMinSize();
	/**
	 * Delete all modules. From scene and from memory.
	 */
	void Clear();
	/**
	 * Switch content from argument to inner data storage of Scene.
	 * Original data are released.
	 * @param[in] counter New value of counter.
	 * @param[in] modules Value for mModules.
	 */
	void RetakeData(size_t& counter, ModulesType& modules);
	/**
	 * @return stored modules
	 */
	const ModulesType& GetModules() const;
	/**
	 * Return inner counter, used to create names for modules.
	 * @return counter
	 */
	const size_t& GetCounter() const;
protected:
	/**
	 * Show context menu for module.
	 * @param[in] module Module item under cursor.
	 * @param[in] position Click position.
	 */
	void ContextModule(ItemModule* module, const QPoint& position);
	/**
	 * Show context menu for connection.
	 * @param[in] connection Connection item under cursor.
	 * @param[in] position Click position
	 */
	void ContextConnection(ItemConnection* connection,const QPoint& position);
	/**
	 * Call when library is drop. Create new module.
	 */
	void LibraryDrop(QGraphicsSceneDragDropEvent *event);
	/**
	 * Call when module is drop. Perform module move, or create Connection dialog.
	 */
	void ModuleDrop(QGraphicsSceneDragDropEvent *event);
	/**
	 * Prepare data and do module drag operation.
	 */
	void DragModuleItem(QGraphicsSceneMouseEvent* event, ItemModule* item);
	/**
	 * Show dialog with properties of module.
	 * @param[in] itemModule Module representation thath contain module for which show dialog.
	 */
	void ShowProperties(ItemModule* itemModule);
	/**
	 * Show dialog with module details.
	 * @param[in] itemModule Module representation thath contain module for which show dialog.
	 */
	void ShowDetail(ItemModule* itemModule);
	/**
	 * Show connection dialog.
	 * @param[in] source Source module representation.
	 * @param[in] target Target module representation.
	 */
	void ShowConnection(ItemModule* source, ItemModule* target);
	/**
	 * Delete module (and his representation) from scene and from memory. Passed
	 * pointer become invalid.
	 * @param[in] itemModule Module representation to delete.
	 */
	void Delete(ItemModule* itemModule);
	/**
	 * Create module from library.
	 * @param[in] id Library id
	 * @param[in] Position Position for new module rperesentation.
	 */
	void Create(const QString& id, const QPointF& position);
signals:
	/**
	 * Emit when running computing on certain module is required.
	 * @param[in] itemModule Module on which run computing.
	 */
	void siRunModule(Assembly::ItemModule* itemModule);
protected:
	void mousePressEvent(QGraphicsSceneMouseEvent* event);
    void dragEnterEvent(QGraphicsSceneDragDropEvent* event);    
	void dragMoveEvent(QGraphicsSceneDragDropEvent* event);
    void dropEvent(QGraphicsSceneDragDropEvent *event);
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* event);
private:
	/**
	 * Counter used to generate new names for modules.
	 */
	size_t mCounter;
	/**
	 * Store module representations.
	 */
	ModulesType mModules;
	/**
	 * Parent widget.
	 */
	QWidget* mParent;
	/**
	 * Reference to libraries.
	 */
	Logic::LibraryStorage& mLibraries;
	/**
	 * Message model.
	 */
	DataModels::Message::Model& mMessages;
private:
	QAction* actRun;
	QAction* actProperties;
	QAction* actDetail;
	QAction* actConnections;
	QAction* actDelete;
	QAction* actDisconnect;
};

}

#endif // eny_assemblyScene_h