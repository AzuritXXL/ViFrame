#include "stdafx.h"
// Qt
#include <QInputDialog>
#include <QColorDialog>
#include <QtGui\QTreeView.h>
#include <QtGui\qsplitter.h>
#include <QtGui\QVBoxLayout>
#include <QSortFilterProxyModel.h>
#include <qmessagebox.h>
#include <QProgressDialog>
// ..
#include "OutputTab.h"
#include "OutputDataItem.h"
#include "DialogObjectDetail.h"
#include "DialogScene.h"
#include "DialogSetMain.h"
#include "DialogSaveScreen.h"
#include "ColorGenerator.h"
#include "DialogExport.h"
#include "OutputExporter.h"

namespace Output {

Tab::Tab(std::vector<BaseLib::Objects::Object*>& objects, DataModels::Output::VirtualItem* root, QList<QString>& keys, QList<QString>& integralKeys, DataItemType& dataItems, QWidget* parent)
	: QWidget(parent), mObjects(), mModel(root, mModelDataItems, this), mGraphicsView(nullptr), mTreeView(nullptr), mSelectedItem(nullptr), mSilentMode(false), mModelSelected(this)
{	// retake object
	std::swap(objects, mObjects);
	std::swap(keys, mDataKeys);
	std::swap(integralKeys, mIntegralDataKeys);
	std::swap(dataItems, mModelDataItems); 
	// mModelDataItems is shared with mGraphicsView ( due selection) and mModel ( becouse of easier regrouping )
	InitGui();
	// create graphics
	CreateGraphics();

	// rescale .. 
	AutoRescale();
	// set scene size
	mGraphicsView->FitSceneSize();
	mGraphicsView->MadeWholeSceneVisible();	
	// update scene items
	mGraphicsView->UpdateItemSize();
}

Tab::~Tab()
{
	// we will delete mObjects and mModelItems .. mModel will delete virtual items
	// and DataItem

	// empty modelSelection
	DataModels::Output::ModelSelected::DataItemsType empty;
	mModelSelected.SetData(empty);

	// release mObjects
	std::for_each(std::begin(mObjects), std::end(mObjects),
		[&] (BaseLib::Objects::Object* item)
		{
			delete item;
			item = nullptr;
		}
	);
	mObjects.clear();

	// release mModelItems -> will be done by mModel, we will just give up the pointers
	mModelDataItems.clear();
}

void Tab::AdjustScene()
{
	// set scene size
	mGraphicsView->FitSceneSize();
	mGraphicsView->MadeWholeSceneVisible();	
	// update scene items
	mGraphicsView->UpdateItemSize();
}

void Tab::CreateGraphics()
{
	size_t index = 0;
	double x,y;
	double minX, minY, maxX, maxY;

	QProgressDialog* dialog = nullptr;
	if (!mSilentMode)
	{
		dialog = new QProgressDialog("Creating graphics representation...", QString(), 0, 100, this);
	}

	int counter = 0;
	int singleStep = mModelDataItems.size() / 100;

	// use first object to set initial value
	(*std::begin(mModelDataItems))->CreateGraphic(mGraphicsView->scene(), index++, x, y);
	minX = maxX = x;
	minY = maxY = y;
	std::for_each(++std::begin(mModelDataItems), std::end(mModelDataItems),
		[&] (DataItemType::reference item)
		{
			// gather data for min max value... 
			item->CreateGraphic(mGraphicsView->scene(), index++, x, y);
			// set min max values
			if (x < minX)
			{
				minX = x;
			}
			else if (x > maxX)
			{
				maxX = x;
			}
			if (y < minY)
			{
				minY = y;
			}
			else if (y > maxY)
			{
				maxY = y;
			}
			// report to dialog
			counter++;
			if(dialog != nullptr && counter == singleStep)
			{
				counter = 0;
				dialog->setValue(dialog->value() + 1);
			}
		}
	);
	mSceneScale.first = mSceneScale.second = 1;
	// set scene
	mSceneSize.first = std::abs(maxX - minX);
	mSceneSize.second = std::abs(maxY - minY);
	// set dialog to end value
	if (dialog != nullptr)
	{
		dialog->setValue(100);
		delete dialog;
		dialog = nullptr;
	}
}

void Tab::InitGui()
{
	QVBoxLayout *mainLayout = new QVBoxLayout(this);
	// ToolBar
	QToolBar* toolBar = new QToolBar(this);
	actReGroup = new QAction("Re-goupe", this);
	connect(actReGroup, SIGNAL(triggered()), this, SLOT(slReGroup()));
	toolBar->addAction(actReGroup);
	actSetMain = new QAction("Set main", this);
	connect(actSetMain, SIGNAL(triggered()), this, SLOT(slSetMain()));
	toolBar->addAction(actSetMain);
	actReSizeScene = new QAction("Resize scene", this);
	connect(actReSizeScene, SIGNAL(triggered()), this, SLOT(slResizeScene()));
	toolBar->addAction(actReSizeScene);
	actSaveScreen = new QAction("Save screen", this);
	connect(actSaveScreen, SIGNAL(triggered()), this, SLOT(slSaveScreen()));
	toolBar->addAction(actSaveScreen);
	actGenColors = new QAction("Generate colors", this);
	connect(actGenColors, SIGNAL(triggered()), this, SLOT(slGenColors()));
	toolBar->addAction(actGenColors);

	actSaveAllScreens = new QAction("Generate screens", this);
	connect(actSaveAllScreens, SIGNAL(triggered()), this, SLOT(slSaveAllScreens()));
	toolBar->addAction(actSaveAllScreens);
	
	mainLayout->addWidget(toolBar);
	// TabWidget
	mTabWidget = new QTabWidget(this);
	{
		// TreeView
		mTreeView = new QTreeView(mTabWidget);
		mTreeView->setSelectionBehavior(QAbstractItemView::SelectItems);
		mTreeView->setSelectionMode(QAbstractItemView::ExtendedSelection);
		mTreeView->setDropIndicatorShown(true);
		mTreeView->setDragDropMode(QAbstractItemView::DragDrop);
		mTreeView->setContextMenuPolicy(Qt::CustomContextMenu);
		mTreeView->setSortingEnabled(true);
		connect(mTreeView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(sShowContextMenu(const QPoint &)));
		connect(mTreeView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slItemViewDoubleClicked(const QModelIndex &)));
		// TableView
		mListView = new QListView(mTabWidget);
		mListView->setModel(&mModelSelected);
		mTreeView->setSelectionBehavior(QAbstractItemView::SelectItems);
		mListView->setSelectionMode(QAbstractItemView::SingleSelection);
		mListView->setContextMenuPolicy(Qt::CustomContextMenu);
		connect(mListView, SIGNAL(customContextMenuRequested(const QPoint &)), this, SLOT(sShowContextMenuList(const QPoint &)));
		connect(mListView, SIGNAL(doubleClicked(const QModelIndex &)), this, SLOT(slItemViewDoubleClicked(const QModelIndex &)));

		mTabWidget->addTab(mTreeView, tr("all"));
		mTabWidget->addTab(mListView, tr("selected"));
	}
	// Sorting
	mProxyModel = new QSortFilterProxyModel(this);
	mProxyModel->setDynamicSortFilter(true);
	mProxyModel->setSourceModel(&mModel);
    mTreeView->setModel(mProxyModel);
	// GraphicsView
	mGraphicsView = new GraphicsView(mModel, mModelDataItems, *mProxyModel, *mTreeView->selectionModel(), mModelSelected, this);
	mGraphicsView->setScene(new QGraphicsScene(mGraphicsView));

