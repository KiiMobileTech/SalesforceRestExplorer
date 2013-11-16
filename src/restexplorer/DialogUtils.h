/*
 * DialogUtils.h
 *
 *  Created on: Apr 2, 2013
 *      Author: Livan Yi Du
 */

#ifndef DIALOGUTILS_H_
#define DIALOGUTILS_H_

#include <bb/cascades/Dialog>
#include <bb/system/SystemListDialog>
#include <bb/cascades/DataModel>

namespace sf {
using namespace bb::cascades;
using namespace bb::system;

class DialogUtils {
public:
	virtual ~DialogUtils();
	static bb::cascades::Dialog * sharedLoadingDialog(const QString & text = "Loading...");
	static QVariantList execListDialog(DataModel *dataModel, const QString & title, const QVariantList & selectedIndexPath = QVariantList(), bool autoDismiss = true);

protected:
	DialogUtils();
	static bb::cascades::Dialog * mLoadingDialog;
};

} /* namespace kiimobile */
#endif /* DIALOGUTILS_H_ */
