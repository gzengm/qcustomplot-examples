#include "crossline.h"
#include "cursorhelper.h"

#include <QDebug>
#include <QMouseEvent>

const QString CrossLine::layer = "overlay";
const QMargins CrossLine::margins = QMargins(6, 6, 6, 6);

CrossLine::CrossLine(CustomPlot* parentPlot, QCPGraph* targetGraph)
	: QObject(parentPlot)
	  , mParentPlot(parentPlot)
	  , mTargetGraph(targetGraph ? targetGraph : mParentPlot->graph())
{
	mLinePen = QPen(Qt::black, 2);
	mLineSelectedPen = QPen(Qt::red, 2);
	mTextColor = Qt::black;
	mTextSelectedColor = Qt::red;

	setLineMode(lmFree);

	connect(parentPlot, SIGNAL(afterReplot()), this, SLOT(update()));
	connect(parentPlot, SIGNAL(itemMoved(QCPAbstractItem*,QMouseEvent*)),
	        this, SLOT(onItemMoved(QCPAbstractItem*,QMouseEvent*)));
}

CrossLine::~CrossLine()
{
}

void CrossLine::addHLine_(double value, const QString& valueTextFormat)
{
	QCPItemLine* line = new QCPItemLine(mParentPlot);
	line->setPen(mLinePen);
	line->setSelectedPen(mLineSelectedPen);
	line->start->setType(QCPItemPosition::ptAbsolute);
	line->end->setType(QCPItemPosition::ptAbsolute);
	line->setLayer(layer);
	mHLines.append(line);

	QCPItemText* text = new QCPItemText(mParentPlot);
	text->setColor(mTextColor);
	text->setSelectedPen(mTextSelectedColor);
	text->setPadding(margins);
	text->setLayer(layer);
	mHTexts.append(text);

	mValues.append(value);
	mHTextFormats.append(valueTextFormat);

	CursorHelper* helper = &mParentPlot->cursorHelper;
	if (mTargetGraph->keyAxis()->orientation() == Qt::Horizontal)
	{
		text->position->setParentAnchor(line->start);
	}
	else
	{
		text->position->setParentAnchor(line->end);
	}

	if (mLineMode == lmFollowCursor)
	{
		return;
	}

	if (mLineMode != lmTracing)
	{
		if (mTargetGraph->keyAxis()->orientation() == Qt::Horizontal)
		{
			helper->setCursor(line, QCursor(Qt::SizeVerCursor));
		}
		else
		{
			helper->setCursor(line, QCursor(Qt::SizeHorCursor));
		}
	}
}

void CrossLine::addHLines_(const QVector<double>& values, const QStringList& valueTextFormats)
{
	if (valueTextFormats.isEmpty())
	{
		foreach(const double& value, values)
		{
			addHLine_(value);
		}
	}
	else
	{
		for (int i = 0; i < values.size(); ++i)
		{
			addHLine_(values[i], valueTextFormats.value(i, DEFAULT_VALUE_TEXT_FORMAT));
		}
	}
}

void CrossLine::clearHLines_()
{
	CursorHelper* helper = &mParentPlot->cursorHelper;
	foreach(QCPItemLine *line, mHLines)
	{
		helper->remove(line);
		mParentPlot->removeItem(line);
	}
	mHLines.clear();

	foreach(QCPItemText *text, mHTexts)
	{
		mParentPlot->removeItem(text);
	}
	mHTexts.clear();
	mHTextFormats.clear();

	mValues.clear();
}

void CrossLine::addVLine_(double key, const QString& keyTextFormat)
{
	QCPItemLine* line = new QCPItemLine(mParentPlot);
	line->setPen(mLinePen);
	line->setSelectedPen(mLineSelectedPen);
	line->start->setType(QCPItemPosition::ptAbsolute);
	line->end->setType(QCPItemPosition::ptAbsolute);
	line->setLayer(layer);
	mVLines.append(line);

	QCPItemText* text = new QCPItemText(mParentPlot);
	text->setColor(mTextColor);
	text->setSelectedColor(mTextSelectedColor);
	text->setPadding(margins);
	text->setLayer(layer);
	mVTexts.append(text);

	mKeys.append(key);
	mVTextFormats.append(keyTextFormat);

	CursorHelper* helper = &mParentPlot->cursorHelper;
	if (mTargetGraph->keyAxis()->orientation() == Qt::Horizontal)
	{
		text->position->setParentAnchor(line->end);
	}
	else
	{
		text->position->setParentAnchor(line->start);
	}

	if (mLineMode == lmFollowCursor)
	{
		return;
	}

	if (mTargetGraph->keyAxis()->orientation() == Qt::Horizontal)
	{
		helper->setCursor(line, QCursor(Qt::SizeHorCursor));
	}
	else
	{
		helper->setCursor(line, QCursor(Qt::SizeVerCursor));
	}
}

