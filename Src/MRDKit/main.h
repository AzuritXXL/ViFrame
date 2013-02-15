#ifndef rdkit_h
#define rdkit_h

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
#include "Loader.hpp"
#include "ReportProperty.hpp"

/**
 * Wrap RDKit library to the application module. Provide acces
 * to descriptor calculation. Used as a source for object.
 */
class RDKitWrap : public BaseLib::Modules::Loader, BaseLib::Properties::Reportable
{
public:
	RDKitWrap();
private:
	void AddDescriptors(std::vector<double>& data, RDKit::ROMol& mol);
public: // BaseLib::Modules::Loader
	virtual bool CheckIntegrity();
public: // BaseLib::Properties::Reportable
	virtual void OnChange(BaseLib::Properties::BaseProperty* prop);
protected: // BaseLib::Modules::Loader
	virtual BaseLib::Objects::Object* GenerateObject();
	virtual void innerOpen();
	virtual void innerPrepare();
	virtual void innerClose();
private:
	/**
	 * File name.
	 */
	std::string mFileName;
	/**
	 * Add source file name to molecule data ?
	 */
	bool mAddSourceName;
	/**
	 *RDKit mol supplier;
	 */ 
	RDKit::SDMolSupplier* mMolSupplier;
	/**
	 * Name of source file, parsed from mFileName.
	 */
	std::string mSourceName;
	/**
	 * Actual molecule.
	 */
	size_t mMoleculeActual;
	/**
	 * Total count of molecules.
	 */
	size_t mMoleculeCount;
	/**
	 * Add weight as single attribute.
	 */
	bool mAddDescWeight;
};

extern "C" _DYNLINK void* CreateModule();

extern "C" _DYNLINK void DeleteModule(void*);

extern "C" _DYNLINK const BaseLib::Description* GetDescription();

#endif // rdkit_h