#include "stdafx.h"
// ..
#include "LibraryBaseItem.h"
#include "LibraryDataItem.h"
#include "AssemblyTab.h"
#include "AssemblyScene.h"
#include "DialogProperties.h"

namespace Assembly {

Tab::Tab(DataModels::Library::LibraryModel* libraryModel, Logic::LibraryStorage& libraryStorage, QWidget* parent)
	: QWidget(parent), mMessageModel(this), mIsWorking(false)
{
	ui.setupUi(this);
	// TreeLibraries
	ui.TreeLibraries->setModel(libraryModel);
	ui.TreeLibraries->setHeaderHidden(true);
	ui.TreeLibraries->setDragDropMode(QAbstractItemView::DragDrop);
	ui.TreeLibraries->setContextMenuPolicy(Qt::CustomContextMenu);
	
	ui.TreeLibraries->resizeColumnToContents(0);
	ui.TreeLibraries->header()->setResizeMode(0, QHeaderView::ResizeToContents);
	// size of header determine maximum size of TreeLibraries
	ui.TreeLibraries->setMaximumWidth( ui.TreeLibraries->columnWidth(0) + 20 );	

	connect(ui.TreeLibraries, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(slLibraryContextMenu(const QPoint&)));

	// Splits size
	ui.splitter_1->setStretchFactor(0, 4);
	ui.splitter_1->setStretchFactor(1, 1);
	ui.splitter_2->setStretchFactor(0, 1);
	ui.splitter_2->setStretchFactor(1, 5);

	// TableMessages
	ui.TableMessages->setModel(&mMessageModel);

	// Scene and GraphicsView
	mScene = new Scene(libraryStorage, mMessageModel, ui.GraphicsView);
	mScene->SetMinSize();
	ui.GraphicsView->setAlignment(Qt::AlignLeft | Qt::AlignTop);
	ui.GraphicsView->setScene(mScene);
	ui.GraphicsView->setDragMode(QGraphicsView::NoDrag);
	ui.GraphicsView->setAcceptDrops(true);

	// init actions
	actLoadLibrary = new QAction("Load library", this);
	actUnloadLibrary = new QAction("Unload library", this);

	// set tableMessage
	ui.TableMessages->setColumnWidth(0, 100);
	ui.TableMessages->setColumnWidth(1, 22);
	ui.TableMessages->setColumnWidth(2, 150);
	ui.TableMessages->horizontalHeader()->setStretchLastSection(true);
	ui.TableMessages->verticalHeader()->setDefaultSectionSize(20);
}

void Tab::SetEnabled(bool enable)
{
	ui.GraphicsView->setEnabled(enable);
	ui.TreeLibraries->setEnabled(enable);
	// also set working flag
	mIsWorking = !(enable);
}

Scene* Tab::GetScene() const
{
	return mScene;
}

DataModels::Message::Model* Tab::GetMessageModel()
{
	return &mMessageModel;
}

bool Tab::GetIsWorking() const
{
	return mIsWorking;
}

void Tab::OnWorkDone()
{	// reset imagen for all items in scene
	auto iter = std::begin(mScene->GetModules());
	auto iterEnd = std::end(mScene->GetModules());

	for (;iter != iterEnd; ++iter)
	{	// reset images
		iter->second->OnCalculationEnd();
	}
}

//											Slots												//

void Tab::slLibraryContextMenu(const QPoint& position)
{
	QModelIndex index = ui.TreeLibraries->indexAt(position);
    if (!index.isValid()) 
	{	// invalid index
		return;
    }

	DataModels::Library::LibraryBaseItem* item = reinterpret_cast<DataModels::Library::LibraryBaseItem*>(index.internalPointer());

	QList<QAction*> actions;

	if (item->GetHoldLibrary())
	{
		DataModels::Library::LibraryDataItem* itemData = dynamic_cast<DataModels::Library::LibraryDataItem*>(item);

		if (itemData->GetLibrary().GetIsLoaded())
		{	// can be unload
			if (itemData->GetLibrary().GetDependencyCount() < 1)
			{	// only libraries without associated module can be delted
				actions.append(actUnloadLibrary);
			}
		}
		else
		{	// can be load
			actions.append(actLoadLibrary);
		}
	}
	else
	{	// with sons we can do both
		actions.append(actUnloadLibrary);
		actions.append(actLoadLibrary);
	}

	if (actions.size() > 0)
	{
		QAction* action = QMenu::exec(actions, ui.TreeLibraries->mapToGlobal(position));

		if (action == actLoadLibrary)
		{
			try
			{
				item->Load();
			}
			catch (std::exception e)
			{
				QMessageBox msg(this);
				QString message = "Failed to load library : ";
				message += e.what();
				msg.setText(message);
				msg.exec();
			}
		}
		else if (action == actUnloadLibrary)
		{
			try
			{
				item->UnLoad();
			}
			catch(...)
			{
				QMessageBox msg(this);
				msg.setText("Failed to unload library.");
				msg.exec();
			}
		}		
	}
}

}