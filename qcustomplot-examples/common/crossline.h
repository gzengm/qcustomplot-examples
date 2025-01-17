﻿#ifndef CROSSLINE_H
#define CROSSLINE_H

#include "customplot.h"

#include <QObject>
#include <QMargins>

class QCustomPlot;
class QCPItemLine;
class QCPItemText;
class QCPItemTracer;
class QCPItemCurve;
class QCPAbstractItem;
class QMouseEvent;
class QCPGraph;

#define DEFAULT_KEY_TEXT_FORMAT "%1"
#define DEFAULT_VALUE_TEXT_FORMAT "%1"
#define DEFAULT_TRACER_TEXT_FORMAT "(%1, %2)"

class CrossLine : public QObject
{
	Q_OBJECT

public:
	enum LineMode
	{
		lmFree,
		lmFollowCursor,
		lmTracing
	};

	Q_ENUM(LineMode)

	explicit CrossLine(CustomPlot* parentPlot, QCPGraph* targetGraph = Q_NULLPTR);
	~CrossLine();

	void addHLine(double value = 0.0, const QString& valueTextFormat = DEFAULT_VALUE_TEXT_FORMAT);
	void addVLine(double key = 0.0, const QString& keyTextFormat = DEFAULT_KEY_TEXT_FORMAT);
	void addTracer(double key, const QString& tracerTextFormat = DEFAULT_TRACER_TEXT_FORMAT, const QString& keyTextFormat = DEFAULT_KEY_TEXT_FORMAT, const QString& valueTextFormat = DEFAULT_VALUE_TEXT_FORMAT);

	void addHLines(const QVector<double>& values, const QStringList& valueTextFormats = QStringList());
	void addVLines(const QVector<double>& keys, const QStringList& keyTextFormats = QStringList());
	void addTracers(const QVector<double>& keys, const QStringList& tracerTextFormats = QStringList(), const QStringList& keyTextFormats = QStringList(), const QStringList& valueTextFormats = QStringList());

	void setHLines(const QVector<double>& values, const QStringList& valueTextFormats = QStringList());
	void setVLines(const QVector<double>& keys, const QStringList& keyTextFormats = QStringList());
	void setTracers(const QVector<double>& keys, const QStringList& tracerTextFormats = QStringList(), const QStringList& keyTextFormats = QStringList(), const QStringList& valueTextFormats = QStringList());

	void clearHLines();
	void clearVLines();
	void clearTracers();

	void setLineMode(LineMode mode);
	LineMode lineMode() const { return mLineMode; }

	void setLineVisible(Qt::Orientation orientation, bool visible = true);
	bool lineVisible(Qt::Orientation orientation);

	void setGraph(QCPGraph* graph);

protected:
	void updateTracer();
	void updateHLine();
	void updateVLine();

private:
	void addHLine_(double value = 0.0, const QString& valueTextFormat = DEFAULT_VALUE_TEXT_FORMAT);
	void addVLine_(double key = 0.0, const QString& keyTextFormat = DEFAULT_KEY_TEXT_FORMAT);
	void addTracer_(double key = 0.0, const QString& tracerTextFormat = DEFAULT_TRACER_TEXT_FORMAT);

	void addHLines_(const QVector<double>& values, const QStringList& valueTextFormats = QStringList());
	void addVLines_(const QVector<double>& keys, const QStringList& keyTextFormats = QStringList());
	void addTracers_(const QVector<double>& keys, const QStringList& tracerTextFormats = QStringList(), const QStringList& keyTextFormats = QStringList(), const QStringList& valueTextFormats = QStringList());

	void clearHLines_();
	void clearVLines_();
	void clearTracers_();

public Q_SLOTS:
	void onMouseMoved(QMouseEvent* event);
	void onItemMoved(QCPAbstractItem* item, QMouseEvent* event);
	void update();

protected:
	CustomPlot* mParentPlot;
	QCPGraph* mTargetGraph;

	// mHLines, mHTexts 和 mValues 长度始终一致
	// mVLines, mVTexts 和 mKeys 长度始终一致
	// mTracers, mTracerTexts 和 mTracerArrows 长度始终一致
	// 如果 mLineMode 为 lmTracing, 则 mHLines, mVLines 和 mTracers 的长度一致
	// 如果 mLineMode 为 lmFollowCursor, 则 mHLines, mVLines 的长度为 1, mTracers 的长度为 0
	// 如果 mLineMode 为 lmFree, 则 mTracers 的长度为 0

	// 水平线
	QVector<QCPItemLine*> mHLines;
	// 水平线上的文本
	QVector<QCPItemText*> mHTexts;

	// 垂直线
	QVector<QCPItemLine*> mVLines;
	// 垂直线上的文本
	QVector<QCPItemText*> mVTexts;

	// 跟踪点
	QVector<QCPItemTracer*> mTracers;
	// 跟踪点上的文本
	QVector<QCPItemText*> mTracerTexts;
	// 跟踪点上的箭头
	QVector<QCPItemCurve*> mTracerArrows;

	LineMode mLineMode;
	QVector<double> mKeys;
	QVector<double> mValues;
	QStringList mHTextFormats;
	QStringList mVTextFormats;
	QStringList mTracerTextFormats;

	static const QString layer;
	static const QMargins margins;
	QPen mLinePen;
	QPen mLineSelectedPen;
	QColor mTextColor;
	QColor mTextSelectedColor;
};

#endif // CROSSLINE_H
