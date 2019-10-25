/*******************************************************************************
 *
 * (c) Copyright IBM Corp. 2016, 2016
 *
 *  This program and the accompanying materials are made available
 *  under the terms of the Eclipse Public License v1.0 and
 *  Apache License v2.0 which accompanies this distribution.
 *
 *      The Eclipse Public License is available at
 *      http://www.eclipse.org/legal/epl-v10.html
 *
 *      The Apache License v2.0 is available at
 *      http://www.opensource.org/licenses/apache2.0.php
 *
 * Contributors:
 *    Multiple authors (IBM Corp.) - initial implementation and documentation
 *******************************************************************************/

#ifndef BYTECODEHELPER_INCL
#define BYTECODEHELPER_INCL

#include "VMFrame.h"

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <dlfcn.h>
#include <errno.h>

class BytecodeHelper {

public:
	static constexpr const char* BYTECODEHELPER_FILE = __FILE__;

	static int64_t getClass(int64_t* object);
	static const char* GET_CLASS_LINE;

	static int64_t getSuperClass(int64_t object);
	static const char* GET_SUPER_CLASS_LINE;

        static void getGlobal(int64_t interp, int64_t framePtr, int64_t receiverPtr, int64_t* globalNamePtr);
	static const char* GET_GLOBAL_LINE;

	static int64_t getNewBlock(VMFrame* frame, int64_t* blockMethod, int64_t numOfArgs);
	static const char* GET_NEW_BLOCK_LINE;

	static int64_t newInteger(int64_t value);
	static const char* NEW_INTEGER_LINE;

	static int64_t newDouble(double value);
	static const char* NEW_DOUBLE_LINE;

	static int64_t newArray(int64_t value);
	static const char* NEW_ARRAY_LINE;

	static int64_t* getFieldFrom(VMObject* selfPtr, int64_t fieldIndex);
	static const char* GET_FIELD_FROM_LINE;

	static void	setFieldTo(int64_t selfPtr, int64_t fieldIndex, int64_t* valuePtr);
	static const char* SET_FIELD_TO_LINE;

	static int64_t getInvokable(int64_t receiverClazz, int64_t* signature);
	static const char* GET_INVOKABLE_LINE;

	static int64_t doSendIfRequired(int64_t* interp, VMFrame* framePtr, int64_t invokablePtr, int64_t* receiverPtr, int64_t* signaturePtr, int64_t bytecodeIndex);
	static const char* DO_SEND_IF_REQUIRED_LINE;

	static int64_t doInlineSendIfRequired(int64_t interp, int64_t framePtr, int64_t invokablePtr, int64_t receiverPtr, int64_t signaturePtr, int64_t bytecodeIndex);
	static const char* DO_INLINE_SEND_IF_REQUIRED_LINE;

	static VMFrame* allocateVMFrame(int64_t interp, int64_t previousFramePtr, int64_t methodPtr, int64_t argumentsPtr, int64_t localsPtr, int64_t** stackPtr, int64_t bytecodeIndex, int64_t recursiveLevel);
	static const char* ALLOCATE_VMFRAME_LINE;

	static int64_t doSuperSendHelper(int64_t* interp, VMFrame* frame, int64_t* signaturePtr, int64_t bytecodeIndex);
	static const char* DO_SUPER_SEND_HELPER_LINE;

	static void popFrameAndPushResult(int64_t* interp, VMFrame* frame, int64_t* result);
	static const char* POP_FRAME_AND_PUSH_RESULT_LINE;

	static int64_t popToContext(int64_t* interp, VMFrame* frame);
	static const char* POP_TO_CONTEXT_LINE;

	static int64_t printObject(int64_t objectPtr, int64_t object2Ptr, int64_t signaturePtr);
        static const char* PRINT_OBJECT_LINE;

        static int64_t invokeHelper(int64_t* interp, VMFrame* framePtr, int64_t invokablePtr, int64_t bytecodeIndex);
        static const char *INVOKE_HELPER_LINE;
        static int64_t getInvokableByDispatch(int64_t receiverPtr, int64_t signature, int8_t code);
        static const char* GET_INVOKABLE_BY_DISPATCH_LINE;
        static int64_t getAddressOfDispatchTable(int64_t classPtr);
        static const char* GET_ADDRESS_OF_DISPATCH_TABLE_LINE;
        static int64_t selectorMismatchHandler(int64_t card, int64_t classPtr);
        static const char* SELECTOR_MISMATCH_HANDLER_LINE;
        static void patchDispatchTableLoad(uint64_t card, uint64_t code);
        static const char* PATCH_DISPATCH_TABLE_LOAD_LINE;
        static int64_t getInvokableCard(int64_t invokablePtr);
        static const char* GET_INVOKABLE_CARD_LINE;
        static void handleUnknownGlobal(int64_t interp, int64_t framePtr, int64_t receiverPtr, int64_t globalNamePtr);
        static const char* HANDLE_UNKNOWN_GLOBAL_LINE;
        
        static void printString(int64_t stringPointer);
        static const char* PRINTSTRING_LINE;
        static void printInt64(int64_t value);
        static const char* PRINTINT64_LINE;
        static void printInt64Hex(int64_t value);
        static const char* PRINTINT64HEX_LINE;

};

#endif /* BYTECODEHELPER_INCL */
