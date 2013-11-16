/*
 * RESTExplorerPageController.cpp
 *
 *  Created on: Oct 29, 2013
 *      Author: Livan Yi Du
 */

#include "RESTExplorerPageController.h"
#include <bb/cascades/QmlDocument>
#include <bb/cascades/ActionItem>
#include <bb/cascades/GroupDataModel>
#include <bb/data/JsonDataAccess>
#include <bb/system/SystemListDialog>
#include <bb/system/SystemToast>
#include <QMetaObject>
#include <core/SFGlobal.h>
#include <rest/SFRestAPI.h>
#include <core/SFNetworkAccessTask.h>
#include <core/SFAuthenticationManager.h>
#include <encryption/SFSecurityManager.h>
#include <oauth/SFOAuthCredentials.h>
#include "DialogUtils.h"

namespace sf {
using namespace bb::cascades;
using namespace bb::system;

static const QString HistoryFolderName = "rest_explorer_history";
static const QString HistoryFileName = "method_parameters_history.json";
static const int HistoryEntriesLimit = 50;
static const QString kHistoryMethodName = "HistoryMethodName";
static const QString kHistoryFriandlyMethodName = "HistoryFriandlyMethodName";
static const QString kHistoryInputData = "HistoryInputData";
static const QString kHistoryIdentifier = "HistoryIdentifier";
static const QString kHistoryTimestamp = "HistoryTimestamp";
static const QString kHistoryEntryText = "text";
static const QString kHistoryEntrySubText = "subtext";


RESTExplorerPageController::RESTExplorerPageController() : BaseUIController() {
	mScrollView = NULL;
	mAPINameLabel = NULL;
	mAPISelectorButton = NULL;
	mInitiatorSegControl =  NULL;
	mMethodSegControl  =  NULL;
	mAPISignatureLabel = NULL;
	mFieldsContainer = NULL;
	mResultTextArea = NULL;
	mLoadingIndicator = NULL;
	mAPIOptionsModel = NULL;
	mSelectedAPIIndexPath = QVariantList();
}

RESTExplorerPageController::~RESTExplorerPageController() {

}

/***********************
 * Life cycle
 ***********************/

void RESTExplorerPageController::onRootObjectCreating(bb::cascades::QmlDocument*) {
//	qml->setContextProperty("SFRestAPI", SFRestAPI::instance());
}

void RESTExplorerPageController::onRootObjectAttached() {
	BaseUIController::onRootObjectAttached();

	mScrollView = rootObject()->findChild<ScrollView*>("scrollView");
	mAPINameLabel = rootObject()->findChild<Label*>("apiNameLbl");
	mAPISelectorButton = rootObject()->findChild<Button*>("apiSelectorBtn");
	mInitiatorSegControl =  rootObject()->findChild<SegmentedControl*>("nativeOrQmlSegCtrl");
	mMethodSegControl  =  rootObject()->findChild<SegmentedControl*>("methodSegCtrl");
	mAPISignatureLabel= rootObject()->findChild<Label*>("methodSignatureLbl");
	mFieldsContainer = rootObject()->findChild<Container*>("fieldsContainer");
	mResultTextArea = rootObject()->findChild<TextArea*>("resultTextArea");
	mLoadingIndicator = rootObject()->findChild<ActivityIndicator*>("loadingIndicator");

	//some initialization

	//connect slots
	connect(mAPISelectorButton, SIGNAL(clicked()), this, SLOT(onApiSelectorClicked()));

	Button *button = rootObject()->findChild<Button*>("invokeBtn");
	connect(button, SIGNAL(clicked()), this, SLOT(onInvokeButtonClicked()));

	button = rootObject()->findChild<Button*>("resetBtn");
	connect(button, SIGNAL(clicked()), this, SLOT(onResetButtonClicked()));

	ActionItem *action = rootObject()->findChild<ActionItem*>("invokeAction");
	connect(action, SIGNAL(triggered()), this, SLOT(onInvokeButtonClicked()));

	action = rootObject()->findChild<ActionItem*>("resetAction");
	connect(action, SIGNAL(triggered()), this, SLOT(onResetButtonClicked()));

	action = rootObject()->findChild<ActionItem*>("historyAction");
	connect(action, SIGNAL(triggered()), this, SLOT(onHistoryButtonClicked()));

	connect(rootObject()->findChild<ActionItem*>("clearHistoryAction"), SIGNAL(triggered()), this, SLOT(onDeleteHistoryButtonClicked()));
	connect(rootObject()->findChild<ActionItem*>("revokeAccessAction"), SIGNAL(triggered()), this, SLOT(onRevokeAccessButtonClicked()));
	connect(rootObject()->findChild<ActionItem*>("revokeAllAction"), SIGNAL(triggered()), this, SLOT(onRevokeAllButtonClicked()));
}

/***********************
 * APIs
 ***********************/
void RESTExplorerPageController::saveCurrentFieldValues(const QScriptValue & apiResultString) {
	if (!apiResultString.isString()) {
		//ignore
	}
	QString resultStr = apiResultString.toString();
	this->saveCurrentFieldValues(static_cast<const void *>(&resultStr), QMetaType::QString);
}

/***********************
 * Signal & Slots
 ***********************/
void RESTExplorerPageController::onApiSelectorClicked() {
	this->launchAPISelector();
}

void RESTExplorerPageController::onInvokeButtonClicked() {
	if (mInitiatorSegControl->selectedIndex() != 0) {
		//not native, ignore
		return;
	}
	this->invokeSelectedMethod();
}

void RESTExplorerPageController::onResetButtonClicked() {
	if (mSelectedMethodName.isNull() || mSelectedMethodName.isEmpty()) {
		return;
	}
	this->populateInputFields(QString(mSelectedMethodName));
	mScrollView->scrollToPoint(0, 0 , ScrollAnimation::Smooth);
}

void RESTExplorerPageController::onHistoryButtonClicked() {
	if (mSelectedMethodName.isNull() || mSelectedMethodName.isEmpty()) {
		return;
	}
	this->launchHistorySelector();
}

void RESTExplorerPageController::onDeleteHistoryButtonClicked() {
	this->deleteHistory();
}

void RESTExplorerPageController::onRevokeAccessButtonClicked() {
	const SFOAuthCredentials * credentials = SFRestAPI::instance()->currentCredentials();
	QString accessToken = credentials ? credentials->getAccessToken() : QString();
	if (accessToken.isNull() || accessToken.isEmpty()) {
		return;
	}

	QString path = QString("https://%1/services/oauth2/revoke?token=%2").arg(SFAuthenticationManager::instance()->loginHost(), accessToken);
	SFNetworkAccessTask *task = new SFNetworkAccessTask(getSharedNetworkAccessManager(), path, HTTPMethod::HTTPGet);
	task->putTag("AllTokens", false);
	task->startTaskAsync(this, SLOT(onNetworkResultReady(sf::SFResult*)));
}

void RESTExplorerPageController::onRevokeAllButtonClicked() {
	const SFOAuthCredentials * credentials = SFRestAPI::instance()->currentCredentials();
	QString refreshToken = credentials ? credentials->getRefreshToken() : QString();
	if (refreshToken.isNull() || refreshToken.isEmpty()) {
		return;
	}

	QString path = QString("https://%1/services/oauth2/revoke?token=%2").arg(SFAuthenticationManager::instance()->loginHost(), refreshToken);
	SFNetworkAccessTask *task = new SFNetworkAccessTask(getSharedNetworkAccessManager(), path, HTTPMethod::HTTPGet);
	task->putTag("AllTokens", true);
	task->startTaskAsync(this, SLOT(onNetworkResultReady(sf::SFResult*)));
}

/***********************
 * Privates/Protected
 ***********************/
void RESTExplorerPageController::launchAPISelector() {
	if (!mAPIOptionsModel) {
		mAPIOptionsModel = new XmlDataModel(this);
		mAPIOptionsModel->setSource(QUrl("asset:///RestExplorer/RestAPIOptions.xml"));
	}
	QVariantList oldIndexPath = mSelectedAPIIndexPath;
	mSelectedAPIIndexPath = DialogUtils::execListDialog(qobject_cast<DataModel*>(mAPIOptionsModel), QString("Select API"), mSelectedAPIIndexPath, false);
	QVariant selected = mAPIOptionsModel->data(mSelectedAPIIndexPath);
	if (mSelectedAPIIndexPath.size() == 0 || selected.isNull() || !selected.isValid() || oldIndexPath == mSelectedAPIIndexPath) {
		//cancelled or invalid index or not changed
		return;
	}
	mAPINameLabel->setText(selected.toMap()["text"].toString());
	this->populateInputFields(selected.toMap()["method"].toString());
	mScrollView->scrollToPoint(0, 0 , ScrollAnimation::Smooth);
}

void RESTExplorerPageController::populateInputFields(const QString & apiMethodName) {

	mSelectedMethod = QMetaMethod();
	mSelectedMethodName = QString();
	//TODO recycle current fields
	mFieldsContainer->removeAll();

	qDebug() << "Populating fields for method:" << apiMethodName;
	const QMetaObject *metaObject = SFRestAPI::instance()->metaObject();
	int found = -1;
	for(int i=0; i<metaObject->methodCount(); i++) {
		if (QString(metaObject->method(i).signature()).startsWith(apiMethodName)) {
			found = i;
			break;
		}
	}
	if (found < 0) {
		qWarning() << "[REST Explorer] The selected method"<< apiMethodName <<"is not found or is not declared as Q_INVOKABLE.";
		return;
	}

	mSelectedMethod = metaObject->method(found);
	mSelectedMethodName = apiMethodName;
	if (mSelectedMethod.access() != QMetaMethod::Public) {
		qWarning() << "[REST Explorer] The selected method"<< apiMethodName <<"is not public.";
		return;
	}
	//signature
	mAPISignatureLabel->setText(QString(mSelectedMethod.signature()));
	rootObject()->setProperty("selectedMethodName", apiMethodName);

	//populate fields
	QList<QByteArray> parameterNames = mSelectedMethod.parameterNames();
	QList<QByteArray> parameterTypes = mSelectedMethod.parameterTypes();
	for(QList<QByteArray>::const_iterator iName = parameterNames.constBegin(), iType = parameterTypes.constBegin();
			iName != parameterNames.constEnd() && iType != parameterTypes.constEnd(); iName++, iType++) {

		QString key = QString(*iName);
		QString typeStr = QString(*iType);
		const int typeId = QMetaType::type(iType->constData());

		Control *control = NULL;
		QVariant initialValue;
		switch(typeId) {
		case QMetaType::QString:
		case QMetaType::QByteArray:
			control = loadRootObjectFromQml<Control>("asset:///RestExplorer/ParamFieldText.qml");
			initialValue = "";
			break;
		case QMetaType::QVariantMap:
			control = loadRootObjectFromQml<Control>("asset:///RestExplorer/ParamFieldVariantMap.qml");
			initialValue = QVariantMap();
			break;
		case QMetaType::QVariantList:
		case QMetaType::QStringList:
			control = loadRootObjectFromQml<Control>("asset:///RestExplorer/ParamFieldVariantList.qml");
			initialValue = QVariantList();
			break;
		case QMetaType::Bool:
		case QMetaType::Int:
		case QMetaType::UInt:
		case QMetaType::Short:
		case QMetaType::UShort:
		case QMetaType::LongLong:
		case QMetaType::ULongLong:
		case QMetaType::Double:
		case QMetaType::Float:
			control = loadRootObjectFromQml<Control>("asset:///RestExplorer/ParamFieldNumber.qml");
			initialValue = 0;
			break;
		case QMetaType::User:
		default:
			//poputlate supported Custom Types
			if (!loadFieldForCustomType(*iType, &control, initialValue)) {
				//ignore non-supported type
				qWarning() << "[REST Explorer] Parameter type" << typeStr <<"is not supported.";
			}
			break;
		}

		if (control) {
			control->setProperty("key", key);
			control->setProperty("initialValue", initialValue);
			mFieldsContainer->add(control);
		}

	}
}

Control *RESTExplorerPageController::loadRootObjectFromQml(const QString & path) {
	QmlDocument *qml = QmlDocument::create(path);
	Control *obj = qml->createRootObject<Control>();
	qml->setParent(obj);
	return obj;
}

bool RESTExplorerPageController::loadFieldForCustomType(const char *typeName, Control **outFieldPtr, QVariant & outInitialValueRef) {
	if (!outFieldPtr) {
		return false;
	}
	Control *control;
	QString typeNameStr = QString(typeName);
	if (typeNameStr.indexOf("HTTPMethod", 0, Qt::CaseInsensitive) >= 0) {
		control = loadRootObjectFromQml<Control>("asset:///RestExplorer/ParamFieldEnum.qml");
		QVariantList options;
		options << QVariant(QVariantList() << "Get" << (int)HTTPMethod::HTTPGet);
		options << QVariant(QVariantList() << "Post" << (int)HTTPMethod::HTTPPost);
		options << QVariant(QVariantList() << "Patch" << (int)HTTPMethod::HTTPPatch);
		options << QVariant(QVariantList() << "Delete" << (int)HTTPMethod::HTTPDelete);

		control->setProperty("options", options);
		outInitialValueRef = (int)HTTPMethod::HTTPPost;
		*outFieldPtr = control;
		return true;
	} else if (typeNameStr.indexOf("HTTPContentType", 0, Qt::CaseInsensitive) >= 0) {
		control = loadRootObjectFromQml<Control>("asset:///RestExplorer/ParamFieldEnum.qml");
		QVariantList options;
		options << QVariant(QVariantList() << "Url Encoded" << SFRestRequest::HTTPContentTypeUrlEncoded);
		options << QVariant(QVariantList() << "JSON" << SFRestRequest::HTTPContentTypeJSON);
		options << QVariant(QVariantList() << "Multi-Part" << SFRestRequest::HTTPContentTypeMultiPart);

		control->setProperty("options", options);
		outInitialValueRef = SFRestRequest::HTTPContentTypeJSON;
		*outFieldPtr = control;
		return true;
	} else {
		return false;
	}

	return false;
}

void RESTExplorerPageController::invokeSelectedMethod() {
	if (!mSelectedMethod.enclosingMetaObject()) {
		return;
	}
	qDebug() << "Signature:" << mSelectedMethod.signature();

	mLoadingIndicator->setRunning(true);
	QString msg = "";
	QTextStream output(&msg);
	mResultTextArea->setText("");

	//construct arguments
	const int ArgsCount = 10;
	QList<ArgumentHolder> args = prepareAPIArguments(ArgsCount);

	//construct return argument
	ArgumentHolder returnArg = prepareAPIReturnArgument(&msg);
	if (!returnArg.isValid()) {
		qWarning() << msg;
		mResultTextArea->setText(msg);
		mLoadingIndicator->setRunning(false);
		return;
	}

	//invoke the method
	bool ret = mSelectedMethod.invoke(SFRestAPI::instance(), returnArg.returnQArg(), args[0].qArg(),
			args[1].qArg(), args[2].qArg(), args[3].qArg(),
			args[4].qArg(), args[5].qArg(), args[6].qArg(),
			args[7].qArg(), args[8].qArg(), args[9].qArg());
	if (!ret) {
		qWarning() << "[C++] Failed to execute API:" << mSelectedMethod.signature();
		output << "[C++] Failed to execute API: " << mSelectedMethod.signature();
		mResultTextArea->setText(msg);
		mLoadingIndicator->setRunning(false);
		return;
	}

	//get the result
	void * pResult = returnArg.pData;
	const int returnTypeId = returnArg.mDataType;

	//save history
	this->saveCurrentFieldValues(pResult, returnTypeId);

	//deal with the result;
	if (returnTypeId == qMetaTypeId<sf::SFRestRequest*>()) {
		//it's a request, send it
		SFRestRequest * request = (*static_cast<SFRestRequest**>(pResult));
		qWarning() << "[C++] Direct API Result:" << request->staticMetaObject.className() << "\n[C++] Sending request...";
		output << "[C++] Direct API Result: " << request->staticMetaObject.className() << "\n[C++] Sending request...";
		SFRestAPI::instance()->sendRestRequest(request, this, SLOT(onRestResultReady(sf::SFResult *)));
	} else {
		//assume the result can be converted to QString
		qDebug() << "pResult=" << pResult;
		QVariant result(QVariant::String, pResult);
		qWarning() << "[C++] Direct API Result:\n" << result;
		output << "[C++] Direct API Result:\n" << result.toString();
		mResultTextArea->setText(mResultTextArea->text() + msg);
		mLoadingIndicator->setRunning(false);
	}
}

QList<ArgumentHolder> RESTExplorerPageController::prepareAPIArguments(const int & ArgsCount) {
	QList<QByteArray> parameterTypes = mSelectedMethod.parameterTypes();
	QList<ArgumentHolder> args;
	for(int i=0; i<ArgsCount; i++) {
		if (i >= mFieldsContainer->count() || i >= parameterTypes.count()) {
			args.append(ArgumentHolder());
			continue;
		}
		//get qvariant value from the field
		Control *control = mFieldsContainer->at(i);
		QVariant value;
		control->metaObject()->invokeMethod(control, "value", Q_RETURN_ARG(QVariant, value));
		qDebug() << "Type:" << parameterTypes[i].data() << "QVariant Value:" << value << " TypeId: " << QMetaType::type(parameterTypes[i].data());

		//extract the content and create argument
		args.append(ArgumentHolder(parameterTypes[i], value));
	}
	return args;
}

ArgumentHolder RESTExplorerPageController::prepareAPIReturnArgument(QString * outMsgPtr) {
	//identify return type
	const int returnTypeId = QMetaType::type(mSelectedMethod.typeName());
	qDebug() << "ReturnType:" << mSelectedMethod.typeName() << "ReturnTypeId" << returnTypeId;
	if (!QMetaType::isRegistered(returnTypeId)) {
		if (!outMsgPtr) *outMsgPtr = "[C++] Unregistered return type. Did you forget Q_DECLARE_METATYPE?";
		return ArgumentHolder();
	}

	//create return argument
	return ArgumentHolder(QByteArray(mSelectedMethod.typeName()));
}

void RESTExplorerPageController::onRestResultReady(sf::SFResult *result) {
	QString msg = "";
	QTextStream output(&msg);
	if (!result->hasError()) {
		qDebug() << "[C++] API Response: Code =" << result->code() << "\nMessage: " << result->message();
		qDebug() << "[C++] Payload:" << result->payload<QVariant>();
		bb::data::JsonDataAccess jda;
		QByteArray buffer;
		jda.saveToBuffer(result->payload<QVariant>(), &buffer);
		output <<"[C++] API Response: Code = " << result->code() << "\nMessage: " << result->message() << "\n[C++] Payload:" << QString(buffer) << "\n";
	} else {
		qDebug() << "[C++] API Response: ErrorCode = " << result->code() << "\nErrorMessage: " << result->message();
		output << "[C++] API Response: ErrorCode = " << result->code() << "\nErrorMessage: " << result->message() << "\n";
	}
	mResultTextArea->setText(mResultTextArea->text() + msg);
	result->deleteLater();
	mLoadingIndicator->setRunning(false);
}

void RESTExplorerPageController::onNetworkResultReady(sf::SFResult *result) {
	bb::system::SystemToast *toast = new bb::system::SystemToast(this);
	if (result->getTag<bool>("AllTokens")) {
		toast->setBody("Tokens revoked.");
	} else {
		toast->setBody("Access Token revoked.");
	}
	toast->setBody("Token(s) revoked.");
	toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
	connect(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), toast, SLOT(deleteLater()));
	toast->show();
}

