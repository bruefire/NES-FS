#include <Windows.h>

#if defined(_WIN32)
#include <dxgi.h> // for GetDefaultAdapterLuid
#pragma comment(lib, "dxgi.lib")
#endif
#include "VR_Manager.h"

#include "../OVR/Common/Win32_GLAppUtil.h"
#include "OculusTextureBuffer.h"
#include "geometry.h"
#include "functions.h"


VR_Manager::VR_Manager()
{
}
VR_Manager::~VR_Manager()
{
}


VR_Manager* vrInst;
void VR_Manager::Init(HINSTANCE hInst)
{
    vrInst = this;
	this->hInst = hInst;

	// Initializes LibOVR, and the Rift
	ovrInitParams initParams = { ovrInit_RequestVersion | ovrInit_FocusAware, OVR_MINOR_VERSION, NULL, 0, 0 };
	ovrResult result = ovr_Initialize(&initParams);
	VALIDATE(OVR_SUCCESS(result), "Failed to initialize libOVR.");

	// use the window for VR.
	VALIDATE(Platform.InitWindow(this->hInst, L"Oculus Room Tiny (GL)"), "Failed to open window.");
}


bool NesEsVRmainLoop(bool val)
{
    return vrInst->MainLoop(val);
}

void VR_Manager::Start()
{
	Platform.Run(NesEsVRmainLoop);
}


void VR_Manager::Dispose()
{
	ovr_Shutdown();
}