void CrossLine::addVLines_(const QVector<double>& keys, const QStringList& keyTextFormats)
{
	if (keyTextFormats.isEmpty())
	{
		foreach(const double& key, keys)
		{
			addVLine_(key);
		}
	}
	else
	{
		for (int i = 0; i < keys.size(); ++i)
		{
			addVLine_(keys[i], keyTextFormats.value(i, DEFAULT_KEY_TEXT_FORMAT));
		}
	}
}

void CrossLine::clearVLines_()
{
	CursorHelper* helper = &mParentPlot->cursorHelper;
	foreach(QCPItemLine *line, mVLines)
	{
		helper->remove(line);
		mParentPlot->removeItem(line);
	}
	mVLines.clear();

	foreach(QCPItemText *text, mVTexts)
	{
		mParentPlot->removeItem(text);
	}
	mVTexts.clear();
	mVTextFormats.clear();

	mKeys.clear();
}

void CrossLine::addTracer_(double key, const QString& tracerTextFormat)
{
	if (mLineMode != lmTracing)
	{
		qDebug() << "CrossLine::addTracer: attempt to add tracer in non-lmTracing mode";
		return;
	}

	QCPItemTracer* tracer = new QCPItemTracer(mParentPlot);
	tracer->setBrush(Qt::red);
	tracer->setInterpolating(true);
	tracer->setStyle(QCPItemTracer::tsCircle);
	tracer->setGraph(mTargetGraph);
	tracer->setGraphKey(key);
	tracer->setLayer(layer);
	mTracers.append(tracer);

	QCPItemText* text = new QCPItemText(mParentPlot);
	text->setPadding(margins);
	text->position->setParentAnchor(tracer->position);
	text->position->setType(QCPItemPosition::ptAbsolute);
	text->setLayer(layer);
	mTracerTexts.append(text);
	mTracerTextFormats.append(tracerTextFormat);

	QCPItemCurve* arrow = new QCPItemCurve(mParentPlot);
	arrow->start->setParentAnchor(text->left);
	arrow->startDir->setParentAnchor(arrow->start);
	arrow->end->setParentAnchor(tracer->position);
	arrow->endDir->setParentAnchor(arrow->end);
	arrow->setHead(QCPLineEnding::esSpikeArrow);
	arrow->setTail(QCPLineEnding(QCPLineEnding::esBar,
	                             (text->bottom->pixelPosition().y() -
		                             text->top->pixelPosition().y()) * 0.85));
	arrow->setLayer(layer);
	mTracerArrows.append(arrow);
}

void CrossLine::addTracers_(const QVector<double>& keys, const QStringList& tracerTextFormats, const QStringList& keyTextFormats, const QStringList& valueTextFormats)
{
	for (int i = 0; i < keys.size(); ++i)
	{
		addTracer_(keys[i], tracerTextFormats.value(i, DEFAULT_TRACER_TEXT_FORMAT));
		addVLine_(keys[i], keyTextFormats.value(i, DEFAULT_KEY_TEXT_FORMAT));
		addHLine_(0.0, valueTextFormats.value(i, DEFAULT_VALUE_TEXT_FORMAT));
	}
}

void CrossLine::clearTracers_()
{
	foreach(QCPItemTracer *tracer, mTracers)
	{
		mParentPlot->removeItem(tracer);
	}
	mTracers.clear();

	foreach(QCPItemText *text, mTracerTexts)
	{
		mParentPlot->removeItem(text);
	}
	mTracerTexts.clear();
	mTracerTextFormats.clear();

	foreach(QCPItemCurve *arrow, mTracerArrows)
	{
		mParentPlot->removeItem(arrow);
	}
	mTracerArrows.clear();
}