QString RESTExplorerPageController::filepathForStoredValues(const bool & createIfNotExist) {
	if (!mSelectedMethod.enclosingMetaObject() || mSelectedMethodName.isNull() || mSelectedMethodName.isEmpty() || mSelectedMethod.parameterNames().isEmpty()) {
		return QString();
	}

	//find the file or create
	QDir folder = QDir::homePath();
	if (!folder.exists(HistoryFolderName)) {
		if (!createIfNotExist) {
			return QString();
		}
		folder.mkdir(HistoryFolderName);
	}
	folder.cd(HistoryFolderName);
	if (!createIfNotExist && !folder.exists(HistoryFileName)) {
		return QString();
	}
	return folder.filePath(HistoryFileName);
}

void RESTExplorerPageController::launchHistorySelector() {
	using namespace bb::data;
	//find the file
	QString path = filepathForStoredValues(false);
	if (path.isNull() || path.isEmpty()) {
		return;
	}
	QVariantList entries;
	try {
		JsonDataAccess jda;
		QFile file(path, this);
		file.open(QIODevice::ReadOnly);
		entries = jda.load(&file).toList();
		file.close();
	} catch(...) {
		qWarning() << "Error when try to load history file" << path;
	}
	if (entries.count() == 0) {
		return;
	}

	//it's not exception safe, but we assume it won't throw
	QStringList groupingKeys = QStringList() << kHistoryFriandlyMethodName << kHistoryMethodName << kHistoryTimestamp;
	GroupDataModel *dataModel = new GroupDataModel(groupingKeys, this);
	dataModel->setSortedAscending(false);
	dataModel->setGrouping(ItemGrouping::ByFullValue);
	for(QVariantList::const_iterator i = entries.constBegin(); i != entries.constEnd(); i++) {
		QVariantMap entry = i->toMap();
		entry[kHistoryEntrySubText] = entry[kHistoryTimestamp].toDateTime().toString("yyyy-MM-dd HH:mm:ss");
		dataModel->insert(entry);
	}
	QVariantList selectedPath = DialogUtils::execListDialog(qobject_cast<DataModel*>(dataModel), QString("History"), QVariantList(), true);
	QVariant selected = dataModel->data(selectedPath);
	dataModel->deleteLater();
	if (selectedPath.size() == 0 || selected.isNull() || !selected.isValid()) {
		//cancelled or invalid index or not changed
		return;
	}

	loadValuesIntoFields(selected.toMap()[kHistoryInputData].toMap());
	mScrollView->scrollToPoint(0, 0 , ScrollAnimation::Smooth);
}

