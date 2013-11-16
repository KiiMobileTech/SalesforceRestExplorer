/*
 * ArgumentHolder.h
 *
 *  Created on: Nov 4, 2013
 *      Author: Livan Yi Du
 */

#ifndef ARGUMENTHOLDER_H_
#define ARGUMENTHOLDER_H_
#include <QMetaType>
#include <QVariant>
#include <QDebug>

namespace sf {

/*
 * Helper class
 * This is a argument holder class. The purpose of having this wrapper is
 * to guarantee that name & data is valid and destroyed properly
 */
class ArgumentHolder {
public:
	/* This class is intened to be used only in RESTExplorerPageController class */
	friend class RESTExplorerPageController;

	/* custom copy constructor, note that we never copy data pointer & data type, data will be allocated when it's needed */
	ArgumentHolder(const ArgumentHolder & holder) : mType(holder.mType), mValue(holder.mValue), pData(NULL), mDataType(QMetaType::Void) {};

	/* custom assignment, same deal, we don't copy data pointer & data type */
	ArgumentHolder & operator=(ArgumentHolder rhs) {
		std::swap(mType, rhs.mType);
		std::swap(mValue, rhs.mValue);
		return *this;
	}
	~ArgumentHolder() {
		destroyCopy(mDataType, pData);
	}
private:
	ArgumentHolder() : mType(), mValue(), pData(NULL), mDataType(QMetaType::Void) {};
	explicit ArgumentHolder(const QByteArray & type, const QVariant & value = QVariant()) : mType(type), mValue(value), pData(NULL) , mDataType(QMetaType::Void) {};

	inline QGenericArgument qArg() {
		if (!pData) {
			pData = createCopy(mType.constData(), mValue, mDataType);
		}
		return QGenericArgument(mType.constData(), pData);
	};

	inline QGenericReturnArgument returnQArg() {
		if (!pData) {
			mDataType = QMetaType::type(mType.constData());
			pData = QMetaType::construct(mDataType);
		}
		return QGenericReturnArgument(mType.constData(), pData);
	};

	inline bool isValid() { return !mType.isNull(); /* we don't check if type is valid*/ };
private:
	QByteArray mType;
	QVariant mValue;
	void * pData;
	int mDataType; // this represent the type of pData pointed to

	/*
	 * This function is converted from QVariant::value() implementation
	 */
	inline static void * createCopy(const char * typeName, const QVariant &v, int & outTypeIdRef) {
		outTypeIdRef = QMetaType::type(typeName);
		if (outTypeIdRef == v.userType()) {
			return QMetaType::construct(outTypeIdRef, v.constData());
		}
		if (outTypeIdRef < int(QMetaType::User)) {
			void * p = QMetaType::construct(outTypeIdRef);
			if (qvariant_cast_helper(v, QVariant::Type(outTypeIdRef), p)) {
				return p;
			}
		}

		//unregistered type, we have to assume it's enum type or developer forgot to register
		if (v.isValid()) {
			//we use whatever type the QVariant was constucted from
			outTypeIdRef = v.type();
			return QMetaType::construct(QMetaType::Type(outTypeIdRef), v.constData());
		}
		outTypeIdRef = QMetaType::Void;
		return QMetaType::construct(QMetaType::Type(outTypeIdRef));
	}

	inline static void destroyCopy(const int & typeId, void * data) {
		if (data) {
			QMetaType::destroy(typeId, data);
		}
	}
};

} /* namespace sf */
#endif /* ARGUMENTHOLDER_H_ */
