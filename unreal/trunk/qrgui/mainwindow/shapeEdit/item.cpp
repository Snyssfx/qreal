#include "item.h"
#include <QtGui/QPainter>
#include <QtGui/QStyle>
#include <QtGui/QStyleOptionGraphicsItem>

Item::Item(QGraphicsItem* parent) : QGraphicsItem(parent), mDomElementType(noneType), mDragState(None)
{
	setFlag(QGraphicsItem::ItemIsSelectable, true);
	setFlag(QGraphicsItem::ItemIsMovable, true);
}

void Item::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
	Q_UNUSED(widget);
	painter->setPen(mPen);
	drawItem(painter, option, widget);
	if (option->state & QStyle::State_Selected) {
		painter->save();
		QPen pen(Qt::red);
		pen.setWidth(3);
		painter->setPen(pen);
		drawExtractionForItem(painter);
		painter->restore();
	}
}

void Item::drawExtractionForItem(QPainter* painter)
{
	painter->drawPoint(mX1, mY1);
	painter->drawPoint(mX1, mY2);
	painter->drawPoint(mX2, mY1);
	painter->drawPoint(mX2, mY2);
}

void Item::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	QGraphicsItem::mouseMoveEvent(event);
}

QPen Item::pen() const
{
	return mPen;
}

QBrush Item::brush() const
{
	return mBrush;
}

void Item::setBrush(QBrush const &brush)
{
	mBrush = brush;
}

void Item::setPen(QPen const &pen)
{
	mPen = pen;
}

void Item::swap(qreal &x, qreal &y)
{
	qreal tmp = x;
	x = y;
	y = tmp;
}

void Item::setX1andY1(qreal x, qreal y)
{
	mX1 = x;
	mY1 = y;
	update();
}

void Item::setX1andY2(qreal x, qreal y)
{
	mX1 = x;
	mY2 = y;
	update();
}

void Item::setX2andY1(qreal x, qreal y)
{
	mX2 = x;
	mY1 = y;
	update();
}

void Item::setX2andY2(qreal x, qreal y)
{
	mX2 = x;
	mY2 = y;
	update();
}

void Item::setNoneDragState()
{
	mDragState = None;
}

void Item::reshapeRectWithShift()
{
	qreal size = qMax(abs(mX2 - mX1), abs(mY2 - mY1));
	if(mX2 > mX1) {
		if (mY2 > mY1)
			setX2andY2(mX1 + size, mY1 + size);
		else
			setX2andY2(mX1 + size, mY1 - size);
	} else {
		if (mY2 > mY1)
			setX2andY2(mX1 - size, mY1 + size);
		else
			setX2andY2(mX1 - size, mY1 - size);
	}
}

void Item::changeDragState(qreal x, qreal y)
{
	if (QRectF(QPointF(mX1 + scenePos().x(), mY1 + scenePos().y()), QSizeF(0, 0)).adjusted(-5, -5, 5, 5).contains(QPointF(x, y)))
		mDragState = TopLeft;
	else if (QRectF(QPointF(mX2 + scenePos().x(), mY1 + scenePos().y()), QSizeF(0, 0)).adjusted(-5, -5, 5, 5).contains(QPointF(x, y)))
		mDragState = TopRight;
	else if (QRectF(QPointF(mX1 + scenePos().x(), mY2 + scenePos().y()), QSizeF(0, 0)).adjusted(-5, -5, 5, 5).contains(QPointF(x, y)))
		mDragState = BottomLeft;
	else if (QRectF(QPointF(mX2 + scenePos().x(), mY2 + scenePos().y()), QSizeF(0, 0)).adjusted(-5, -5, 5, 5).contains(QPointF(x, y)))
		mDragState = BottomRight;
	else
		mDragState = None;
}

void Item::calcResizeItem(QGraphicsSceneMouseEvent *event)
{
	qreal x = mapFromScene(event->scenePos()).x();
	qreal y = mapFromScene(event->scenePos()).y();
	if (mDragState != None)
		setFlag(QGraphicsItem::ItemIsMovable, false);
	if (mDragState == TopLeft)
		setX1andY1(x, y);
	else if (mDragState == TopRight)
		setX2andY1(x, y);
	else if (mDragState == BottomLeft)
		setX1andY2(x, y);
	else if (mDragState == BottomRight)
		setX2andY2(x, y);
}

void Item::resizeItem(QGraphicsSceneMouseEvent *event)
{
	if (mDragState != None)
		calcResizeItem(event);
}

void Item::setXandY(QDomElement& dom, QRectF const &rect)
{
	dom.setAttribute("y1", rect.top());
	dom.setAttribute("x1", rect.left());
	dom.setAttribute("y2", rect.bottom());
	dom.setAttribute("x2", rect.right());
}

QDomElement Item::setPenBrush(QDomDocument &document, QString const &domName)
{
	QDomElement dom = document.createElement(domName);
	dom.setAttribute("fill", mBrush.color().name());

	if (mBrush.style() == Qt::NoBrush)
		dom.setAttribute("fill-style", "none");
	if (mBrush.style() == Qt::SolidPattern)
		dom.setAttribute("fill-style", "solid");

	dom.setAttribute("stroke", mPen.color().name());

	dom.setAttribute("stroke-width", mPen.width());

	QString penStyle;
	switch (mPen.style()) {
	case Qt::SolidLine:
		penStyle = "solid";
		break;
	case Qt::DotLine:
		penStyle = "dot";
		break;
	case Qt::DashLine:
		penStyle = "dash";
		break;
	case Qt::DashDotLine:
		penStyle =  "dashdot";
		break;
	case Qt::DashDotDotLine:
		penStyle = "dashdotdot";
		break;
	case Qt::NoPen:
		penStyle = "none";
		break;
	default:
		break;
	}
	dom.setAttribute("stroke-style", penStyle);

	return dom;
}

QRectF Item::sceneBoundingRectCoord(QPointF const &topLeftPicture)
{
	qreal const x1 = scenePos().x() + boundingRect().x() - topLeftPicture.x();
	qreal const y1 = scenePos().y() + boundingRect().y() - topLeftPicture.y();
	return QRectF(x1, y1, boundingRect().width(), boundingRect().height());
}