void CrossLine::addHLine(double value, const QString& valueTextFormat)
{
	if (mLineMode != LineMode::lmFree)
	{
		qDebug() << "CrossLine::addHLine: attempt to add HLine in non-lmFree mode";
		return;
	}

	addHLine_(value, valueTextFormat);

	update();
}

void CrossLine::addHLines(const QVector<double>& values, const QStringList& valueTextFormats)
{
	if (mLineMode != LineMode::lmFree)
	{
		qDebug() << "CrossLine::addHLines: attempt to add HLines in non-lmFree mode";
		return;
	}

	addHLines_(values, valueTextFormats);

	update();
}

void CrossLine::setHLines(const QVector<double>& values, const QStringList& valueTextFormats)
{
	if (mLineMode != LineMode::lmFree)
	{
		qDebug() << "CrossLine::setHLines: attempt to set HLines in non-lmFree mode";
		return;
	}

	clearHLines_();

	addHLines_(values, valueTextFormats);

	update();
}

void CrossLine::clearHLines()
{
	if (mLineMode != lmFree)
	{
		qDebug() << "CrossLine::clearHLines: attempt to clear HLines in non-lmFree mode";
		return;
	}

	clearHLines_();

	update();
}

void CrossLine::addVLine(double key, const QString& keyTextFormat)
{
	if (mLineMode != LineMode::lmFree)
	{
		qDebug() << "CrossLine::addVLine: attempt to add VLine in non-lmFree mode";
		return;
	}

	addVLine_(key, keyTextFormat);

	update();
}

void CrossLine::addVLines(const QVector<double>& keys, const QStringList& keyTextFormats)
{
	if (mLineMode != LineMode::lmFree)
	{
		qDebug() << "CrossLine::addVLines: attempt to add VLines in non-lmFree mode";
		return;
	}

	addVLines_(keys, keyTextFormats);

	update();
}

void CrossLine::setVLines(const QVector<double>& keys, const QStringList& keyTextFormats)
{
	if (mLineMode != LineMode::lmFree)
	{
		qDebug() << "CrossLine::setVLines: attempt to set VLines in non-lmFree mode";
		return;
	}

	clearVLines_();

	addVLines_(keys, keyTextFormats);

	update();
}

void CrossLine::clearVLines()
{
	if (mLineMode != lmFree)
	{
		qDebug() << "CrossLine::clearVLines: attempt to clear VLines in non-lmFree mode";
		return;
	}

	clearVLines_();

	update();
}

void CrossLine::addTracer(double key, const QString& tracerTextFormat, const QString& keyTextFormat, const QString& valueTextFormat)
{
	if (mLineMode != lmTracing)
	{
		qDebug() << "CrossLine::addTracer: attempt to add tracer in non-lmTracing mode";
		return;
	}

	addTracer_(key, tracerTextFormat);
	addVLine_(key, keyTextFormat);
	addHLine_(0.0, valueTextFormat);

	update();
}

void CrossLine::addTracers(const QVector<double>& keys, const QStringList& tracerTextFormats, const QStringList& keyTextFormats, const QStringList& valueTextFormats)
{
	if (mLineMode != lmTracing)
	{
		qDebug() << "CrossLine::addTracers: attempt to add tracers in non-lmTracing mode";
		return;
	}

	addTracers_(keys, tracerTextFormats, keyTextFormats, valueTextFormats);

	update();
}

void CrossLine::setTracers(const QVector<double>& keys, const QStringList& tracerTextFormats, const QStringList& keyTextFormats, const QStringList& valueTextFormats)
{
	if (mLineMode != lmTracing)
	{
		qDebug() << "CrossLine::setTracers: attempt to set tracers in non-lmTracing mode";
		return;
	}

	clearTracers_();
	clearVLines_();
	clearHLines_();

	addTracers_(keys, tracerTextFormats, keyTextFormats, valueTextFormats);

	update();
}

void CrossLine::clearTracers()
{
	if (mLineMode != lmTracing)
	{
		qDebug() << "CrossLine::clearTracers: attempt to clear tracers in non-lmTracing mode";
		return;
	}

	clearTracers_();
	clearVLines_();
	clearHLines_();

	update();
}

