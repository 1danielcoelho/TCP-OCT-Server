#ifdef VOLUMERENDERERLIBRARY_EXPORTS
#define VOLUMERENDERERLIBRARY_API __declspec(dllexport)
#else
#define VOLUMERENDERERLIBRARY_API __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif
    #include <SpectralRadar.h>

    struct C_VolumeRenderer;
    typedef C_VolumeRenderer* VolumeRendererHandle;

    struct C_RendererDataSource;
    typedef C_RendererDataSource* RendererDataSourceHandle;

    VOLUMERENDERERLIBRARY_API VolumeRendererHandle createOpenGLRenderer(void);
    VOLUMERENDERERLIBRARY_API void clearVolumeRenderer(VolumeRendererHandle Render);

    VOLUMERENDERERLIBRARY_API RendererDataSourceHandle createOnlineDataSource(OCTDeviceHandle Dev, ProcessingHandle Proc, Coloring32BitHandle Coloring);
    VOLUMERENDERERLIBRARY_API void clearDataSource(RendererDataSourceHandle DataSource);

    VOLUMERENDERERLIBRARY_API void startDataSource(VolumeRendererHandle Renderer, RendererDataSourceHandle DataSource, ScanPatternHandle Pattern);
    VOLUMERENDERERLIBRARY_API void stopDataSource(RendererDataSourceHandle DataSource);
    VOLUMERENDERERLIBRARY_API void renderVolume(VolumeRendererHandle Renderer);
    VOLUMERENDERERLIBRARY_API double getRenderedVolumesPerSec(VolumeRendererHandle Renderer);

    VOLUMERENDERERLIBRARY_API void setAngles(VolumeRendererHandle Renderer, double AngleX, double AngleY);
    VOLUMERENDERERLIBRARY_API void setZoom(VolumeRendererHandle Renderer, double Zoom);
    VOLUMERENDERERLIBRARY_API void showRenderingBox(VolumeRendererHandle Renderer, BOOL OnOff);

    VOLUMERENDERERLIBRARY_API void setViewportSize(int SizeX, int SizeY);
    VOLUMERENDERERLIBRARY_API void setRenderingSlices(VolumeRendererHandle Renderer, int Slices);
#ifdef __cplusplus
}
#endif
