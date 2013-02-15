#ifndef eny_dialogProperties_h
#define eny_dialogProperties_h

/**
 * @author 
 * @version 1.0
 * @date 20.5.2012
 */

// Standart library includes
///
#include <vector>

// Qt includes
///
#include <qsignalmapper.h>
#include <QtGui\qdialog.h>
#include <QtGui\qlineedit.h>

// Local includes
///
#include "ModuleHolder.h"
#include "AssemblyScene.h"

// Generated includes
///
#include "ui_DialogProperties.h"

// BaseLib includes
///
#include "Module.hpp"
#include "Reportable.hpp"
#include "BaseProperty.hpp"
#include "Property.hpp"

namespace Dialog {

class Properties : public QDialog
{
	Q_OBJECT
public:
	Properties(Logic::ModuleHolder& moduleHolder, Assembly::Scene::ModulesType& modules, QWidget* parent);
	~Properties();
protected slots:
	void slTextChange();
	void slAccept();
	void slCancel();
	void slClose();
	void slSetFile(QObject* objArg);
	void slCurrentIndexChanged(int index);
private:
	/**
	 * Store all needed data to change property.
	 */
	struct sProperty
	{
	public:
		/**
		 * Pointer to "setter" of event.
		 */
		QWidget* Setter;
		/**
		 * Index of property.
		 */
		int Index;
		/**
		 * Line edit to show short messages to user.
		 */
		QLineEdit* TipLine;
		/**
		 * Original value.
		 */
		QVariant OriginalValue;
	public:
		sProperty(int index, QWidget* setter, QLineEdit* tipLine, QVariant originalValue) 
			: Setter(setter), Index(index), TipLine(tipLine), OriginalValue(originalValue) { };
	};
	/**
	 * For signal mapper, store all what is need to file selection.
	 */
	struct sFileSelectArg : public QObject
	{
	public:
		// line edit to which store file
		QLineEdit* LineEdit;
		// used file extensions
		QString Extensions;
	public:
		sFileSelectArg(QLineEdit* lineEdit, const QString& extension) 
			: LineEdit(lineEdit), Extensions(extension)	{ };
	};
private:
	/**
	 * Graphical interface.
	 */
	Ui::DialogProperties ui;
	/**
	 * Signal mapper for file selection.
	 */
	QSignalMapper* mSignalsFileSelect;
	/**
	 * Module to which we serve.
	 */
	BaseLib::iModule* mModule;
	/**
	 * Store property groups.
	 */
	std::vector<sProperty> mProperties;
	/**
	 * Pointer to modules.
	 */
	Assembly::Scene::ModulesType& mModules;
private:
	template<typename T> bool PropertySet(QLineEdit* lineEdit, QLineEdit* lineMessage, BaseLib::Properties::Property<T>* propertyPointer, const T& value);
	/**
	 * @param[in] propoerty index
	 * @param[in] propertyIter iter which will be used to create property
	 */
	void InitProperty(int indexProperty, BaseLib::iModule::PropertyType::iterator& propertyIter);
	/**
	 * Init propoerties gui.
	 */
	void InitProperties();
	/**
	 * Resize dialog to fit properties.
	 */
	void FitSize();
};

}

#endif // eny_dialogProperties_h