bool VR_Manager::MainLoop(bool retryCreate)
{
    OculusTextureBuffer* eyeRenderTexture[2] = { nullptr, nullptr };
    ovrMirrorTexture mirrorTexture = nullptr;
    GLuint          mirrorFBO = 0;
    //Scene* roomScene = nullptr;
    long long frameIndex = 0;

    ovrSession session;
    ovrGraphicsLuid luid;
    ovrResult result = ovr_Create(&session, &luid);
    if (!OVR_SUCCESS(result))
        return retryCreate;

    if (Compare(luid, GetDefaultAdapterLuid())) // If luid that the Rift is on is not the default adapter LUID...
    {
        VALIDATE(false, "OpenGL supports only the default graphics adapter.");
    }

    ovrHmdDesc hmdDesc = ovr_GetHmdDesc(session);

    // Setup Window and Graphics
    // Note: the mirror window can be any size, for this sample we use 1/2 the HMD resolution
    ovrSizei windowSize = { hmdDesc.Resolution.w / 2, hmdDesc.Resolution.h / 2 };
    if (!Platform.InitDevice(windowSize.w, windowSize.h, reinterpret_cast<LUID*>(&luid)))
        goto Done;

    // Make eye render buffers
    for (int eye = 0; eye < 2; ++eye)
    {
        ovrSizei idealTextureSize = ovr_GetFovTextureSize(session, ovrEyeType(eye), hmdDesc.DefaultEyeFov[eye], 1);
        eyeRenderTexture[eye] = new OculusTextureBuffer(session, idealTextureSize, 1);

        if (!eyeRenderTexture[eye]->ColorTextureChain || !eyeRenderTexture[eye]->DepthTextureChain)
        {
            if (retryCreate) goto Done;
            VALIDATE(false, "Failed to create texture.");
        }
    }

    ovrMirrorTextureDesc desc;
    memset(&desc, 0, sizeof(desc));
    desc.Width = windowSize.w;
    desc.Height = windowSize.h;
    desc.Format = OVR_FORMAT_R8G8B8A8_UNORM_SRGB;

    // Create mirror texture and an FBO used to copy mirror texture to back buffer
    result = ovr_CreateMirrorTextureWithOptionsGL(session, &desc, &mirrorTexture);
    if (!OVR_SUCCESS(result))
    {
        if (retryCreate) goto Done;
        VALIDATE(false, "Failed to create mirror texture.");
    }

    // Configure the mirror read buffer
    GLuint texId;
    ovr_GetMirrorTextureBufferGL(session, mirrorTexture, &texId);

    glGenFramebuffers(1, &mirrorFBO);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
    glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, texId, 0);
    glFramebufferRenderbuffer(GL_READ_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

    // Turn off vsync to let the compositor do its magic
    wglSwapIntervalEXT(0);

    // Make scene - can simplify further if needed
    //roomScene = new Scene(false);
    initGlScnene(
        eyeRenderTexture[0]->texSize.w,
        eyeRenderTexture[0]->texSize.h, 
        (hmdDesc.DefaultEyeFov[0].LeftTan + hmdDesc.DefaultEyeFov[0].RightTan) * 0.5);

    // FloorLevel will give tracking poses where the floor height is 0
    ovr_SetTrackingOriginType(session, ovrTrackingOrigin_FloorLevel);

    // Main loop
    pt3 preVrLoc = pt3(0, 0, 0);
    pt3 preVrStd[2] = { pt3(0, 0, 0), pt3(0, 0, 0) };
    pt3 vrLoc;
    pt3 vrStd[2];
    while (Platform.HandleMessages())
    {
        ovrSessionStatus sessionStatus;
        ovr_GetSessionStatus(session, &sessionStatus);
        if (sessionStatus.ShouldQuit)
        {
            // Because the application is requested to quit, should not request retry
            retryCreate = false;
            break;
        }
        if (sessionStatus.ShouldRecenter)
            ovr_RecenterTrackingOrigin(session);

        if (sessionStatus.IsVisible)
        {
            // Keyboard inputs to adjust player orientation
            static float Yaw(3.141592f);
            if (Platform.Key[VK_LEFT])  Yaw += 0.02f;
            if (Platform.Key[VK_RIGHT]) Yaw -= 0.02f;

            // Keyboard inputs to adjust player position
            static Vector3f Pos2(0.0f, 0.0f, -5.0f);
            if (Platform.Key['W'] || Platform.Key[VK_UP])     Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, -0.05f));
            if (Platform.Key['S'] || Platform.Key[VK_DOWN])   Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(0, 0, +0.05f));
            if (Platform.Key['D'])                            Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(+0.05f, 0, 0));
            if (Platform.Key['A'])                            Pos2 += Matrix4f::RotationY(Yaw).Transform(Vector3f(-0.05f, 0, 0));

            // Animate the cube
            static float cubeClock = 0;
            //if (sessionStatus.HasInputFocus) // Pause the application if we are not supposed to have input.
            //    roomScene->Models[0]->Pos = Vector3f(9 * (float)sin(cubeClock), 3, 9 * (float)cos(cubeClock += 0.015f));

            // Call ovr_GetRenderDesc each frame to get the ovrEyeRenderDesc, as the returned values (e.g. HmdToEyePose) may change at runtime.
            ovrEyeRenderDesc eyeRenderDesc[2];
            eyeRenderDesc[0] = ovr_GetRenderDesc(session, ovrEye_Left, hmdDesc.DefaultEyeFov[0]);
            eyeRenderDesc[1] = ovr_GetRenderDesc(session, ovrEye_Right, hmdDesc.DefaultEyeFov[1]);
            double eyeDstHf = pt3(
                eyeRenderDesc[1].HmdToEyePose.Position.x - eyeRenderDesc[0].HmdToEyePose.Position.x,
                eyeRenderDesc[1].HmdToEyePose.Position.y - eyeRenderDesc[0].HmdToEyePose.Position.y,
                eyeRenderDesc[1].HmdToEyePose.Position.z - eyeRenderDesc[0].HmdToEyePose.Position.z)
                .length() * 0.5;

            // Get eye poses, feeding in correct IPD offset
            ovrPosef EyeRenderPose[2];
            ovrPosef HmdToEyePose[2] = { eyeRenderDesc[0].HmdToEyePose,
                                         eyeRenderDesc[1].HmdToEyePose };

            double sensorSampleTime;    // sensorSampleTime is fed into the layer later
            ovr_GetEyePoses(session, frameIndex, ovrTrue, HmdToEyePose, EyeRenderPose, &sensorSampleTime);

            ovrTimewarpProjectionDesc posTimewarpProjectionDesc = {};

            // Render world
            int repnEye = 0;
            ovrVector3f eyeLoc = EyeRenderPose[repnEye].Position;
            vrLoc = pt3(eyeLoc.x, eyeLoc.y, -eyeLoc.z);

            ovrQuatf eyeRot = EyeRenderPose[repnEye].Orientation;
            pt4 eyeQ = pt4(eyeRot.w, eyeRot.x, eyeRot.y, eyeRot.z);
            vrStd[0] = pt3(0, 0, 1);
            vrStd[1] = pt3(0, 1, 0);
            for (int s = 0; s < 2; s++)
                vrStd[s] = eyeQ.qtrMtp(vrStd[s]);
            
            pt3 vrLocDf;
            pt3 vrStdDf[2];
            if (!preVrStd[0].isZero())
            {
                vrLocDf = vrLoc.mns(preVrLoc);
                //test
                pt3 tvrStd[2] = { vrStd[0], vrStd[1] };
                tvrStd[0].x *= -1;
                tvrStd[0].y *= -1;
                tvrStd[1].x *= -1;
                tvrStd[1].y *= -1;
                tvrStd[1] = tvrStd[1].mtp(-1);
                pt3 tvrCross = pt3::cross(tvrStd[1], tvrStd[0]);
                vrLocDf = pt3(
                    pt3::dot(tvrCross, vrLocDf),
                    pt3::dot(tvrStd[1], vrLocDf),
                    pt3::dot(tvrStd[0], vrLocDf));
                //test
                pt3 preCross = pt3::cross(preVrStd[1], preVrStd[0]);

                for (int i = 0; i < 2; i++)
                {
                    vrStdDf[i] = pt3(
                        pt3::dot(preCross, vrStd[i]),
                        pt3::dot(preVrStd[1], vrStd[i]),
                        pt3::dot(preVrStd[0], vrStd[i]));
                }
            }
            else
            {
                vrLocDf = pt3();
                vrStdDf[0] = pt3();
                vrStdDf[1] = pt3();
            }
            // test
            vrLocDf = vrLocDf.mtp(55);
            eyeDstHf *= 1;
            // test

            // do main logical proccessing.
            SendPose(vrLocDf, vrStdDf, 0);
            updateSceneLgc();

            // Render Scene to Eye Buffers
            for (int eye = 0; eye < 2; ++eye)
            {
                // Switch to eye render target
                eyeRenderTexture[eye]->SetAndClearRenderSurface();

                // Get view and projection matrices
                Matrix4f rollPitchYaw = Matrix4f::RotationY(Yaw);
                Matrix4f finalRollPitchYaw = rollPitchYaw * Matrix4f(EyeRenderPose[eye].Orientation);
                Vector3f finalUp = finalRollPitchYaw.Transform(Vector3f(0, 1, 0));
                Vector3f finalForward = finalRollPitchYaw.Transform(Vector3f(0, 0, -1));
                Vector3f shiftedEyePos = Pos2 + rollPitchYaw.Transform(EyeRenderPose[eye].Position);

                Matrix4f view = Matrix4f::LookAtRH(shiftedEyePos, shiftedEyePos + finalForward, finalUp);
                Matrix4f proj = ovrMatrix4f_Projection(hmdDesc.DefaultEyeFov[eye], 0.2f, 1000.0f, ovrProjection_None);
                posTimewarpProjectionDesc = ovrTimewarpProjectionDesc_FromProjection(proj, ovrProjection_None);

                // Render world
                // Rendering Scene
                SendPose(vrLocDf, vrStdDf, eyeDstHf * powi(-1, eye + 1));
                updateGlScene();

                //roomScene->Render(view, proj);

                // Avoids an error when calling SetAndClearRenderSurface during next iteration.
                // Without this, during the next while loop iteration SetAndClearRenderSurface
                // would bind a framebuffer with an invalid COLOR_ATTACHMENT0 because the texture ID
                // associated with COLOR_ATTACHMENT0 had been unlocked by calling wglDXUnlockObjectsNV.
                eyeRenderTexture[eye]->UnsetRenderSurface();

                // Commit changes to the textures so they get picked up frame
                eyeRenderTexture[eye]->Commit();
            }

            // Do distortion rendering, Present and flush/sync

            ovrLayerEyeFovDepth ld = {};
            ld.Header.Type = ovrLayerType_EyeFovDepth;
            ld.Header.Flags = ovrLayerFlag_TextureOriginAtBottomLeft;   // Because OpenGL.
            ld.ProjectionDesc = posTimewarpProjectionDesc;
            ld.SensorSampleTime = sensorSampleTime;

            for (int eye = 0; eye < 2; ++eye)
            {
                ld.ColorTexture[eye] = eyeRenderTexture[eye]->ColorTextureChain;
                ld.DepthTexture[eye] = eyeRenderTexture[eye]->DepthTextureChain;
                ld.Viewport[eye] = Recti(eyeRenderTexture[eye]->GetSize());
                ld.Fov[eye] = hmdDesc.DefaultEyeFov[eye];
                ld.RenderPose[eye] = EyeRenderPose[eye];
            }

            ovrLayerHeader* layers = &ld.Header;
            result = ovr_SubmitFrame(session, frameIndex, nullptr, &layers, 1);
            // exit the rendering loop if submit returns an error, will retry on ovrError_DisplayLost
            if (!OVR_SUCCESS(result))
                goto Done;

            frameIndex++;
        }

        // Blit mirror texture to back buffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, mirrorFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        GLint w = windowSize.w;
        GLint h = windowSize.h;
        glBlitFramebuffer(0, h, w, 0,
            0, 0, w, h,
            GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

        SwapBuffers(Platform.hDC);

        preVrLoc = vrLoc;
        preVrStd[0] = vrStd[0];
        preVrStd[1] = vrStd[1];
    }