	// Spliter
	QSplitter *splitter = new QSplitter(this);
	splitter->setOrientation(Qt::Horizontal);
	splitter->addWidget(mGraphicsView);	
	splitter->addWidget(mTabWidget);
	QList<int> sizes;
	sizes.append(400);
	sizes.append(100);
	splitter->setSizes(sizes);

	// Final 
	mainLayout->addWidget(splitter);
	setLayout(mainLayout);

	// Actions - context menu
	actAttributes = new QAction("Show details", this);
	connect(actAttributes, SIGNAL(triggered()), this, SLOT(slAttributes()));
	actCreateGroup = new QAction("Create group", this);
	connect(actCreateGroup, SIGNAL(triggered()), this, SLOT(slCreateGroup()));
	actDismissGroup = new QAction("Dismiss group", this);
	connect(actDismissGroup, SIGNAL(triggered()), this, SLOT(slDismissGroup()));
	actSetColor = new QAction("Set color", this);
	connect(actSetColor, SIGNAL(triggered()), this, SLOT(slSetColor()));
	actRename = new QAction("Rename", this);
	connect(actRename, SIGNAL(triggered()), this, SLOT(slRename()));
	actExport = new QAction("Export", this);
	connect(actExport, SIGNAL(triggered()), this, SLOT(slExport()));
	actHide = new QAction("Hyde", this);
	connect(actHide, SIGNAL(triggered()), this, SLOT(slHide()));
	actShow = new QAction("Show", this);
	connect(actShow, SIGNAL(triggered()), this, SLOT(slShow()));
	// action for selection module
	actListAttributes = new QAction("Show details", this);
	actMoveSelectionToNewGroup = new QAction("Move to new group", this);
}

// when changing this it is necesarly to change size calculation in slSaveAllScreens
void Tab::DrawLegend(int width, int height, int longestText, const std::unordered_set<DataModels::Output::VirtualItem*>& items, QPainter& painter, int position) const
{
	if (items.size() == 0)
	{	// no data -> no lengend 
		return;
	}

	// border rect .. used to draw rectangle around legend .. 
	int textHeight = painter.fontMetrics().height() * 1.5; // text + we add spacing .. 

	// legend margin ( distance from image borders )
	int legendMargin = painter.fontMetrics().height();

	// size that we will have from borders .. 
	int borderWidthSpace = painter.fontMetrics().averageCharWidth() << 1;
	int borderHeightSpace = painter.fontMetrics().height();

	// left top corner of legend ..
	int legendX = 0;
	int legendY = 0;

	// calculate left upper position of legend
	switch(position)
	{
	case Dialog::SaveScreen::LeftTop:	
		{
			legendX = legendY = legendMargin;
			break;
		}
	case Dialog::SaveScreen::LeftBottom:
		{
			legendX = legendMargin;
			legendY = height - ((textHeight * items.size() ) + borderHeightSpace + legendMargin);
			break;
		}
	case Dialog::SaveScreen::RightTop:
		{
			legendX = width - (longestText + (borderWidthSpace << 1) + legendMargin);
			legendY = legendMargin;
			break;
		}
	case Dialog::SaveScreen::RightBottom:
		{
			legendX = width - (longestText + (borderWidthSpace << 1) + legendMargin);
			legendY = height - ((textHeight * items.size() ) + borderHeightSpace + legendMargin);
			break;
		}
	case Dialog::SaveScreen::None:
	default:	// ignore legend draw ..
		return;
	}

	// draw legend ( adjust left top corner to legendX/Y)
	auto iter = std::begin(items);
	auto iterEnd = std::end(items);

	int startWidth = legendX + borderWidthSpace;
	int startHeight = legendY + borderHeightSpace + painter.fontMetrics().height() * 0.5;
	int actualHeight = startHeight;
	for (int i = 0; iter != iterEnd; ++iter, ++i)
	{
		// set color
		painter.setPen((*iter)->GetColor());
		painter.drawText(startWidth, actualHeight, (*iter)->GetName() );
		actualHeight += textHeight;
	}
	// set color to black and draw border rect ..
	painter.setPen(Qt::black);
	painter.drawRect(legendX, legendY, longestText + (borderWidthSpace << 1), (textHeight * items.size() ) + borderHeightSpace );

}

