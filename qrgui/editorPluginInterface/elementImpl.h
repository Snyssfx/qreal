#pragma once

#include <QtWidgets/QWidget>
#include <QtCore/QList>
#include <QtCore/QRectF>
#include <QtCore/QPointF>
#include <QtCore/QUrl>
#include <QtGui/QPainter>

#include <qrkernel/ids.h>
#include <qrutils/inFile.h>

#include "editorPluginInterface/labelInterface.h"
#include "editorPluginInterface/labelFactoryInterface.h"
#include "editorPluginInterface/elementRepoInterface.h"
#include "editorPluginInterface/portHelpers.h"

namespace enums {
namespace linkShape {
enum LinkShape
{
	unset = -1
	, broken
	, square
	, curve
};
}
}

namespace qReal {

typedef QPair<QPair<qReal::Id, qReal::Id>, QPair<bool, qReal::Id> > PossibleEdge;
typedef QPair<QPair<QString, QString>, QPair<bool, QString> > StringPossibleEdge;
typedef QPair<bool, qReal::Id> PossibleEdgeType;
typedef QPair<qReal::Id, qReal::Id> ElementPair;

/** @class ElementImpl
*	@brief base class for generated stuff in plugins
*	TODO: split into NodeElementImpl and EdgeElementImpl
* */

class ElementImpl {
public:
	virtual ~ElementImpl() {}
	virtual void init(QRectF &contents
			, PortFactoryInterface const &portFactory
			, QList<PortInterface *> &ports
			, LabelFactoryInterface &labelFactory
			, QList<LabelInterface *> &labels) = 0;
	virtual void init(LabelFactoryInterface &factory
			, QList<LabelInterface*> &titles) = 0;

	virtual QString qmlString() const = 0;

	virtual void updateData(ElementRepoInterface *repo) const = 0;
	virtual bool isNode() const = 0;
	virtual bool isResizeable() const = 0;
	virtual Qt::PenStyle getPenStyle() const = 0;
	virtual int getPenWidth() const = 0;
	virtual QColor getPenColor() const = 0;
	virtual void drawStartArrow(QPainter *painter) const = 0;
	virtual void drawEndArrow(QPainter *painter) const = 0;
	virtual bool isDividable() const = 0;

	/*Container properties*/
	virtual bool isContainer() const = 0;
	virtual bool isSortingContainer() const = 0;
	virtual QVector<int> sizeOfForestalling() const = 0;
	virtual int sizeOfChildrenForestalling() const = 0;
	virtual bool hasMovableChildren() const = 0;
	virtual bool minimizesToChildren() const = 0;
	virtual bool maximizesChildren() const = 0;

	virtual QStringList fromPortTypes() const = 0;
	virtual QStringList toPortTypes() const = 0;

	virtual enums::linkShape::LinkShape shapeType() const = 0;

	virtual bool isPort() const = 0;
	virtual bool hasPin() const = 0;

	virtual bool createChildrenFromMenu() const = 0;

	virtual QList<double> border() const = 0;

	virtual QStringList bonusContextMenuFields() const = 0;

	/// Update shape of an element. Does nothing in case of generated editors, used by metamodel interpreter.
	virtual void updateRendererContent(QString const &shape)
	{
		Q_UNUSED(shape);
	}
};

}