Done:
    //delete roomScene;
    disposeGlScene();
    if (mirrorFBO) glDeleteFramebuffers(1, &mirrorFBO);
    if (mirrorTexture) ovr_DestroyMirrorTexture(session, mirrorTexture);
    for (int eye = 0; eye < 2; ++eye)
    {
        delete eyeRenderTexture[eye];
    }
    Platform.ReleaseDevice();
    ovr_Destroy(session);

    // Retry on ovrError_DisplayLost
    return retryCreate || (result == ovrError_DisplayLost);
}


int VR_Manager::Compare(const ovrGraphicsLuid& lhs, const ovrGraphicsLuid& rhs)
{
    return memcmp(&lhs, &rhs, sizeof(ovrGraphicsLuid));
}


ovrGraphicsLuid VR_Manager::GetDefaultAdapterLuid()
{
    ovrGraphicsLuid luid = ovrGraphicsLuid();

#if defined(_WIN32)
    IDXGIFactory* factory = nullptr;

    if (SUCCEEDED(CreateDXGIFactory(IID_PPV_ARGS(&factory))))
    {
        IDXGIAdapter* adapter = nullptr;

        if (SUCCEEDED(factory->EnumAdapters(0, &adapter)))
        {
            DXGI_ADAPTER_DESC desc;

            adapter->GetDesc(&desc);
            memcpy(&luid, &desc.AdapterLuid, sizeof(luid));
            adapter->Release();
        }

        factory->Release();
    }
#endif

    return luid;
}
