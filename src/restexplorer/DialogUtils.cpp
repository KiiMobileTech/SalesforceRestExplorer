/*
 * DialogUtils.cpp
 *
 *  Created on: Apr 2, 2013
 *      Author: Livan Yi Du
 */

#include "DialogUtils.h"
#include <bb/cascades/QmlDocument>
#include <bb/Application>
#include <bb/cascades/ListView>
#include <bb/cascades/ArrayDataModel>
#include <bb/cascades/DateTimePicker>
#include <QEventLoop>

namespace sf {
using namespace bb::cascades;
using namespace bb::system;

Dialog * DialogUtils::mLoadingDialog = NULL;

DialogUtils::DialogUtils() {

}

DialogUtils::~DialogUtils() {
}

Dialog * DialogUtils::sharedLoadingDialog(const QString & text) {
	if (!mLoadingDialog) {
		QmlDocument *qml = QmlDocument::create("asset:///LoadingDialog.qml");
		mLoadingDialog = qml->createRootObject<bb::cascades::Dialog>();
		mLoadingDialog->setParent(bb::Application::instance());
		qml->deleteLater();
	}
	mLoadingDialog->setProperty("text", text);
	return mLoadingDialog;
}

QVariantList DialogUtils::execListDialog(DataModel *dataModel, const QString & title, const QVariantList & selectedIndexPath, bool autoDismiss) {
	if (dataModel == NULL) {
		return QVariantList();
	}
	QmlDocument *qml = QmlDocument::create("asset:///RestExplorer/ListDialog.qml");
	Dialog *dialog = qml->createRootObject<bb::cascades::Dialog>();
	qml->deleteLater();
	dialog->setProperty("title", title);
	dialog->setProperty("autoDismiss", autoDismiss);
	ListView *list = dialog->findChild<ListView*>("list");
	if (list) {
		list->setDataModel(dataModel);
	}
	dialog->setProperty("selectedIndexPath", selectedIndexPath);

	QEventLoop loop;
	loop.connect(dialog, SIGNAL(closed()), &loop, SLOT(quit()));
	dialog->open();
	loop.exec();
	QVariantList indexPath = dialog->property("selectedIndexPath").toList();
	dialog->deleteLater();
	return indexPath;
}

} /* namespace kiimobile */
