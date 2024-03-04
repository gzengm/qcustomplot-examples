﻿#ifndef CUSTOMPLOT_H
#define CUSTOMPLOT_H

#include "../lib/qcustomplot.h"
#include "cursorhelper.h"

class CustomPlot : public QCustomPlot
{
	Q_OBJECT

public:
	CustomPlot(QWidget* parent = Q_NULLPTR);
	~CustomPlot() Q_DECL_OVERRIDE;

Q_SIGNALS:
	void itemMoved(QCPAbstractItem* item, QMouseEvent* event);

	// QWidget interface
protected:
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	// QWidget interface
public:
	QSize sizeHint() const Q_DECL_OVERRIDE;

public:
	CursorHelper cursorHelper;
};

#endif // CUSTOMPLOT_H
