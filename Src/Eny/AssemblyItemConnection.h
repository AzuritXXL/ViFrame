#ifndef eny_assemblyItemConnection_h
#define eny_assemblyItemConnection_h

/**
 *
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standarty library includes
///
#include <vector>

// Qt Includes
///
#include <QGraphicsScene>
#include <QGraphicsItemGroup>

namespace Assembly {

class ItemModule;

/**
 * Class for graphics representation of connection between modules.
 */
class ItemConnection : public QGraphicsItemGroup
{
public:
	enum {
		Identification = 2
	};
public:
	/**
	 * Create new item for connection.
	 * @param[in] source Source module representation.
	 * @param[in] target Target module representation.
	 * @param[in] socket Number of used socket.
	 * @param[in] scene QGraphicsScene.
	 */
	ItemConnection(ItemModule* source, ItemModule* target, size_t socket, QGraphicsScene* scene);
	/**
	 * Remove connection item from scene.
	 */
	~ItemConnection();
public:
	/**
	 * Return source module representation.
	 * @return module representation
	 */
	ItemModule* GetSource() const throw();
	/**
	 * Return target module representation.
	 * @return module representation.
	 */
	ItemModule* GetTarget() const throw();
	/**
	 * Add socket to connection list.
	 * @param[in] socket Socket number.
	 */
	void AddSocket(const size_t& socket);
	/** 
	 * Remove socket from connection list.
	 * @param[in] socket Socket number.
	 * @return true there is no other connection
	 */
	bool RemoveSocket(const size_t& socket);
	/**
	 * Update connection to fit to the actual positon of source and target.
	 */
	void UpdateConnection();
	/**
	 * Return refrence to sockets that are represented by item connections.
	 * @return acces to socket
	 */
	const std::vector<size_t>& GetSockets() const;
private:
	/**
	 * Pointer to source module.
	 */
	ItemModule* mSource;
	/**
	 * Pointer to target module.
	 */
	ItemModule* mTarget;
	/**
	 * Sockets represented by connection.
	 */
	std::vector<size_t> mSockets;
	/**
	 * Lines which create connection item.
	 */
	std::vector<QGraphicsLineItem*> mChildrens;
private:
	/**
	 * Create new lines to mChildren.
	 */
	void CreateConnection();
	/**
	 * Delete and release all items in mChildren.
	 */
	void DeleteConnection();
};

};

#endif // eny_assemblyItemConnection_h