void Tab::AutoRescale()
{
	bool resize = false;

	// check is we need to resize .. 
	if ((mSceneSize.first < 100 || mSceneSize.first > 10000) && mSceneSize.first != 0)
	{
		resize = true;
		mSceneScale.first = 1000 / mSceneSize.first;
	}
	if ((mSceneSize.second < 100 || mSceneSize.second > 10000)  && mSceneSize.second != 0)
	{
		resize = true;
		mSceneScale.second = 1000 / mSceneSize.second;
	}

	if (resize)
	{
		QProgressDialog* dialog = nullptr;
		int counter = 0;
		int singleStep = mModelDataItems.size() / 99;

		if (mSilentMode)
		{	// no information to user
		}
		else
		{
			QMessageBox::information(this, "Scene resize", "Inappropriate scene size. Scene will be resize.", QMessageBox::Ok);

			// dialog .. 
			dialog = new QProgressDialog("Resizing scene...", QString(), 0, 100, this);
		}
		
		// do transformation
		std::for_each(std::begin(mModelDataItems), std::end(mModelDataItems),
			[&] (DataItemType::reference item)
			{
				item->ScalePosition(mSceneScale.first, mSceneScale.second);

				// report to dialog
				counter++;
				if(dialog != nullptr && counter == singleStep)
				{
					counter = 0;
					dialog->setValue(dialog->value() + 1);
				}
			}
		);

		// update after resize
		mGraphicsView->update();

		if (dialog != nullptr)
		{
			// set dialog to end value
			dialog->setValue(100);
			delete dialog;
			dialog = nullptr;
		}
	}
}

bool Tab::SetMain(const Output::Policy& policy)
{
	bool resutl = true;

	// use first object to set initial value of scene size
	double x, y;
	double minX, minY, maxX, maxY;
	(*std::begin(mModelDataItems))->SetShowKey(policy, x, y);
	minX = maxX = x;
	minY = maxY = y;
		
	// dialog .. 
	QProgressDialog* dialog = nullptr;
	int counter = 0;
	int singleStep = mModelDataItems.size() / 100;

	if (!mSilentMode)
	{
		dialog = new QProgressDialog("Setting up main value...", QString(), 0, 100, this);
	}

	// set show key for all data
	std::for_each(++std::begin(mModelDataItems), std::end(mModelDataItems),
		[&] (DataItemType::reference item)
		{
			resutl &= item->SetShowKey(policy, x, y);
			// gather data for min max value ...
			// set min max values
			if (x < minX)
			{
				minX = x;
			}
			else if (x > maxX)
			{
				maxX = x;
			}
			if (y < minY)
			{
				minY = y;
			}
			else if (y > maxY)
			{
				maxY = y;
			}

			// report to dialog
			counter++;
			if(dialog != nullptr && counter == singleStep)
			{
				counter = 0;
				dialog->setValue(dialog->value() + 1);
			}
		}
	);

	// set scene size
	mSceneSize.first = std::abs(maxX - minX);
	mSceneSize.second = std::abs(maxY - minY);

	// set dialog to end value
	if (dialog != nullptr)
	{
		dialog->setValue(100);
		delete dialog;
		dialog = nullptr;
	}

	// reset scale
	mSceneScale.first = mSceneScale.second = 1;
	// rescale scene if needed
	AutoRescale();

	return resutl;
}

//											Slots												//

void Tab::sShowContextMenu(const QPoint& position)
{
	// default selected item is root
	mSelectedItem = mModel.GetRoot();
	// 
	QModelIndex index = mProxyModel->mapToSource(mTreeView->indexAt(position));
		
	QList<QAction*> actions;
    if (!index.isValid()) 
	{	// invalid index
    }
	else
	{	// we are on some index
		mSelectedItem = static_cast<DataModels::Output::BaseItem*>(index.internalPointer());
		
		if (mSelectedItem->GetType() == DataModels::Output::VirtualItem::Identification)
		{	// Virtual
			actions.append(actRename);
			actions.append(actSetColor);
			actions.append(actDismissGroup);
		}
		else
		{	// Data
			actions.append(actAttributes);
		}
	}

	// in all cases we can create group ( in root item )
	actions.append(actCreateGroup);

	// selection dependent ...
	if (mTreeView->selectionModel()->hasSelection())
	{	// somethyng is selected .. so add selection depend actions
		actions.append(actExport);
		actions.append(actShow);
		actions.append(actHide);
	}
	
	QMenu::exec(actions, mTreeView->mapToGlobal(position));
}

void Tab::slAttributes()
{ // in mSelectedItem is selected item
	if (mSelectedItem->GetType() == DataModels::Output::DataItem::Identification)
	{
		Dialog::ObjectDetail dlg(dynamic_cast<DataModels::Output::DataItem*>(mSelectedItem)->GetObject(), this);
		dlg.exec();
	}	
}

