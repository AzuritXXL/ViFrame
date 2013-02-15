#include "stdafx.h"
// ...
#include "main.h"
// BaseLib
#include "Property.hpp"
#include "PropertyCreator.hpp"
#include "FileProperty.hpp"
#include "Description.hpp"
// RdKit
#include "GraphMol/Descriptors/MolDescriptors.h"

RDKitWrap::RDKitWrap()
	: mFileName(""), mAddSourceName(true), mMolSupplier(nullptr), mSourceName(""), mAddDescWeight(false)
{
	// set specification for output socket
	mOutputSocketAdd.SetPrimary("RDKit_Descriptors", BaseLib::Objects::Variant::VectorDouble);
	mOutputSocketAdd.AddData("SourceFile", BaseLib::Objects::Variant::String);
	mOutputSocketAdd.SetInterface(BaseLib::Interfaces::SequentialAcces::Identification);

	// validate output socket
	ValidateOutputSocket();

	// properties
	mProperties.push_back(BaseLib::Properties::CreateFileProperty("Source file: ", "File to load chemical compounds from.", mFileName, "All files (*.*)") );
	mProperties.push_back(BaseLib::Properties::CreateReport("Add file name: ", "Add source file name to molecule data ?", mAddSourceName, this) );
	mProperties.push_back(BaseLib::Properties::CreateReport("Add weight: ", "Add weight (AMW) as attribute ?", mAddDescWeight, this) );
}

void RDKitWrap::AddDescriptors(std::vector<double>& data, RDKit::ROMol& mol)
{
	// Crippen.h
	{
		// void RDKit::Descriptors::getCrippenAtomContribs (const ROMol &mol, std::vector< double > &logpContribs, std::vector< double > &mrContribs, bool force=false)
		// void RDKit::Descriptors::calcCrippenDescriptors (const ROMol &mol, double &logp, double &mr, bool includeHs=true, bool force=false)
	}

	// Lipinski.h 
	{
		data.push_back( RDKit::Descriptors::calcLipinskiHBA(mol)		);
		data.push_back( RDKit::Descriptors::calcLipinskiHBD(mol)		);
		data.push_back( RDKit::Descriptors::calcNumRotatableBonds(mol)	);
		data.push_back( RDKit::Descriptors::calcNumHBD(mol)				);
		data.push_back( RDKit::Descriptors::calcNumHBA(mol)				);
		data.push_back( RDKit::Descriptors::calcNumHeteroatoms(mol)		);
		data.push_back( RDKit::Descriptors::calcNumAmideBonds(mol)		);
		data.push_back( RDKit::Descriptors::calcNumRings(mol)			);
	}

	// MolSurf.h
	{
		// double RDKit::Descriptors::getLabuteAtomContribs (const ROMol &mol, std::vector< double > &Vi, double &hContrib, bool includeHs=true, bool force=false)
		// double RDKit::Descriptors::calcLabuteASA (const ROMol &mol, bool includeHs=true, bool force=false)
		// double RDKit::Descriptors::getTPSAAtomContribs (const ROMol &mol, std::vector< double > &Vi, bool force=false)
		// double RDKit::Descriptors::calcTPSA (const ROMol &mol, bool force=false)
		// std::vector< double > RDKit::Descriptors::calcSlogP_VSA (const ROMol &mol, std::vector< double > *bins=0, bool force=false)
		// std::vector< double > RDKit::Descriptors::calcSMR_VSA (const ROMol &mol, std::vector< double > *bins=0, bool force=false)
		// std::vector< double > RDKit::Descriptors::calcPEOE_VSA (const ROMol &mol, std::vector< double > *bins=0, bool force=false)
	}

	// MolDescriptors.h
	{
		data.push_back( RDKit::Descriptors::calcAMW(mol)				);
		data.push_back( RDKit::Descriptors::calcExactMW(mol)			);
		// RDKit::Descriptors::calcMolFormula -> string
	}
}

//								BaseLib::Modules::Loader										//

bool RDKitWrap::CheckIntegrity()
{	// check file
	// call base implementation
	return BaseLib::Modules::Loader::CheckIntegrity();
}