void RESTExplorerPageController::saveCurrentFieldValues(const void * pApiResult, const int & typeId) {
	using namespace bb::data;

	//find the file or create
	QString path = filepathForStoredValues(true);
	if (path.isNull() || path.isEmpty()) {
		return;
	}

	try {
		JsonDataAccess jda;
		QFile file(path, this);

		//construct result text
		QString recordText;
		if (pApiResult && typeId == qMetaTypeId<sf::SFRestRequest*>()) {
			//it's a request
			SFRestRequest * request = (*static_cast<SFRestRequest * const*>(pApiResult));
			if (request) {
				recordText = request->path() + "?";
				QVariantMap params = request->requestParams();
				for (QVariantMap::const_iterator i = params.constBegin(); i!=params.constEnd(); i++) {
					recordText += i.key() + "=" + i.value().toString() + "&";
				}
			} else {
				recordText = "SFRestRequest (NULL)";
			}
		} else if (pApiResult) {
			//assume the result can be converted to QString
			QVariant result(QVariant::String, pApiResult);
			recordText = result.toString();
		}

		//extract value and construct save data;
		QVariantMap fieldValues;
		for(int i=0; i<mFieldsContainer->count(); i++) {
			//get qvariant value from the field
			Control *control = mFieldsContainer->at(i);
			QString key = control->property("key").toString();
			QVariant value;
			control->metaObject()->invokeMethod(control, "value", Q_RETURN_ARG(QVariant, value));
			fieldValues[key] = value;
		}

		//generate identifier
		QString idFeed;
		QString identifier = "";
		jda.saveToBuffer(fieldValues, &idFeed);
		if (!jda.hasError() && !idFeed.isEmpty()) {
			identifier = SFSecurityManager::instance()->hash(mSelectedMethodName+"\n"+idFeed);
		} else {
			identifier = SFSecurityManager::instance()->hash(mSelectedMethodName);
		}

		QVariantMap entry;
		entry[kHistoryInputData] = fieldValues;
		entry[kHistoryMethodName] = mSelectedMethodName;
		entry[kHistoryIdentifier] = identifier;
		entry[kHistoryEntryText] = recordText;
		entry[kHistoryFriandlyMethodName] = mAPINameLabel->text();
		entry[kHistoryTimestamp] = QDateTime::currentDateTime();

		//extract existing list
		file.open(QIODevice::ReadOnly);
		QVariantList entries = jda.load(&file).toList();
		file.close();

		//replace or insert
		int i = 0;
		for(i = 0; i<entries.count(); i++) {
			if (entries[i].toMap()[kHistoryIdentifier] == identifier) {
				entries.replace(i, entry);
				break;
			}
		}
		if (i >= entries.count()) {
			entries << entry;
		}

		//control max list size
		if (entries.count() > HistoryEntriesLimit) {
			entries.removeFirst();
		}

		//save back to file
		file.open(QIODevice::ReadWrite);
		jda.save(QVariant(entries), &file);
		file.flush();
		file.close();

		//for debug
		file.open(QIODevice::ReadOnly);
		QString text = QString(file.readAll());
		qDebug() << "Saved As" << file.fileName() << "\n" << text;
		file.close();

	} catch(...) {
		qWarning() << "Error when try to save history file" << path;
	}
}

