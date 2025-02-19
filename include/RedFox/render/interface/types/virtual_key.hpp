#pragma once

namespace RF
{
	enum class virtual_key_t
	{
		eUnknown = 0,
		eA, eB, eC, eD, eE, eF, eG, eH, eI, eJ, eK, eL, eM,
		eN, eO, eP, eQ, eR, eS, eT, eU, eV, eW, eX, eY, eZ,

		eNumKey0, eNumKey1, eNumKey2, eNumKey3, eNumKey4, eNumKey5, eNumKey6, eNumKey7, eNumKey8, eNumKey9,
		eNumPad0, eNumPad1, eNumPad2, eNumPad3, eNumPad4, eNumPad5, eNumPad6, eNumPad7, eNumPad8, eNumPad9,
		eNumAny0, eNumAny1, eNumAny2, eNumAny3, eNumAny4, eNumAny5, eNumAny6, eNumAny7, eNumAny8, eNumAny9, // obsolete
		
		eEqual, ePlus,
		eKeyMinus, ePadMinus,

		eEscape,
		eKeyReturn, ePadReturn,
		eKeyBackspace, ePadBackspace,
		eTab,
		eSpace,

		eLShift, eRShift,
		eLControl, eRControl,
		eLSuper, eRSuper,
		eAlt, eAltGr,

		eFunction1, eFunction2, eFunction3, eFunction4, eFunction5, eFunction6, eFunction7, eFunction8, eFunction9, eFunction10, eFunction11, eFunction12,

		eHome, eEnd,
		eInsert, eDelete,
		ePageUp, ePageDown,

		eArrowUp, eArrowDown, eArrowLeft, eArrowRight,

		eCapsLock, ePrintScreen, eScrollLock, ePauseBreak
	};
} // namespace RF