void Tab::slCreateGroup()
{ // in mSelectedItem is selected item

	// we need Virtual item so it we dont have it, we get parent wich is Virtual ( by definition )
	if (mSelectedItem->GetType() != DataModels::Output::VirtualItem::Identification)
	{	// root IsVirtual, so we can't get wrong
		mSelectedItem = mSelectedItem->GetParent();
	}
	
	DataModels::Output::VirtualItem* virtualItem = 
		dynamic_cast<DataModels::Output::VirtualItem*>(mSelectedItem);

	bool ok;
	QString name = QInputDialog::getText(this, tr("Create new group"),
                                          tr("Name:"), QLineEdit::Normal,
                                          tr("New group"), &ok);
	if (ok && !name.isEmpty())
	{
		mModel.AddGroup(name, virtualItem);
	}

	mSelectedItem = nullptr;
}

void Tab::slDismissGroup()
{ // in mSelectedItem is selected item
	if (mSelectedItem->GetType() != DataModels::Output::VirtualItem::Identification)
	{
		mSelectedItem = nullptr;
		return;
	}
	
	DataModels::Output::VirtualItem* virtualItem = 
		dynamic_cast<DataModels::Output::VirtualItem*>(mSelectedItem);
	// dismiss
	mModel.DismissGroup(virtualItem);
	mSelectedItem = nullptr;
}

void Tab::slSetColor()
{ // in mSelectedItem is selected item
	if (mSelectedItem->GetType() != DataModels::Output::VirtualItem::Identification)
	{
		mSelectedItem = nullptr;
		return;
	}

	DataModels::Output::VirtualItem* virtualItem = 
		dynamic_cast<DataModels::Output::VirtualItem*>(mSelectedItem);

	QColor color = QColorDialog::getColor(Qt::black, this, tr("Select color") );

	if (color.isValid())
	{
		virtualItem->SetColor(&color);
	}

	mSelectedItem = nullptr;
}

void Tab::slRename()
{ // in mSelectedItem is selected item
	if (mSelectedItem->GetType() == DataModels::Output::VirtualItem::Identification)
	{
		DataModels::Output::VirtualItem* virtualItem = 
			dynamic_cast<DataModels::Output::VirtualItem*>(mSelectedItem);

		bool ok = false;
		QString originalText = virtualItem->GetName();
		QString name =  QInputDialog::getText(this, tr("Rename group"),
                                          tr("Name:"), QLineEdit::Normal,
                                          originalText, &ok);
		if (ok && !name.isEmpty())
		{
			virtualItem->SetName(name);
		}
	}
	else
	{	// DataItem can not be renamed
	}
	mSelectedItem = nullptr;
}

void Tab::slReGroup()
{
	bool ok = false;
	QString keyName = QInputDialog::getItem(this, "Select attribute", "Select attribute which will be used to create groups.", mDataKeys, 0, false, &ok);
	if (ok)
	{
		mModel.ReGroup(keyName);
	}
}

void Tab::slSetMain()
{
	Dialog::SetMain dlg(mIntegralDataKeys, this);
	int result = dlg.exec();
	if (result == QDialog::Accepted)
	{	// get policy 
		const Policy& policy = dlg.GetPolicy();
		// and set ...
		if (SetMain(policy))
		{	// everything goes well
		}
		else
		{	// show message box
			QMessageBox::warning(this, "Lack of data", "There are not enough data for set up all main data, missing data was replaced by zero value", QMessageBox::Ok);
		}

		// update scene items
		mGraphicsView->UpdateItemSize();
		// set scene size
		mGraphicsView->FitSceneSize();
		mGraphicsView->MadeWholeSceneVisible();	
		// update scene items
		mGraphicsView->UpdateItemSize();
	}
}

void Tab::slExport()
{	// let user select data to export

	std::string objectName = "object name";
	std::string parentName = "parent name";

	Dialog::Export dlg(mDataKeys, this, objectName, parentName);

	if (dlg.exec() == QDialog::Rejected)
	{	// export canceled
		return;
	}
	
	auto attToExportQ = dlg.GetSelection();
	if (attToExportQ.size() == 0)
	{
		QMessageBox::information(this, "Export canceled", "No attributes to export selected", QMessageBox::Ok, QMessageBox::Ok);
		return;
	}

	QString path = QFileDialog::getSaveFileName(this, tr("Select file"), "", "Text files (*.txt)");
	if (path.isNull())
	{	// dialog canceled
		return;
	}

	// prepare information to export
	std::string separator = dlg.GetSeparator().toStdString();
	// atribute names
	std::vector<std::string> attributes;
	attributes.resize(attToExportQ.size());
	for (int i = 0; i < attToExportQ.size(); ++i)
	{
		attributes[i] = attToExportQ[i].toStdString();
	}

	Output::Eporter exporter(attributes, objectName, parentName, separator);
	exporter.Open(path.toStdString());

	// we need to go through all selected data
	QModelIndexList list = mTreeView->selectionModel()->selectedIndexes();
	auto dataIter = list.begin();
	auto dataIterEnd = list.end();

	int progress = 0;
	int nextProgress = list.size() / 20;
	QProgressDialog report("Exporting data...", QString(), 0, list.size(), this);

	auto attIterEnd = std::end(attributes);
	for (;dataIter != dataIterEnd; ++dataIter)
	{
		auto baseData = reinterpret_cast<DataModels::Output::BaseItem*>(mProxyModel->mapToSource(*dataIter).internalPointer());
		if (baseData->GetType() == DataModels::Output::DataItem::Identification)
		{	// data oject -> export
			exporter.Export( dynamic_cast<DataModels::Output::DataItem*>(baseData) );
		}
		else
		{	// DataModels::Output::VirtualItem::Identification
			exporter.Export( dynamic_cast<DataModels::Output::VirtualItem*>(baseData) );
		}
		// report progress
		progress++;
		if (progress > nextProgress)
		{
			nextProgress += list.size() / 20;
			report.setValue(progress);
		}
	}
	// close file
	exporter.Close();
	// close dialog
	report.setValue(list.size());
}