BaseLib::Objects::Object* RDKitWrap::GenerateObject()
{
	if (mMolSupplier->atEnd())
	{
		return nullptr;
	}
	
	RDKit::ROMol* mol = mMolSupplier->next();
	if ( mol == nullptr)
	{	// there is not end .. so there has to be an error
		ReportMessage(BaseLib::iReportable::Error, "Failed to load molecule. ");
		throw std::exception();
	}
	// secure name and create module
	std::string name;
	mol->getProp("_Name", name);
	BaseLib::Objects::Object* obj = new BaseLib::Objects::Object(name);
	// if we should then add file name
	if (mAddSourceName)
	{
		obj->Add("SourceFile", new BaseLib::Objects::String(mSourceName));
	}
	// add data 
	auto mainData = new BaseLib::Objects::VDouble();
	obj->Add("RDKit_Descriptors", mainData);
	obj->SetMain("RDKit_Descriptors");
	
	// -> calculate descriptors .. 
	AddDescriptors(mainData->Data, *mol);
	
	if (mAddDescWeight)
	{	// add weight as a single molecule .. 
		auto att = new BaseLib::Objects::Double();
		att->Data = RDKit::Descriptors::calcAMW(*mol);
		obj->Add("MolWeight", att);
	}

	// report state
	ReportProgress(0, mMoleculeCount, ++mMoleculeActual);

	// report pass
	obj->Pass();
	// delete RDKit rep. and return our object
	delete mol;
	return obj;
}

void RDKitWrap::innerOpen()
{	// we will open source
	try
	{
		// can throw, in case of bad module or .. 
		mMolSupplier = new RDKit::SDMolSupplier(mFileName);
	}
	catch(...)
	{
		mMolSupplier = nullptr;
		ReportMessage(BaseLib::iReportable::Error, "Failed to open source file ( " + mFileName + " ) ");
		throw std::exception();
	}

	if (mAddSourceName)
	{	// determine source file name
		mSourceName = mFileName.substr(mFileName.find_last_of("/\\") + 1);
	}

	// save molecule count
	mMoleculeActual = 0;
	mMoleculeCount = mMolSupplier->length();
	// now we move mMolSupplier to last molecule, so reset
	mMolSupplier->reset();
}

void RDKitWrap::innerPrepare()
{
}

void RDKitWrap::innerClose()
{
	delete mMolSupplier;
	mMolSupplier = nullptr;
}

//							BaseLib::Properties::Reportable										//

void RDKitWrap::OnChange(BaseLib::Properties::BaseProperty* prop)
{	// reset mOutputSocketAdd
	mOutputSocketAdd.Reset();
	mOutputSocketAdd.SetPrimary("RDKit_Descriptors", BaseLib::Objects::Variant::VectorDouble);	
	mOutputSocketAdd.SetInterface(BaseLib::Interfaces::SequentialAcces::Identification);
	if (mAddSourceName)
	{	// if add source file name then add specification
		mOutputSocketAdd.AddData("SourceFile", BaseLib::Objects::Variant::String);
	}
	if (mAddDescWeight)
	{
		mOutputSocketAdd.AddData("MolWeight", BaseLib::Objects::Variant::Double);
	}
	// validate socket .. 
	ValidateOutputSocket();
	// and report change
	EmitTargetControl();
}


//										Module access											//

_DYNLINK void* CreateModule() 
{ 
	return new RDKitWrap(); 
}

_DYNLINK void DeleteModule(void* module) 
{
	RDKitWrap* convModule = reinterpret_cast<RDKitWrap*>(module);
	delete convModule;
}

BaseLib::Description locDescription(std::pair<int, int>(1,0), 
	"RDKit wrap",
	"Wrap function from RDKit.",
	"Module wrap chemical library RDKit and provide acces to his funcionality."
	"As a input module take file with chemical coumpands from which load chemical compounds."
	"After coumpound is load module calculate set of descriptors. </br>"
	"Calculated descriptors are <ul>"
	"<li>LipinskiHBA</li>"
	"<li>LipinskiHBD</li>"
	"<li>NumRotatableBonds</li>"
	"<li>NumHBD</li>"
	"<li>NumHBA</li>"
	"<li>NumHeteroatoms</li>"
	"<li>NumAmideBonds</li>"
	"<li>NumRings</li>"
	"<li>calcAMW</li>"
	"<li>calcExactMW</li>"
	"</ul>"
	"Values of descriptors are add in this order to data object. Also name of coumpound is add."
	"Additional data (SourceFileName and AMW) can be add as separated secondary attributes."
	"</br></br>"
	"Module use RDKit (http://www.rdkit.org/)."
	);

_DYNLINK const BaseLib::Description* GetDescription() 
{ 
	return &locDescription; 
}