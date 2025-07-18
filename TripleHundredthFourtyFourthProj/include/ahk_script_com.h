﻿#pragma once


extern bool g_ComErrorNotify;


class ComObject;
class ComEvent : public ObjectBase
{
	DWORD mCookie;
	ComObject *mObject;
	ITypeInfo *mTypeInfo;
	IID mIID;
	IObject *mAhkObject;
	TCHAR mPrefix[64];

public:
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP GetIDsOfNames(REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId);
	STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr);
	
	// IObject::Invoke() and Type() are unlikely to be called, since that would mean
	// the script has a reference to the object, which means either that the script
	// itself has implemented IConnectionPoint (and why would it?), or has used the
	// IEnumConnections interface to retrieve its own object (unlikely).
	ResultType STDMETHODCALLTYPE Invoke(ExprTokenType &aResultToken, ExprTokenType &aThisToken, int aFlags, ExprTokenType *aParam[], int aParamCount)
	{
		return INVOKE_NOT_HANDLED;
	}
	IObject_Type_Impl("ComEvent") // Unlikely to be called; see above.

	HRESULT Connect(ITypeInfo *tinfo = nullptr, IID *iid = nullptr);
	void SetPrefixOrSink(LPCTSTR pfx, IObject *ahkObject);

	ComEvent(ComObject *obj) : mObject(obj), mCookie(0), mTypeInfo(NULL), mAhkObject(NULL) { }
	~ComEvent();

	friend class ComObject;
};


class ComObject : public ObjectBase
{
public:
	union
	{
		IDispatch *mDispatch;
		IUnknown *mUnknown;
		SAFEARRAY *mArray;
		void *mValPtr;
		__int64 mVal64; // Allow 64-bit values when ComObject is used as a VARIANT in 32-bit builds.
	};
	ComEvent *mEventSink;
	VARTYPE mVarType;
	enum { F_OWNVALUE = 1 };
	USHORT mFlags;

	ResultType STDMETHODCALLTYPE Invoke(ExprTokenType &aResultToken, ExprTokenType &aThisToken, int aFlags, ExprTokenType *aParam[], int aParamCount);
	ResultType SafeArrayInvoke(ExprTokenType &aResultToken, int aFlags, ExprTokenType *aParam[], int aParamCount);
	LPTSTR Type();

	void ToVariant(VARIANT &aVar)
	{
		aVar.vt = mVarType;
		aVar.llVal = mVal64;
		// Caller handles this if needed:
		//if (VT_DISPATCH == mVarType && mDispatch)
		//	mDispatch->AddRef();
	}

	ComObject(IDispatch *pdisp)
		: mVal64((__int64)pdisp), mVarType(VT_DISPATCH), mEventSink(NULL), mFlags(0) { }
	ComObject(__int64 llVal, VARTYPE vt, USHORT flags = 0)
		: mVal64(llVal), mVarType(vt), mEventSink(NULL), mFlags(flags) { }
	~ComObject()
	{
		if ((VT_DISPATCH == mVarType || VT_UNKNOWN == mVarType) && mUnknown)
		{
			if (mEventSink)
			{
				mEventSink->Connect(FALSE);
				if (mEventSink) // i.e. it wasn't fully released as a result of calling Unadvise().
					mEventSink->mObject = nullptr;
			}
			mUnknown->Release();
		}
		else if ((mVarType & (VT_BYREF|VT_ARRAY)) == VT_ARRAY && (mFlags & F_OWNVALUE))
		{
			SafeArrayDestroy(mArray);
		}
	}

#ifdef CONFIG_DEBUGGER
	void DebugWriteProperty(IDebugProperties *, int aPage, int aPageSize, int aDepth);
#endif
};


class ComEnum : public EnumBase
{
	IEnumVARIANT *penum;
	bool cheat;

public:
	int Next(Var *aOutput, Var *aOutputType);

	ComEnum(IEnumVARIANT *enm);
	~ComEnum()
	{
		penum->Release();
	}
	IObject_Type_Impl("ComObject.Enumerator")
};


class ComArrayEnum : public EnumBase
{
	ComObject *mArrayObject;
	char *mPointer, *mEnd;
	UINT mElemSize;
	VARTYPE mType;

	ComArrayEnum(ComObject *aObj, char *aData, char *aDataEnd, UINT aElemSize, VARTYPE aType)
		: mArrayObject(aObj), mPointer(aData - aElemSize), mEnd(aDataEnd), mElemSize(aElemSize), mType(aType)
	{
	}

public:
	static HRESULT Begin(ComObject *aArrayObject, ComArrayEnum *&aOutput);
	int Next(Var *aOutput, Var *aOutputType);
	~ComArrayEnum();
	IObject_Type_Impl("ComObjArray.Enumerator")
};


// Adapts an AutoHotkey enumerator object to the IEnumVARIANT COM interface.
class EnumComCompat : public IEnumVARIANT, public IServiceProvider
{
	IObject *mEnum;
	int mRefCount;
	bool mCheat;

public:
	EnumComCompat(IObject *enumObj) : mEnum(enumObj), mRefCount(1), mCheat(false) {}
	~EnumComCompat() { mEnum->Release(); }

	STDMETHODIMP QueryInterface(REFIID riid, void **ppvObject);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	STDMETHODIMP Next(ULONG celt, /*out*/ VARIANT *rgVar, /*out*/ ULONG *pCeltFetched);
	STDMETHODIMP Skip(ULONG celt);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(/*out*/ IEnumVARIANT **ppEnum);

	STDMETHODIMP QueryService(REFGUID guidService, REFIID riid, void **ppvObject);
};


void ComError(HRESULT, LPTSTR = _T(""), EXCEPINFO* = NULL);

bool SafeSetTokenObject(ExprTokenType &aToken, IObject *aObject);