void Tab::slResizeScene()
{	// we use scale to obtain actual scene size, and we also pass max scene size
	Dialog::Scene dlg(mSceneScale.first * mSceneSize.first, mSceneScale.second * mSceneSize.second, mSceneSize.first, mSceneSize.second, this);
	
	if (dlg.exec() == QDialog::Rejected)
	{	// dialog canceled
		return;
	}
	// ok dialog was closed .. by ok .. so calculate new scale
	double newSizeX = dlg.GetX();
	double newSizeY = dlg.GetY();

	// calculate scale, avoid division by zero
	if (mSceneSize.first == 0)
	{
		mSceneScale.first = 1;
	}
	else
	{
		mSceneScale.first = newSizeX / mSceneSize.first;
	}
	if (mSceneSize.second == 0)
	{
		mSceneScale.second = 1;
	}
	else
	{
		mSceneScale.second = newSizeY / mSceneSize.second;
	}
	
	// do transformation
	std::for_each(std::begin(mModelDataItems), std::end(mModelDataItems),
		[&] (DataItemType::reference item)
		{
			item->ScalePosition(mSceneScale.first, mSceneScale.second);
		}
	);
	// reset transformation
	mGraphicsView->resetTransform();
	// set scene size
	mGraphicsView->FitSceneSize();
	mGraphicsView->MadeWholeSceneVisible();	
	// update scene items
	mGraphicsView->UpdateItemSize();
}

void Tab::slHide()
{	// iterate through all selected items
	QModelIndexList list = mTreeView->selectionModel()->selectedIndexes();
	auto iter = list.begin();
	auto iterEnd = list.end();
	for (;iter != iterEnd; ++iter)
	{	// set visibility
		reinterpret_cast<DataModels::Output::BaseItem*>(mProxyModel->mapToSource(*iter).internalPointer())->SetVisible(false);
	}
}

void Tab::slShow()
{	// iterate through all selected items
	QModelIndexList list = mTreeView->selectionModel()->selectedIndexes();
	auto iter = list.begin();
	auto iterEnd = list.end();
	for (;iter != iterEnd; ++iter)
	{	// set visibility
		reinterpret_cast<DataModels::Output::BaseItem*>(mProxyModel->mapToSource(*iter).internalPointer())->SetVisible(true);
	}
}

void Tab::slSaveScreen()
{
	// first dialog with options
	Dialog::SaveScreen dlgSaveScreen(this);
	if (dlgSaveScreen.exec() == QDialog::Rejected)
	{	// dialog close with cancel
		return;
	}

	// second dialog with file selection
	QString path = QFileDialog::getSaveFileName(this, tr("Save image as"), "", "Png image (*.png)");
	if (path.isNull())
	{	// dialog canceled
		return;
	}

	QProgressDialog progress("Generating image ...", QString(), 0, 100, this);
	progress.setWindowModality(Qt::WindowModal);

	// we have path and setting -> we will save data ...	
	QImage image(dlgSaveScreen.GetWidth(), dlgSaveScreen.GetHeight(), QImage::Format_ARGB32_Premultiplied);
	QPainter paint(&image);

	// otherwise .. we continue to gather data and draw legend ... 

	// we paint GraphicsView .. what is exacly what we see .. 
	mGraphicsView->render(&paint);	
	progress.setValue(20);

	if (dlgSaveScreen.GetLegend() == Dialog::SaveScreen::None)
	{	// no Legend .. we can end here
		paint.end();
		// save
		image.save(path, "PNG");
		progress.setValue(100);
		return;
	}

	// create legend -> begin with set up font
	paint.setFont(dlgSaveScreen.GetFont());

	// here we store groups that will be shown in legend
	std::unordered_set<DataModels::Output::VirtualItem*> virtualItems;

	// max text leght, later used to determine legend width
	int maxTextSize = 0;
	// used for calculating text lenght
	auto textMetrics = paint.fontMetrics();

	progress.setLabelText("Gathering information about groups.");
	if (dlgSaveScreen.GetUseRoot())
	{	// we just use root items .. 
		int childCount = mModel.GetRoot()->GetChildCount();

		// size of single progress step
		int progressCounter = 0;
		int progressStep = childCount / 80;

		for (int i = 0; i < childCount; ++i)
		{
			if (mModel.GetRoot()->GetChild(i)->GetType() == DataModels::Output::VirtualItem::Identification)
			{	//group .. so add it
				int tempLeght = textMetrics.boundingRect(dynamic_cast<DataModels::Output::VirtualItem*>( mModel.GetRoot()->GetChild(i) )->GetName()).width();
				maxTextSize = tempLeght > maxTextSize ? tempLeght : maxTextSize;
				virtualItems.insert(dynamic_cast<DataModels::Output::VirtualItem*>( mModel.GetRoot()->GetChild(i) ));
			}
			
			// dialog ... 
			progressCounter++;
			if (progressCounter > progressStep)
			{
				progressCounter = 0;
				progress.setValue(progress.value() + progressStep);
			}
		}
	}
	else
	{	// we have to find relative items -> we go through all items .. and find parents .. ( to determine what show in legend .. )

		// paint scene, only what we see ( items take viewport position )
		QList<QGraphicsItem*> visibleScene = mGraphicsView->items(0, 0, mGraphicsView->width(), mGraphicsView->height());
		// mGraphicsView->items , return only visible items !!
				

		// size of single progress step
		int progressCounter = 0;
		int progressStep = visibleScene.size() / 80;
		
		std::for_each(visibleScene.begin(), visibleScene.end(),
			[&] (QGraphicsItem* item)
			{	// obtain data .. which is index into mObjects
				auto index = item->data(0).toInt();
				assert(index >= 0 && index < mObjects.size());			
				// in runtime we belive that index is valid ( we do it in seleciton too )
				if (mModelDataItems[index]->GetParent()->GetParent() != nullptr)
				{	// parent of parent is not null -> parent has parent root -> parent is not root
					auto parent = mModelDataItems[index]->GetParent();
					// gather max text lenght
					int tempLeght = textMetrics.boundingRect(parent->GetName()).width();
					maxTextSize = tempLeght > maxTextSize ? tempLeght : maxTextSize;

					virtualItems.insert(parent);
					// if failed .. virtualItems already contains value .. ok we dont want same value twice ..
				}
				else
				{	// according to previous .. parent is root so nothing .. ( root can't be in legend )
				}
				// dialog ... 
				progressCounter++;
				if (progressCounter > progressStep)
				{
					progressCounter = 0;
					progress.setValue(progress.value() + progressStep);
				}
			}
		);
	}
	// now we have in virtualItems parent of all visible items .. potencial candidates to legend
	
	progress.setLabelText("Drawing legend.");

	// first start with calculating legend size estimation ..	
	int estimatedWidth = maxTextSize + 10;
	int estimatedHeigh = (paint.fontMetrics().height() * virtualItems.size()) + 10;

	progress.setValue(91);

	if ((estimatedWidth >= (image.width() * 0.5)) || (estimatedHeigh >= (image.height() * 0.5)) )
	{	// legend is larger than half of image .. 
		QMessageBox::information(this, "Legend", "Image is too small for legend. Legend will not be draw.", QMessageBox::Ok);
	}
	else
	{
		// draw legend ...
		DrawLegend(image.width(), image.height(), maxTextSize, virtualItems, paint, dlgSaveScreen.GetLegend());
	}
	// end draw
	paint.end();

	// save
	image.save(path, "PNG");

	progress.setValue(100);
}

