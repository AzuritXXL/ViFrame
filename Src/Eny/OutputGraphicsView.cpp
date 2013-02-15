#include "stdafx.h"
// ..
#include "OutputGraphicsView.h"

namespace Output {

GraphicsView::GraphicsView(DataModels::Output::Model& model, DataItemType& dataItems, QSortFilterProxyModel& proxyModel, QItemSelectionModel& selectionModel, DataModels::Output::ModelSelected& modelSelection,  QWidget* parent)
	: QGraphicsView(parent), mModel(model), mProxyModel(proxyModel), mSelectionModel(selectionModel), mDataItems(dataItems), mSelectionState(None), mModelSelected(modelSelection)
{
	// we support drag and drop movement
	setDragMode(QGraphicsView::ScrollHandDrag);
	// connect to selection model
	connect(&mSelectionModel, SIGNAL(selectionChanged(const QItemSelection&, const QItemSelection&)),  this, SLOT(slModelSelectionChanged(const QItemSelection&, const QItemSelection&)));
}

void GraphicsView::UpdateItemSize()
{
	// calculate size
	double size = 10.0 / matrix().m11();

	// update size
	std::for_each(std::begin(mDataItems), std::end(mDataItems), 
		[&] (DataItemType::reference item)
		{
			item->SetSize(size);
		}
	);
	// update scene
	scene()->update();
}

void GraphicsView::FitSceneSize()
{	// get size
	//QRectF boundRect = scene()->itemsBoundingRect();
	QRectF boundRect = scene()->sceneRect();
	// determine border size
	double border = boundRect.width() < boundRect.height() ? boundRect.width() : boundRect.height();
	border = border / 50.0;	// take two percent as a border
	// increase size ( add borders )
	boundRect.setX(boundRect.x() - border);
	boundRect.setY(boundRect.y() - border);
	boundRect.setWidth(boundRect.width() + (2 * border));
	boundRect.setHeight(boundRect.height() + (2 * border));
	// set new size
	scene()->setSceneRect(boundRect);
}

void GraphicsView::MadeWholeSceneVisible()
{
	// start with reseting transformation
	resetTransform();

	//auto sr = sceneRect();
	//auto s = size();

	double boundSize = std::max(sceneRect().width(), sceneRect().height());
	// resize scene so whole scene will fit into view, we substract some size to create borders .. 
	scale((size().width() - 10 )/ boundSize, (size().height() - 10) / boundSize);
}

void GraphicsView::OnUpdateSelection()
{
	QItemSelection selection;

	std::vector<DataModels::Output::DataItem*> selected;
	
	auto iter = std::begin(mDataItems);
	auto iterEnd = std::end(mDataItems);
	for (;iter != iterEnd; ++iter)
	{
		if ((*iter)->IsSelected())
		{	// item is selected
		
			auto modelIndex = mModel.CreateIndex((*iter)->GetRow(), 0, (*iter));
			selection.push_back(QItemSelectionRange(modelIndex));
			// add to selected models
			selected.push_back(*iter);
		}
	}
	
	// transform into proxy
	QItemSelection proxySelection = mProxyModel.mapSelectionFromSource(selection);
	// select items 
	mSelectionModel.select(proxySelection, QItemSelectionModel::ClearAndSelect);

	// set model for selection
	mModelSelected.SetData(selected);
}

//										QGraphicsView											//

void GraphicsView::wheelEvent(QWheelEvent* event)
{
	// how fast we zoom
    double scaleFactor = 1.15;

    if(event->delta() > 0) 
	{   // zoom in
		auto mfs = mapFromScene(QRect(0, 0, 1, 1));

		auto w = mfs.boundingRect().width();

		if (mfs.boundingRect().width() < 10)
		{
			scale(scaleFactor, scaleFactor);
			UpdateItemSize();
		}
    } 
	else 
	{
		QRectF transRect = transform().mapRect(sceneRect());
		
		// if we need to zoom out more 
		if (size().width() * 1.1 < transRect.width() || size().height() * 1.1 < transRect.height())
		{
			// zooming out
			scale(1.0 / scaleFactor, 1.0 / scaleFactor);
			UpdateItemSize();
		}
		else
		{

		}
    }
	event->accept();
}

void GraphicsView::contextMenuEvent(QContextMenuEvent* event)
{
}

void GraphicsView::keyPressEvent(QKeyEvent * event)
{
	if ((QApplication::keyboardModifiers() & Qt::ControlModifier) == Qt::ControlModifier)
	{	// ctr pressed we are selecting
		
		switch(mSelectionState)
		{
		case None:	// none selection start new
			{
				mSelectionState = Ready;
				setDragMode(QGraphicsView::RubberBandDrag);
				break;
			}
		case Deactive:	// ctr is back ..
			{
				mSelectionState = Selecting;
				break;
			}
		default:	// do not change actual setting
			break;
		}

	}
	QGraphicsView::keyPressEvent(event);
}

void GraphicsView::keyReleaseEvent(QKeyEvent* event)
{
	if ((QApplication::keyboardModifiers() & Qt::ControlModifier) != Qt::ControlModifier)
	{	// ctrl released we shoul end selection 
		switch(mSelectionState)
		{
		case Ready:	// only ctr .. end selection
			{
				mSelectionState = None;
				setDragMode(QGraphicsView::ScrollHandDrag);
				break;
			}
		case Selecting:
			{
				mSelectionState = Deactive;
				break;
			}
		}
	}
	QGraphicsView::keyPressEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent* event)
{
	// call original implementation
	QGraphicsView::mouseReleaseEvent(event);

	// if mouse up we first need to hande it and then change DragMode

	switch(mSelectionState)
	{
	case Selecting:	// selecting ... 
	case Deactive:	// selection ending wait only on us
		{
			setDragMode(QGraphicsView::ScrollHandDrag);
			// it is importat to call OnUpdateSelection before changing mSelectionState !!
			// becouse of slModelSelectionChanged
			OnUpdateSelection();
			// in every case this ends selection
			mSelectionState = None;
			break;	
		}
	}	
}

void GraphicsView::mousePressEvent(QMouseEvent* event)
{
	switch(mSelectionState)
	{
	case Ready:	// if ready then start seleciton
		{
			mSelectionState = Selecting;
			break;	
		}
	}	
	// call original implementation
	QGraphicsView::mousePressEvent(event);
}

//											Slots												//

void GraphicsView::slModelSelectionChanged(const QItemSelection& selected, const QItemSelection& deselected)
{	// selection change in TreeView	
	if (mSelectionState != None)
	{	// we selecting, we can ignore this ( it just report what we know allready )
		return;
	}

	QItemSelection sourceSelected = mProxyModel.mapSelectionToSource(selected);
	QItemSelection sourceDeselected = mProxyModel.mapSelectionToSource(deselected);
	
	// select
	auto indexesSelected = sourceSelected.indexes();	
	size_t selectedSize = indexesSelected.size();
	for (size_t i = 0; i < selectedSize; ++i)
	{
		auto base = reinterpret_cast<DataModels::Output::BaseItem*>(indexesSelected[i].internalPointer());
		base->SelectionChange(true);

		if (base->GetType() == DataModels::Output::DataItem::Identification)
		{	
			mModelSelected.Append( dynamic_cast<DataModels::Output::DataItem*>(base) );
		}
	}

	// deselect
	auto indexesDeselected = sourceDeselected.indexes();	
	size_t deselectedSize = indexesDeselected.size();
	for (size_t i = 0; i < deselectedSize; ++i)
	{
		auto base = reinterpret_cast<DataModels::Output::BaseItem*>(indexesDeselected[i].internalPointer());
		base->SelectionChange(false);

		if (base->GetType() == DataModels::Output::DataItem::Identification)
		{		
			mModelSelected.Remove( dynamic_cast<DataModels::Output::DataItem*>(base) );
		}
	}
}

}