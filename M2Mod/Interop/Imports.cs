using System;
using System.Runtime.InteropServices;
using M2Mod.Controls;
using M2Mod.Interop.Structures;

namespace M2Mod.Interop
{
    static class Imports
    {
        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl, CharSet = CharSet.Unicode)]
        public static extern IntPtr M2_Create([In] ref Settings settings);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_Load(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_Save(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)]string filePath, SaveMask saveMask);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_ExportM2Intermediate(IntPtr handle,
            [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_ImportM2Intermediate(IntPtr handle,
            [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_SetNeedRemapReferences(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)]string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_SetNeedRemoveTXIDChunk(IntPtr handle);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_AddNormalizationRule(IntPtr handle,
            NormalizeRuleType sourceType, uint[] sourceData, int sourceDataLen,
            NormalizeRuleType targetType, uint[] targetData, int targetDataLen, bool preferSource);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_SetReplaceM2(IntPtr handle,
            [MarshalAs(UnmanagedType.LPWStr)] string filePath);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void M2_Free(IntPtr handle);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(ConstWCharPtrMarshaller))]
        public static extern string GetErrorText(M2LibError errNo);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr FileStorage_Get([MarshalAs(UnmanagedType.LPWStr)] string mappingsDirectory);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FileStorage_Clear();

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void FileStorage_SetMappingsDirectory(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)] string mappingsDirectory);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr FileStorage_GetFileInfoByFileDataId(IntPtr handle, uint fileDataId);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr FileStorage_GetFileInfoByPartialPath(IntPtr handle, [MarshalAs(UnmanagedType.LPWStr)]string path);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint FileInfo_GetFileDataId(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(ConstWCharPtrMarshaller))]
        public static extern string FileInfo_GetPath(IntPtr pointer);

        public delegate void LoggerDelegate(LogLevel logLevel, [MarshalAs(UnmanagedType.LPWStr)]string message);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void AttachLoggerCallback(LogLevel logLevel, LoggerDelegate callback);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void DetachLoggerCallback(LogLevel logLevel, LoggerDelegate callback);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern IntPtr Wrapper_Create(IntPtr oldM2, IntPtr newM2, float weightThreshold, bool compareTextures, bool precictScale, ref float sourceScale);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern CompareStatus Wrapper_GetResult(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        [return: MarshalAs(UnmanagedType.CustomMarshaler, MarshalTypeRef = typeof(ConstWCharPtrMarshaller))]
        public static extern string Wrapper_GetStringResult(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern uint Wrapper_DiffSize(IntPtr pointer);

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern void Wrapper_Free(IntPtr pointer);

        [return: MarshalAs(UnmanagedType.LPWStr)]
        public delegate string SaveMappingsDelegate();

        [DllImport("M2Lib.dll", CallingConvention = CallingConvention.Cdecl)]
        public static extern M2LibError M2_SetSaveMappingsCallback(IntPtr handle, SaveMappingsDelegate callback);
    }
}
