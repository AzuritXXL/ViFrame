#ifndef baseLib_sockets_socket_h
#define baseLib_sockets_socket_h

/**
 * Contains socket class that describes modules socket.
 *
 * @author
 * @version 1.0
 * @date 1.5.2012
 */

// Standart library includes
///
#include <map>

// Local includes
///
#include "Object.hpp"

namespace BaseLib {

// Forward declaration
///
class iModule;
	
namespace Sockets {

/**
 * Describes module socket and holds connected iModule. It can be used for input as well as output socket.
 * When used as an output socket description, then describe what data is module 
 * undertake to provide. 
 * It also provides static method for requirments check.
 * mModule member variable should be accessed only from module which owns the Socket, in
 * case when Socket is used to describe output module, the mModule is not used.
 */
class Socket
{
public:
	/**
	 * Describes a single atribute, which is subobject in socket.
	 */
	struct Attribute
	{
		/**
		 * Level when add.
		 */
		int Level;
		/**
		 * Type.
		 */
		int Type;
	public:
		Attribute() 
			: Level(-1), Type(0)
		{ };
		Attribute(int level, int type)
			: Level(level), Type(type)
		{ };
	public:
		bool operator== (const Attribute& right) const
		{
			return Level == right.Level && Type == right.Type;
		}
	};
	/**
	 * Describes primary data for the socket.
	 */
	struct PrimaryType
	{
	public:
		/**
		 * Atribute's pure name. 
		 */
		std::string Name;
		/**
		 * Describe primary.
		 */
		Attribute Att;
	public:
		PrimaryType()
			: Name(""), Att()
		{ };
		PrimaryType(const std::string& name, int level, int type)
			: Name(name), Att(level, type)
		{ };
	};
	/**
	 * For one name it holds the list of atributes that are store under that name.
	 */
	typedef std::map<std::string, std::vector<Attribute> > DataType;
public:
	Socket();
	/**
	 * 
	 * @param[in] name Socket name.
	 * @param[in] description Socket description.
	 * @param[in] socInterface Socket interface.
	 */
	Socket(const std::string& name, const std::string& description, int socInterface);
public:
	/**
	 * Return socket specified interface. 
	 * @return Interface specification.
	 */
	int GetInterface() const;
	/**
	 * Set specified interface.
	 * @param[in] Interface.
	 */
	void SetInterface(int inter);
	/**
	 * Return socket name.
	 * @return socket name
	 */
	const std::string& GetName() const;
	/**
	 * Return socket description.
	 * @return socket description
	 */ 
	const std::string& GetDescription() const;
	/**
	 * Set socket primary data. First param is the name 
	 * of the primary data and the second is the requirement ( data identification ).
	 * If any primary is already set, it's overwritten.
	 * When used to specify requirements, primary name value is ignored.
	 * Data level is obtained from Socket class.
	 * @param[in] name Primary name.
	 * @param[in] requirement Requirement (values from Objects::Object::DataType).
	 */
	void SetPrimary(const std::string& name, int requirement);
	/**
	 * Set socket primary data. First param is the name 
	 * of the primary data and the second is the requirement ( data identification ).
	 * If any primary is already set, it's overwritten.
	 * When used to specify requirements, primary name value is ignored.
	 * @warning Use with causion.
	 * @param[in] name Primary name.
	 * @param[in] level Primary level.
	 * @param[in] requirement Requirement (values from Objects::Object::DataType).
	 */
	void SetPrimary(const std::string& name, int level, int requirement);
	/**
	 * Add record for secondary data.
	 * If record witch the same name name already exist, it is overwritten.
	 * Data level is obtained from Socket class.
	 * @param[in] name Data name.
	 * @param[in] requirement Requirement (values from Objects::Object::DataType).
	 */
	void AddData(const std::string& name, int requirement);
	/**
	 * Add record for secondary data.
	 * If record witch the same name name already exist, it is overwritten.
	 * @warning Use with causion.
	 * @param[in] name Data name.
	 * @param[in] level Data level.
	 * @param[in] requirement Requirement (values from Objects::Object::DataType).
	 */
	void AddData(const std::string& name, int level, int requirement);
	/**
	 * Return acces to primary data specification.
	 * @return Primary data specification
	 */
	const PrimaryType& GetPrimary() const;
	/**
	 * Return specified primary type.
	 * @return primary type
	 */
	int GetPrimaryType() const;
	/**
	 * Return specified primary key.
	 * @param key
	 */
	const std::string& GetPrimaryName() const;
	/**
	 * Return const reference to secondaty data records.
	 * @return Secondary data.
	 */
	const DataType& GetData() const;
	/**
	 * Return const reference to secondaty data records.
	 * @warning Use with causion.
	 * @return Secondary data.
	 */
	DataType& GetDataFull();
	/**
	 * Increase pass number when passed betwen modules.
	 */
	void IncreasePassCounter();
	/**
	 * Return actual pass counter.
	 */
	int GetPassCounter() const;
	/**
	 * Set pass counter.
	 * @warning Use with causion.
	 * @param[in] pass New pass counter value.
	 */
	void SetPassCounter(int pass);
	/**
	 * Reset counter, delete primary and secondary data.
	 */
	void Reset();
	/**
	 * Copy primary and secondary data from socket, passCounter and interface.
	 * Original primary and secondary data are deleted.
	 * Anything else remains untouched.
	 * @param[in] source Socket to copy from.
	 */
	void Copy(const Socket& source);
	/**
	 * Add data from source socket. Added data are prefixed with actual passCounter.
	 * If source has no valid primary data ( ie. == 0 ) original primary data are
	 * preserved, otherwise primary data are moved to secondary and source primary 
	 * become primary.
	 * Anything else remains untouched.
	 * @param[in] socket Socket to add data from.
	 */
	void Add(const Socket& socket);
	/**
	 * Test socket, whether the source meets requirements for target.
	 * For interface source, the interface must be included in the target.
	 * For primary data, only value is checked, secondary data name and keys are checked.
	 * @param[in] source Source socket.
	 * @param[in] target Target socket.
	 * @retrun True if module can be connected.
	 */
	static bool Match(const Socket& source, const Socket& target) throw();
protected:
	friend class ::BaseLib::iModule;
protected:
	/**
	 * Connected module. Accessable only from iModule.
	 */
	::BaseLib::iModule* mModule;
private:
	/**
	 * Pass counter. Used to create full names of added objects.
	 */
	int mPassCounter;
	/**
	 * Socket name.
	 */
	std::string mName;
	/**
	 * Socket description.
	 */
	std::string mDescription;
	/**
	 * Primary name, passCounter and type.
	 */
	PrimaryType mPrimary;
	/**
	 * Requirements on secondary data.
	 */ 
	DataType mData;
	/**
	 * Specified module interface.
	 */
	int mInterface;
};

} }

#endif // baseLib_sockets_socket_h

