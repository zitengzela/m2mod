
namespace M2Mod.Interop.Structures
{
    public enum M2LibError : int
    {
        OK = 0,

        PathTooLong,

        FailedToLoadM2_NoFileSpecified,
        FailedToLoadM2_CouldNotOpenFile,
        FailedToLoadM2_FileCorrupt,
        FailedToLoadM2_VersionNotSupported,

        FailedToExportM2I_NoFileSpecified,
        FailedToExportM2I_CouldNotOpenFile,
        FailedToExportM2I_M2NotLoaded,

        FailedToImportM2I_NoFileSpecified,
        FailedToImportM2I_CouldNotOpenFile,
        FailedToImportM2I_FileCorrupt,
        FailedToImportM2I_UnsupportedVersion,
        FailedToImportM2I_TooManyVertices,
        FailedToImportM2I_SkinHasTooManyIndices,

        FailedToSaveM2,
        FailedToSaveM2_NoFileSpecified,

        FailedToLoadSKIN_NoFileSpecified,
        FailedToLoadSKIN_CouldNotOpenFile,
        FailedToLoadSKIN_FileMissingOrCorrupt,

        FailedToSaveSKIN,
        FailedToSaveSKIN_NoFileSpecified,

        FailedToSaveM2BR_Ambiguous,

        FailedToLoadSkeleton_NoFileSpecified,
        FailedToLoadSkeleton_CouldNotOpenFile,
    }
}