void Tab::slGenColors()
{	// get color generator type
	Utility::ColorGenerator::eType type;

	bool selected;
	QStringList strList;
	strList.push_back("Normal");
	strList.push_back("Fluent");
	QString selectedItem = QInputDialog::getItem(this, "Color type", "Select color type:", strList, 0, false, &selected);
	// nothing selected, end .. 
	if (!selected)
	{
		return;
	}

	// as group count we use number of children
	int groupCount = mModel.GetRoot()->GetChildCount();
	if (!selectedItem.isEmpty())
	{
		for (int i = 0; i < strList.size(); ++i)
		{
			if (selectedItem == strList[i])
			{
				switch(i)
				{
				case 1:
					{
						type = Utility::ColorGenerator::FluentChange;
						// show information about 
						QMessageBox::information(this, "Group name", 
							"Group must have names as numbers. From 0 to count of number of groups.", QMessageBox::Ok);
						// as groupCount we need index of highest group, let user fill
						bool indexFill;
						int tempGroupCount = QInputDialog::getInt(this, "Group count", "Highest group index. If canceled number of group under root will be used.",
							0, 0, 214000000, 1, &indexFill);
						if (indexFill)
						{	// use user index as group count
							groupCount = tempGroupCount;
						}
						else
						{
							// use number of root childs
						}					
						break;
					}
				case 0:
				default:
					type = Utility::ColorGenerator::Base;
					break;
				}
				// end cycle
				break;
			}
		}
	}

	// create color generator class
	Utility::ColorGenerator colorGen(type, groupCount);
	
	// progress dialog .. we do not report true data .. becouse single step is too small
	QProgressDialog progress("Generating colors ...", QString(), 0, 100, this);	
	progress.setWindowModality(Qt::WindowModal);
	progress.setValue(50);

	int childCount = mModel.GetRoot()->GetChildCount();
	// index for virtual items .. 
	int virtualIndex = 0;
	
	for (int i = 0; i < childCount; ++i)
	{
		if (mModel.GetRoot()->GetChild(i)->GetType() == DataModels::Output::VirtualItem::Identification)
		{	//group .. so add it
			auto virtualItem = dynamic_cast<DataModels::Output::VirtualItem*>( mModel.GetRoot()->GetChild(i) );

			if (type == Utility::ColorGenerator::FluentChange)
			{	// set virtual index according to name
				bool ok = false;
				int value = virtualItem->GetName().toInt(&ok);
				if (ok)
				{	// parse succesfull use value
					// values are from 1 we want from 0 so decrease
					value;
				}
				else
				{	// parse failed use 0 as default
					value = 0;
				}

				virtualItem->SetColor(colorGen.GenerateColor(value));
			}
			else // (type == Utility::ColorGenerator::Base
			{
				// set color according to object index
				virtualItem->SetColor(colorGen.GenerateColor(virtualIndex++));
			}
		}
	}

	progress.setValue(100);
}