void RESTExplorerPageController::loadValuesIntoFields(const int & historyIndex) {
	using namespace bb::data;
	//find the file
	QString path = filepathForStoredValues(false);
	if (path.isNull() || path.isEmpty()) {
		return;
	}
	try {
		JsonDataAccess jda;
		QFile file(path, this);
		file.open(QIODevice::ReadOnly);
		QVariantList historyEntries = jda.load(&file).toList();
		QVariantMap entry;
		if (historyEntries.count() == 0) {
			return;
		} else if (historyIndex < 0 || historyIndex >= historyEntries.count()) {
			//default to the last one
			entry = historyEntries.last().toMap();
		} else {
			entry = historyEntries[historyIndex].toMap();
		}
		QVariantMap fieldValues = entry[kHistoryInputData].toMap();
		loadValuesIntoFields(fieldValues);
		file.close();
	} catch(...) {
		qWarning() << "Error when try to load history file" << path;
	}
}

void RESTExplorerPageController::loadValuesIntoFields(const QVariantMap & fieldValues) {
	for(int i=0; i<mFieldsContainer->count(); i++) {
		//get qvariant value from the field
		Control *control = mFieldsContainer->at(i);
		QString key = control->property("key").toString();
		QVariant value = fieldValues[key];
		control->setProperty("initialValue", value);
	}
}

void RESTExplorerPageController::deleteHistory() {
	QDir folder = QDir::homePath();
	if (!folder.exists(HistoryFolderName)) {
		folder.mkdir(HistoryFolderName);
	}
	folder.cd(HistoryFolderName);
	if (folder.exists(HistoryFileName)) {
		folder.remove(HistoryFileName);
	}

	bb::system::SystemToast *toast = new bb::system::SystemToast(this);
	toast->setBody("History cleared.");
	toast->setPosition(bb::system::SystemUiPosition::BottomCenter);
	connect(toast, SIGNAL(finished(bb::system::SystemUiResult::Type)), toast, SLOT(deleteLater()));
	toast->show();
}

/*************************
 * static
 *************************/



} /* namespace sf */
