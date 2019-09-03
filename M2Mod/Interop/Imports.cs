using System;
using System.Runtime.InteropServices;
using M2Mod.Interop.Structures;

namespace M2Mod.Interop
{
    static class Imports
    {
        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr M2_Create([In] ref Settings settings);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_Load(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_Save(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)]string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_ExportM2Intermediate(IntPtr handle,
            [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_ImportM2Intermediate(IntPtr handle,
            [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_SetReplaceM2(IntPtr handle,
            [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void M2_Free(IntPtr handle);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(ConstCharPtrMarshaller))]
        public static extern string GetErrorText(M2LibError errNo);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void LoadFileStorage([MarshalAs(UnmanagedType.LPStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetFileInfoByFileDataId(uint fileDataId);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr GetFileInfoByPartialPath([MarshalAs(UnmanagedType.LPStr)]string path);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint FileInfo_GetFileDataId(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(ConstCharPtrMarshaller))]
        public static extern string FileInfo_GetPath(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern bool FileInfo_IsCustom(IntPtr pointer);

        public delegate void LoggerDelegate(int logLevel, string message);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AttachLoggerCallback(LoggerDelegate callback);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Wrapper_Create([MarshalAs(UnmanagedType.LPWStr)]string oldM2Path, [MarshalAs(UnmanagedType.LPWStr)]string newM2Path, float weightThreshold, bool compareTextures, ref Settings settings);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError Wrapper_GetErrorStatus(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]

        public static extern CompareStatus Wrapper_GetResult(IntPtr pointer);
        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(ConstCharPtrMarshaller))]
        public static extern string Wrapper_GetStringResult(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint Wrapper_DiffSize(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Wrapper_Free(IntPtr pointer);
    }
}