void Tab::slSaveAllScreens()
{	// first we obtain atribute name 
	bool ok = false;
	QString keyName = QInputDialog::getItem(this, "Select attribute", "Select attribute which will be used to create groups.", mIntegralDataKeys, 0, false, &ok);
	if (!ok)
	{	// dialog closed by cancel
		return;
	}
	std::string keyNameStd = keyName.toStdString();
	// then image properties .. we force user to use root groupe for legened generation
	Dialog::SaveScreen dlgSaveScreen(this, true);
	if (dlgSaveScreen.exec() == QDialog::Rejected)
	{	// dialog close with cancel
		return;
	}
	// and on the end directory into which we can save data
	QString directory = QFileDialog::getExistingDirectory(this, "Select directory for images");
	if (directory.isEmpty())
	{
		return;
	}
	// else we have all what we need from user .. so lets find out something about data .. 

	QProgressDialog progress("Gathering data about objects ...", QString(), 0, 101, this);	
	progress.setWindowModality(Qt::WindowModal);
	// we show progress dialog .. 
	progress.show();

	// store original positions
	std::vector<QPointF> originalPositions;
	originalPositions.resize(mModelDataItems.size());
	// we start by finding maximum data size
	int maxIndex = 0;
	auto iter = std::begin(mModelDataItems);
	auto iterEnd = std::end(mModelDataItems);
	for (size_t i = 0;iter != iterEnd; ++iter, ++i)
	{
		originalPositions[i] = (*iter)->GetPosition();
		int tempSize = (*iter)->GetDataSize(keyNameStd);
		maxIndex = tempSize > maxIndex ? tempSize : maxIndex;
	} // now we have stored original position and we know highest index value

	progress.setValue(10);

	// generate legend, all data will be visible, so we can use root one .. 
	
	// create image and set font .. 
	QImage* legendImage = nullptr;
	int legendPosX = 0, legendPosY = 0;

	if (dlgSaveScreen.GetLegend() != Dialog::SaveScreen::None)
	{	// if we will need legend .. then gather data for it 
		QPainter legendPainter;
		legendPainter.setFont(dlgSaveScreen.GetFont());	// necesarly to determine text size .. 

		// here we store groups that will be shown in legend
		std::unordered_set<DataModels::Output::VirtualItem*> virtualItems;

		progress.setLabelText(tr("Gathering data for legend ..."));

		// max text leght, later used to determine legend width
		int maxTextSize = 0;
		// used for calculating text lenght
		auto textMetrics = legendPainter.fontMetrics();
		
		// we just use root items .. 
		int childCount = mModel.GetRoot()->GetChildCount();

		// size of single progress step
		int progressCounter = 0;
		int progressStep = childCount / 10;

		for (int i = 0; i < childCount; ++i)
		{
			if (mModel.GetRoot()->GetChild(i)->GetType() == DataModels::Output::VirtualItem::Identification)
			{	//group .. so add it
				int tempLeght = textMetrics.boundingRect(dynamic_cast<DataModels::Output::VirtualItem*>( mModel.GetRoot()->GetChild(i) )->GetName()).width();
				maxTextSize = tempLeght > maxTextSize ? tempLeght : maxTextSize;
				virtualItems.insert(dynamic_cast<DataModels::Output::VirtualItem*>( mModel.GetRoot()->GetChild(i) ));
			}
			
			// dialog ... 
			progressCounter++;
			if (progressCounter > progressStep)
			{
				progressCounter = 0;
				progress.setValue(progress.value() + progressStep);
			}
		}

		// we draw legend ...
		progress.setLabelText(tr("Drawing legend ..."));

		// we need to dermine legend size .. and then resize image to that size
		int width = maxTextSize + (legendPainter.fontMetrics().averageCharWidth() << 2) + legendPainter.fontMetrics().height() + 5;
		int height = (legendPainter.fontMetrics().height() * 1.5 * virtualItems.size() ) + (legendPainter.fontMetrics().height() << 2) + 5;

		if ((width >= (dlgSaveScreen.GetWidth() * 0.5)) || (height >= (dlgSaveScreen.GetHeight() * 0.5)) )
		{	// legend is larger than half of image .. 
			QMessageBox::information(this, "Legend", "Image is too small for legend. Legend will not be draw.", QMessageBox::Ok);
			// in legendImage stay nullptr
		}
		else
		{	// draw legend
			legendImage = new QImage(width, height, QImage::Format_ARGB32_Premultiplied);			
			legendImage->fill(Qt::transparent);

			legendPainter.begin(legendImage);

			// draw legend into left top corner
			DrawLegend(width, height, maxTextSize, virtualItems, legendPainter, Dialog::SaveScreen::LeftTop);

			legendPainter.end();

			// calculate legend position

			switch(dlgSaveScreen.GetLegend())
			{
			case Dialog::SaveScreen::LeftTop:	
				{
					legendPosX = legendPosY = 0;
					break;
				}
			case Dialog::SaveScreen::LeftBottom:
				{
					legendPosX = 0;
					legendPosY = dlgSaveScreen.GetHeight() - legendImage->height();
					break;
				}
			case Dialog::SaveScreen::RightTop:
				{
					legendPosX = dlgSaveScreen.GetWidth() - legendImage->width();
					legendPosY = 0;
					break;
				}
			case Dialog::SaveScreen::RightBottom:
				{
					legendPosX = dlgSaveScreen.GetWidth() - legendImage->width();
					legendPosY = dlgSaveScreen.GetHeight() - legendImage->height();
					break;
				}
			case Dialog::SaveScreen::None:
			default:	// ignore legend draw ..
				return;
			}
		}
	}
	progress.setValue(20);	
	// we have 80 points in dialog left .. 
		

	int screenCount = 0;
	for (int i = 0; i < maxIndex; ++i) screenCount += i;
	// calculate dialog step
	int dialogCount = 0;
	int dialogStep = 80 / screenCount;
	int imageNumber = 0;

	Output::Policy policy;
	policy.Name = keyNameStd;
	policy.UseFirstAviable = false;
	policy.UseSecondAviable = false;
	// do next thing in silend mode so no dialog from app
	mSilentMode = true;

	QString screenCountString = tr(" from ") + QString::number(screenCount);

	// update size according to output image size
	std::for_each(std::begin(mModelDataItems), std::end(mModelDataItems), 
		[&] (DataItemType::reference item)
		{
			item->SetSize(10);
		}
	);

	// now we have data for legen and we know all .. co be begin draw images .. 
	for (int index0 = 0; index0 != maxIndex; ++index0)
	{
		policy.FirstColumn = index0;
		for (int index1 = index0 + 1; index1 != maxIndex; ++index1)
		{	// index0 index1 is couple wor what we will draw to .. 
			policy.SecondColumn = index1;
			
			progress.setLabelText(tr("Drawing images ") + QString::number(imageNumber++) + screenCountString );

			QImage image(dlgSaveScreen.GetWidth(), dlgSaveScreen.GetHeight(), QImage::Format_ARGB32_Premultiplied);
			QPainter paint(&image);

			SetMain(policy);

			// set scene size
			mGraphicsView->FitSceneSize();
			mGraphicsView->MadeWholeSceneVisible();	

			// we paint GraphicsView .. what is exacly what we see .. 
			mGraphicsView->scene()->render(&paint);	

			if (dlgSaveScreen.GetLegend() == Dialog::SaveScreen::None || legendImage != nullptr)
			{	// no Legend ..
			}
			else
			{	// there is legend, so draw it .. 
				paint.drawImage(legendPosX, legendPosY, *legendImage);
			}
			paint.end();
			// create filename and save ..
			QString filename = keyName + tr("_") + QString::number(index0) + tr("x") + QString::number(index1) + tr(".png");			
			image.save(directory + tr("/") + filename, "PNG");

			// report dialog progress
			dialogCount++;
			if (dialogCount > dialogStep)
			{
				dialogCount = 0;
				progress.setValue(progress.value() + 1);
			}
		}
	}

	// we also release legendImage if need
	if (legendImage != nullptr)
	{
		delete legendImage;
		legendImage = nullptr;
	}

	// set old positions back .. 
	{
		// we start by finding maximum data size
		auto iter = std::begin(mModelDataItems);
		auto iterEnd = std::end(mModelDataItems);
		for (size_t i = 0;iter != iterEnd; ++iter, ++i)
		{
			(*iter)->SetPosition(originalPositions[i]);
		}
		// rescale
		AutoRescale();
		// set scene size
		mGraphicsView->FitSceneSize();
		mGraphicsView->MadeWholeSceneVisible();	
		// update scene items
		mGraphicsView->UpdateItemSize();
	}

	// all work is done .. 
	progress.setValue(101);

	// swich off silent mode 
	mSilentMode = false;
}

