/*
 * AbstractVMObject.h
 *
 *  Created on: 10.03.2011
 *      Author: christian
 */

#ifndef ABSTRACTOBJECT_H_
#define ABSTRACTOBJECT_H_
#include "../misc/defs.h"
#include "ObjectFormats.h"
#include "../memory/Heap.h"

/*
 * macro for padding - only word-aligned memory must be allocated
 */
#define PAD_BYTES(N) ((sizeof(void*) - ((N) % sizeof(void*))) % sizeof(void*))

class VMClass;
class VMObject;
class VMSymbol;

#include <iostream>
using namespace std;

//this is the base class for all VMObjects
class AbstractVMObject {
protected:
	int32_t gcfield;
public:
	virtual int32_t GetHash();
	virtual pVMClass GetClass() const = 0;
	virtual pVMObject Clone() const = 0;
	virtual void Send(StdString, pVMObject*, int);
	int32_t GetGCField() const;
	void SetGCField(int32_t);
	virtual int32_t GetObjectSize() const = 0;
	inline virtual void SetObjectSize(size_t size) {
		cout << "this object doesn't support SetObjectSize" << endl;
		throw "this object doesn't support SetObjectSize";
	}
	inline virtual int GetNumberOfFields() const {
		cout << "this object doesn't support GetNumberOfFields" << endl;
		throw "this object doesn't support GetNumberOfFields";
	}
	virtual void SetNumberOfFields(int nof) {
		cout << "this object doesn't support SetNumberOfFields" << endl;
		throw "this object doesn't support SetNumberOfFields";
	}
	inline virtual void SetClass(pVMClass cl) {
		cout << "this object doesn't support SetClass" << endl;
		throw "this object doesn't support SetClass";
	}
	int GetFieldIndex(pVMSymbol fieldName) const;
	inline virtual void SetField(int index, pVMObject value) {
		cout << "this object doesn't support SetField" << endl;
		throw "this object doesn't support SetField";
	}
	virtual pVMObject GetField(int index) const;
	inline virtual void WalkObjects(pVMObject (pVMObject)) {
		return;
	}
	inline virtual pVMSymbol GetFieldName(int index) const {
		cout << "this object doesn't support GetFieldName" << endl;
		throw "this object doesn't support GetFieldName";
	}

	void* operator new(size_t numBytes, Heap* heap,
			unsigned int additionalBytes = 0) {
		void* mem = (void*) heap->AllocateObject(numBytes + additionalBytes);
		return mem;
	}
};

#endif /* ABSTRACTOBJECT_H_ */