void CrossLine::setLineMode(CrossLine::LineMode mode)
{
	mLineMode = mode;
	CursorHelper* helper = &mParentPlot->cursorHelper;

	clearTracers_();
	clearVLines_();
	clearHLines_();

	if (mLineMode == lmFollowCursor)
	{
		connect(mParentPlot, SIGNAL(mouseMove(QMouseEvent*)),
		        this, SLOT(onMouseMoved(QMouseEvent*)));

		addHLine_();
		addVLine_();

		foreach(QCPItemLine *line, mVLines)
		{
			line->setSelectable(false);
		}

		foreach(QCPItemLine *line, mHLines)
		{
			line->setSelectable(false);
		}

		foreach(QCPItemLine *line, mHLines)
		{
			helper->remove(line);
		}

		foreach(QCPItemLine *line, mVLines)
		{
			helper->remove(line);
		}
	}
	else
	{
		disconnect(mParentPlot, SIGNAL(mouseMove(QMouseEvent*)),
		           this, SLOT(onMouseMoved(QMouseEvent*)));

		if (mLineMode == lmTracing)
		{
			addTracer_();
			addVLine_();
			addHLine_();
		}
		else
		{
			addHLine_();
			addVLine_();
		}

		foreach(QCPItemLine *line, mVLines)
		{
			line->setSelectable(true);
		}

		if (mTargetGraph->keyAxis()->orientation() == Qt::Horizontal)
		{
			for (int i = 0; i < mHLines.size(); ++i)
			{
				mHTexts[i]->position->setParentAnchor(mHLines[i]->start);
				helper->setCursor(mHLines[i], QCursor(Qt::SizeVerCursor));
			}
			for (int i = 0; i < mVLines.size(); ++i)
			{
				mVTexts[i]->position->setParentAnchor(mVLines[i]->end);
				helper->setCursor(mVLines[i], QCursor(Qt::SizeHorCursor));
			}
		}
		else
		{
			for (int i = 0; i < mHLines.size(); ++i)
			{
				mHTexts[i]->position->setParentAnchor(mHLines[i]->end);
				helper->setCursor(mHLines[i], QCursor(Qt::SizeHorCursor));
			}
			for (int i = 0; i < mVLines.size(); ++i)
			{
				mVTexts[i]->position->setParentAnchor(mVLines[i]->start);
				helper->setCursor(mVLines[i], QCursor(Qt::SizeVerCursor));
			}
		}

		if (mLineMode == lmTracing)
		{
			foreach(QCPItemLine *line, mHLines)
			{
				line->setSelectable(false);
			}

			for (int i = 0; i < mKeys.size(); ++i)
			{
				mTracers[i]->setGraphKey(mKeys[i]);
			}

			foreach(QCPItemLine *line, mHLines)
			{
				helper->remove(line);
			}
		}
		else if (mLineMode == lmFree)
		{
			foreach(QCPItemLine *line, mHLines)
			{
				line->setSelectable(true);
			}
		}
	}

	update();
}

/*!
  Set \a orientation line to show or hide.
  \note Note: The vertical line is always perpendicular to the keyAxis, and the horizontal line perpendicular to the valueAxis.
 */
void CrossLine::setLineVisible(Qt::Orientation orientation, bool visible)
{
	if ((mTargetGraph->keyAxis()->orientation() == Qt::Horizontal) ^ (orientation == Qt::Horizontal))
	{
		foreach(QCPItemLine *line, mVLines)
		{
			line->setVisible(visible);
		}

		foreach(QCPItemText *text, mVTexts)
		{
			text->setVisible(visible);
		}
	}
	else
	{
		foreach(QCPItemLine *line, mHLines)
		{
			line->setVisible(visible);
		}

		foreach(QCPItemText *text, mHTexts)
		{
			text->setVisible(visible);
		}
	}
}

bool CrossLine::lineVisible(Qt::Orientation orientation)
{
	if ((mTargetGraph->keyAxis()->orientation() == Qt::Horizontal) ^ (orientation == Qt::Horizontal))
	{
		return mVLines[0]->visible();
	}
	return mHLines[0]->visible();
}