void Tab::sShowContextMenuList(const QPoint& position)
{
	QModelIndex index = mListView->indexAt(position);
	
	QList<QAction*> actions;

	if (index.isValid()) 
	{	// valid index
		actions.append(actListAttributes);
    }
	else
	{	// invlalid index .. no detail 
	}
	
	// we always can create group for empty ones if there is something selected )
	if (mModelSelected.AccesData().empty())
	{	// no data -> index was invalid -> no action menu
		return;
	}
	actions.append(actMoveSelectionToNewGroup);

	// show context menu
	auto result = QMenu::exec(actions, mListView->mapToGlobal(position));
	
	if (result == actListAttributes)
	{
		// we are on some index -> get data
		DataModels::Output::DataItem* activeItem = mModelSelected.AccesData(index.row());

		Dialog::ObjectDetail dlg(activeItem->GetObject(), this);
		dlg.exec();
	}
	else if (result == actMoveSelectionToNewGroup)
	{	// move items into new group 

		// get name
		bool ok;
		QString name = QInputDialog::getText(this, tr("Create new group"),
											  tr("Name:"), QLineEdit::Normal,
											  tr("New group"), &ok);
		if (ok && !name.isEmpty())
		{	// move items
			mModel.MoveIntoNewGroup(name, mModelSelected.AccesData());
		}	
	}

}

void Tab::slItemViewDoubleClicked(const QModelIndex& index)
{
	if (!index.isValid())
	{	// invalid index
		return;
	}

	DataModels::Output::BaseItem* baseItem = nullptr;
	// we have to differ getting object according to sender
	if (sender() == mTreeView)
	{
		auto modelIndex = mProxyModel->mapToSource(index);
		baseItem = reinterpret_cast<DataModels::Output::BaseItem*>(modelIndex.internalPointer());
	}
	else
	{	// mTreeView do not provide object pointer in internalPointer
		baseItem = reinterpret_cast<DataModels::Output::BaseItem*>( mModelSelected.AccesData(index.row()) );
	}

	if (baseItem->GetType() == DataModels::Output::DataItem::Identification)
	{	// dataItem, we ca center on him
		DataModels::Output::DataItem* dataItem = dynamic_cast<DataModels::Output::DataItem*>(baseItem);
		// center
		mGraphicsView->centerOn(dataItem->GetPosition());
	}
	else
	{	// ignore
	}
}

}