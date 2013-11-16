/*
 * RESTExplorerPageController.h
 *
 *  Created on: Oct 29, 2013
 *      Author: Livan Yi Du
 */

#ifndef RESTEXPLORERPAGECONTROLLER_H_
#define RESTEXPLORERPAGECONTROLLER_H_

#include "BaseUIController.h"
#include <bb/cascades/Button>
#include <bb/cascades/SegmentedControl>
#include <bb/cascades/Label>
#include <bb/cascades/Container>
#include <bb/cascades/TextArea>
#include <bb/cascades/ScrollView>
#include <bb/cascades/ActivityIndicator>
#include <bb/system/SystemListDialog>
#include <bb/cascades/XmlDataModel>
#include <QMetaMethod>
#include "ArgumentHolder.h"
#include <QtScript/QtScript>

namespace sf {
class SFResult;
class SFRestRequest;
using namespace bb::cascades;
using namespace bb::system;

class RESTExplorerPageController : public BaseUIController {
	Q_OBJECT

public slots:
	void onApiSelectorClicked();
	void onInvokeButtonClicked();
	void onResetButtonClicked();
	void onHistoryButtonClicked();
	void onDeleteHistoryButtonClicked();
	void onRevokeAccessButtonClicked();
	void onRevokeAllButtonClicked();

protected slots:
	virtual void onRootObjectCreating(bb::cascades::QmlDocument*);
	virtual void onRootObjectAttached();
	void onRestResultReady(sf::SFResult *result);
	void onNetworkResultReady(sf::SFResult *result);

public:
	RESTExplorerPageController();
	virtual ~RESTExplorerPageController();

	/* History */
	Q_INVOKABLE void saveCurrentFieldValues(const QScriptValue & apiResultString);

protected:
	ScrollView *mScrollView;
	Label *mAPINameLabel;
	Button *mAPISelectorButton;
	SegmentedControl *mInitiatorSegControl;
	SegmentedControl *mMethodSegControl;
	Label *mAPISignatureLabel;
	Container *mFieldsContainer;
	TextArea *mResultTextArea;
	ActivityIndicator *mLoadingIndicator;
	XmlDataModel *mAPIOptionsModel;
	QVariantList mSelectedAPIIndexPath;
	QString mSelectedMethodName;
	QMetaMethod mSelectedMethod;

	/* API selection and Field population */
	void launchAPISelector();
	void populateInputFields(const QString & apiMethodName);
	template <class T>
	T *loadRootObjectFromQml(const QString & path) {
		return qobject_cast<T*>(loadRootObjectFromQml(path));
	}
	Control *loadRootObjectFromQml(const QString & path);
	bool loadFieldForCustomType(const char *typeName, Control **outFieldPtr, QVariant & outInitialValueRef);

	/* dynamic API invokation */
	void invokeSelectedMethod();

	QList<ArgumentHolder> prepareAPIArguments(const int & ArgsCount);
	ArgumentHolder prepareAPIReturnArgument(QString * outMsgPtr);

	/* History */
	QString filepathForStoredValues(const bool & createIfNotExist = true);
	void launchHistorySelector();
	void saveCurrentFieldValues(const void * pApiResult, const int & typeId);
	void loadValuesIntoFields(const int & historyIndex = -1);
	void loadValuesIntoFields(const QVariantMap & fieldValues);
	void deleteHistory();
};



} /* namespace sf */
#endif /* RESTEXPLORERPAGECONTROLLER_H_ */