/*!
  Set the \a graph which tracer sticks to. And only vertical line can be moved.
  To free the tracer from any tracer, set \a graph to 0.
  Set \a graph to 0 means that horizontal line and vertical line can be moved individually.
 */
void CrossLine::setGraph(QCPGraph* graph)
{
	if (!graph)
	{
		qDebug() << "CrossLine::setGraph: graph is null";
		return;
	}
	if (graph == mTargetGraph)
		return;
	mTargetGraph = graph;

	foreach(QCPItemTracer *tracer, mTracers)
	{
		tracer->setGraph(mTargetGraph);
		tracer->setGraphKey(0);
	}

	if (mTargetGraph->keyAxis()->orientation() == Qt::Horizontal)
	{
		for (int i = 0; i < mHLines.size(); ++i)
		{
			mHTexts[i]->position->setParentAnchor(mHLines[i]->start);
		}
		for (int i = 0; i < mVLines.size(); ++i)
		{
			mVTexts[i]->position->setParentAnchor(mVLines[i]->end);
		}
	}
	else
	{
		for (int i = 0; i < mHLines.size(); ++i)
		{
			mHTexts[i]->position->setParentAnchor(mHLines[i]->end);
		}
		for (int i = 0; i < mVLines.size(); ++i)
		{
			mVTexts[i]->position->setParentAnchor(mVLines[i]->start);
		}
	}
}

void CrossLine::onMouseMoved(QMouseEvent* event)
{
	mTargetGraph->pixelsToCoords(event->localPos(), mKeys[0], mValues[0]);
	update();
}

void CrossLine::onItemMoved(QCPAbstractItem* item, QMouseEvent* event)
{
	if (item == Q_NULLPTR)
		return;

	const int hLineIndex = mHLines.indexOf(qobject_cast<QCPItemLine*>(item));
	const int vLineIndex = mVLines.indexOf(qobject_cast<QCPItemLine*>(item));
	const bool isHLine = (hLineIndex != -1);
	const bool isVLine = (vLineIndex != -1);
	if (!isVLine && !isHLine)
		return;

	const QCPAxis* keyAxis = mTargetGraph->keyAxis();
	const QCPAxis* valueAxis = mTargetGraph->valueAxis();

	const QPointF localPos = event->localPos();
	double key, value;

	if (keyAxis->orientation() == Qt::Horizontal)
	{
		key = keyAxis->pixelToCoord(localPos.x());
		value = valueAxis->pixelToCoord(localPos.y());
	}
	else
	{
		key = keyAxis->pixelToCoord(localPos.y());
		value = valueAxis->pixelToCoord(localPos.x());
	}

	if (mLineMode == lmTracing)
	{
		if (isVLine)
		{
			mTracers[vLineIndex]->setGraphKey(key);
		}
	}
	else
	{
		if (isHLine)
		{
			mValues[hLineIndex] = value;
		}
		else
		{
			mKeys[vLineIndex] = key;
		}
	}

	update();
}

void CrossLine::update()
{
	updateTracer();
	updateHLine();
	updateVLine();

	if (mParentPlot->isVisible() && mParentPlot->layer("overlay")->visible())
		mParentPlot->layer("overlay")->replot();
	// This is needed to ensure that lines are positioned correctly after zooming

	if (mParentPlot->isVisible())
		mParentPlot->replot();
	// This is needed to ensure that lines and tracers are updated properly after adding or removing them
}

