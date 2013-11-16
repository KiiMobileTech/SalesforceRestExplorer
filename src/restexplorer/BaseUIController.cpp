/*
 * BaseUIController.cpp
 *
 *  Created on: Mar 4, 2013
 *      Author: Livan Yi Du
 */

#include "BaseUIController.h"
#include <qobject.h>

namespace sf {
using namespace bb::cascades;
BaseUIController::BaseUIController() : BaseObject() {
	pRootObject = NULL;
	connect(this, SIGNAL(rootObjectCreating(bb::cascades::QmlDocument*)), this, SLOT(onRootObjectCreating(bb::cascades::QmlDocument*)));
	connect(this, SIGNAL(rootObjectDetached()), this, SLOT(onRootObjectDetached()));
	connect(this, SIGNAL(rootObjectAttached()), this, SLOT(onRootObjectAttached()));
}

BaseUIController::~BaseUIController() {
	// TODO Auto-generated destructor stub
}

void BaseUIController::load(const QString &qmlAsset) {

	//load qml
	QmlDocument *qml = QmlDocument::create(QString("asset:///").append(qmlAsset));
	qml->setParent(this);
	qml->setContextProperty("pageController", this);
	emit rootObjectCreating(qml);

	// create root object for the UI
	UIObject *obj = qml->createRootObject<UIObject>();
	qml->setParent(obj);

	this->attachRoot(obj);
}

void BaseUIController::attachRoot(UIObject* obj) {
	//break existing link with original root view
	this->detachRoot();

	//attach the new one
	pRootObject = obj;
	this->setParent(pRootObject);

	//notify
	if (pRootObject) {
		emit rootObjectAttached();
	}
}

void BaseUIController::detachRoot() {
	bool hadRoot = (pRootObject);
	pRootObject = NULL;
	this->setParent(NULL);
	if (hadRoot) {
		emit rootObjectDetached();
	}
}

UIObject* BaseUIController::rootObject() {
	return pRootObject;
}

//template<class RootType>
//RootType* BaseUIController::rootObject<>() {
//	return pRootObject;
//}

/*
 * Slots
 */
void BaseUIController::onRootObjectCreating(bb::cascades::QmlDocument*) {
	qDebug() << "onRootObjectCreating()";
}

void BaseUIController::onRootObjectAttached() {
	qDebug() << "onRootObjectAttached(): " << pRootObject;
}

void BaseUIController::onRootObjectDetached() {
	qDebug() << "onRootObjectDetached(): " << pRootObject;
}

} /* namespace kiimobile */
