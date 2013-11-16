/*
 * BaseUIController.h
 *
 *  Created on: Mar 4, 2013
 *      Author: Livan Yi Du
 */

#ifndef BASEUICONTROLLER_H_
#define BASEUICONTROLLER_H_

#include <qobject.h>
#include <bb/cascades/BaseObject>
#include <bb/cascades/AbstractPane>
#include <bb/cascades/Tab>
#include <bb/cascades/NavigationPane>
#include <bb/cascades/UIObject>
#include <bb/cascades/QmlDocument>

using namespace bb::cascades;

namespace sf {
class BaseUIController : public BaseObject {
	Q_OBJECT
	Q_PROPERTY(bb::cascades::UIObject* root READ rootObject);
signals:
	void rootObjectCreating(bb::cascades::QmlDocument *qml);
	void rootObjectAttached();
	void rootObjectDetached();
protected slots:
	virtual void onRootObjectCreating(bb::cascades::QmlDocument *qml);
	virtual void onRootObjectAttached();
	virtual void onRootObjectDetached();
public:
	BaseUIController();
	virtual ~BaseUIController();
	virtual UIObject* rootObject();
	Q_INVOKABLE virtual void load(const QString &qmlAsset);
	Q_INVOKABLE virtual void attachRoot(UIObject* pane);
	virtual void detachRoot();
	template<class RootType>
	RootType* rootObject() { return qobject_cast<RootType*>(pRootObject); }; // this is templated function that auto-cast root object
	template<class ParentType>
	ParentType* findRootParent(const QString & objectName = "") {
		QObject *currentNode = pRootObject;
		while ((currentNode = currentNode->parent())) {
			ParentType* obj = qobject_cast<ParentType*>(currentNode);
			if (obj && (objectName.length() == 0 || objectName == obj->objectName())) {
				return obj;
			}
		}
		return NULL;
	}
private:
	UIObject *pRootObject;
};

} /* namespace sf */
#endif /* PAGECONTROLLER_H_ */