void CrossLine::updateTracer()
{
	if (mLineMode != lmTracing)
		return;

	const QRect rect = mTargetGraph->keyAxis()->axisRect()->rect();
	const QPointF center = rect.center();
	const double offset = qMax(rect.width(), rect.height()) / double(8.0);
	const int endOffset = 10;
	const int endDirOffset = 30;
	const int startDirOffset = 40;

	for (int i = 0; i < mTracers.size(); ++i)
	{
		mTracers[i]->updatePosition();
		mKeys[i] = mTracers[i]->position->key();
		mValues[i] = mTracers[i]->position->value();

		QPointF pixel = mTracers[i]->position->pixelPosition();

		Qt::Alignment alignment;
		double offsetX, offsetY;

		if (pixel.x() <= center.x())
		{
			alignment = Qt::AlignLeft;
			offsetX = offset;
			mTracerArrows[i]->start->setParentAnchor(mTracerTexts[i]->left);
			mTracerArrows[i]->startDir->setCoords(-startDirOffset, 0);
			mTracerArrows[i]->end->setCoords(endOffset, 0);
			mTracerArrows[i]->endDir->setCoords(endDirOffset, 0);
		}
		else
		{
			alignment = Qt::AlignRight;
			offsetX = -offset;
			mTracerArrows[i]->start->setParentAnchor(mTracerTexts[i]->right);
			mTracerArrows[i]->startDir->setCoords(startDirOffset, 0);
			mTracerArrows[i]->end->setCoords(-endOffset, 0);
			mTracerArrows[i]->endDir->setCoords(-endDirOffset, 0);
		}
		if (pixel.y() <= center.y())
		{
			alignment |= Qt::AlignTop;
			offsetY = offset;
			mTracerArrows[i]->end->setCoords(mTracerArrows[i]->end->coords().x(), endOffset);
			mTracerArrows[i]->endDir->setCoords(mTracerArrows[i]->endDir->coords().x(), endDirOffset);
		}
		else
		{
			alignment |= Qt::AlignBottom;
			offsetY = -offset;
			mTracerArrows[i]->end->setCoords(mTracerArrows[i]->end->coords().x(), -endOffset);
			mTracerArrows[i]->endDir->setCoords(mTracerArrows[i]->endDir->coords().x(), -endDirOffset);
		}
		mTracerTexts[i]->position->setCoords(offsetX, offsetY);
		mTracerTexts[i]->setPositionAlignment(alignment);
		mTracerTexts[i]->setText(mTracerTextFormats[i].arg(mKeys[i], 0, 'f', 2).arg(mValues[i], 0, 'f', 2));
	}
}

void CrossLine::updateHLine()
{
	const QCPAxis* valueAxis = mTargetGraph->valueAxis();
	const QRect rect = valueAxis->axisRect()->rect();
	const QPointF center = rect.center();

	for (int i = 0; i < mValues.size(); ++i)
	{
		const auto mHLine = mHLines[i];
		const auto mHText = mHTexts[i];
		double value = mValues[i];
		const QString& valueTextFormat = mHTextFormats[i];

		mHText->setText(valueTextFormat.arg(QString::number(value, 'f', 2)));

		value = valueAxis->coordToPixel(value);

		if (valueAxis->orientation() == Qt::Vertical)
		{
			mHLine->start->setCoords(rect.left(), value);
			mHLine->end->setCoords(rect.right(), value);
			const Qt::Alignment alignment = (value >= center.y() ? Qt::AlignBottom : Qt::AlignTop) | Qt::AlignLeft;
			mHText->setPositionAlignment(alignment);
		}
		else
		{
			mHLine->start->setCoords(value, rect.top());
			mHLine->end->setCoords(value, rect.bottom());
			const Qt::Alignment alignment = (value >= center.x() ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignBottom;
			mHText->setPositionAlignment(alignment);
		}
	}
}

void CrossLine::updateVLine()
{
	const QCPAxis* keyAxis = mTargetGraph->keyAxis();
	const QRect rect = keyAxis->axisRect()->rect();
	const QPointF center = rect.center();

	for (int i = 0; i < mKeys.size(); ++i)
	{
		const auto mVLine = mVLines[i];
		const auto mVText = mVTexts[i];
		double key = mKeys[i];
		const QString& keyTextFormat = mVTextFormats[i];

		mVText->setText(keyTextFormat.arg(QString::number(key, 'f', 2)));

		key = keyAxis->coordToPixel(key);

		if (keyAxis->orientation() == Qt::Horizontal)
		{
			mVLine->start->setCoords(key, rect.top());
			mVLine->end->setCoords(key, rect.bottom());
			const Qt::Alignment alignment = (key >= center.x() ? Qt::AlignRight : Qt::AlignLeft) | Qt::AlignBottom;
			mVText->setPositionAlignment(alignment);
		}
		else
		{
			mVLine->start->setCoords(rect.left(), key);
			mVLine->end->setCoords(rect.right(), key);
			const Qt::Alignment alignment = (key >= center.y() ? Qt::AlignBottom : Qt::AlignTop) | Qt::AlignLeft;
			mVText->setPositionAlignment(alignment);
		}
	}
}
