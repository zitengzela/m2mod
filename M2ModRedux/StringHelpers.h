#pragma once

namespace M2ModRedux
{
	using namespace System;
	using namespace System::Runtime::InteropServices;

	ref class StringConverter
	{
		private: String^ Value;

		private: IntPtr pStringA = IntPtr::Zero;
		private: IntPtr pStringW = IntPtr::Zero;

		public:StringConverter(String^ Value)
		{
			this->Value = Value;
		}

		public: char const* ToStringA()
		{
			if (pStringA == IntPtr::Zero)
				pStringA = Marshal::StringToHGlobalAnsi(Value);

			return (char const*)pStringA.ToPointer();
		}

		public: wchar_t const* ToStringW()
		{
			if (pStringW == IntPtr::Zero)
				pStringW = Marshal::StringToHGlobalUni(Value);

			return (wchar_t const*)pStringW.ToPointer();
		}

		public: ~StringConverter()
		{
			if (pStringA != IntPtr::Zero)
				Marshal::FreeHGlobal(pStringA);
			if (pStringW != IntPtr::Zero)
				Marshal::FreeHGlobal(pStringW);
		}
	};
}

