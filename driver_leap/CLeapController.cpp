#include "stdafx.h"

#include "CLeapController.h"
#include "CControllerButton.h"

#include "CDriverConfig.h"
#include "CGestureMatcher.h"
#include "Utils.h"

const glm::quat g_reverseRotation(0.f, 0.f, 0.70106769f, -0.70106769f);
const glm::quat g_rotateHalfPiZ(0.70106769f, 0.f, 0.f, 0.70106769f);
const glm::quat g_rotateHalfPiZN(0.70106769f, 0.f, 0.f, -0.70106769f);

vr::IVRServerDriverHost *CLeapController::ms_driverHost = nullptr;
vr::IVRDriverInput *CLeapController::ms_driverInput = nullptr;
double CLeapController::ms_headPosition[] = { .0, .0, .0 };
vr::HmdQuaternion_t CLeapController::ms_headRotation = { 1.0, .0, .0, .0 };
vr::CVRPropertyHelpers *CLeapController::ms_propertyHelpers = nullptr;

CLeapController::CLeapController()
{
    m_haptic = vr::k_ulInvalidPropertyContainer;
    m_propertyContainer = vr::k_ulInvalidPropertyContainer;
    m_trackedDevice = vr::k_unTrackedDeviceIndexInvalid;

    m_pose = { 0 };
    m_pose.deviceIsConnected = false;
    for(size_t i = 0U; i < 3U; i++)
    {
        m_pose.vecAcceleration[i] = .0;
        m_pose.vecAngularAcceleration[i] = .0;
        m_pose.vecAngularVelocity[i] = .0;
        m_pose.vecDriverFromHeadTranslation[i] = .0;
    }
    m_pose.poseTimeOffset = -0.016;
    m_pose.qDriverFromHeadRotation = { 1.0, .0, .0, .0 };
    m_pose.qRotation = { 1.0, .0, .0, .0 };
    m_pose.qWorldFromDriverRotation = { 1.0, .0, .0, .0 };
    m_pose.result = vr::TrackingResult_Uninitialized;
    m_pose.shouldApplyHeadModel = false;
    m_pose.willDriftInYaw = false;

    m_hand = CH_Left;
    m_gameProfile = GP_Default;
    m_specialMode = false;
}
CLeapController::~CLeapController()
{
    for(auto l_button : m_buttons) delete l_button;
}

// vr::ITrackedDeviceServerDriver
vr::EVRInitError CLeapController::Activate(uint32_t unObjectId)
{
    vr::EVRInitError l_resultError = vr::VRInitError_Driver_Failed;

    if(m_trackedDevice == vr::k_unTrackedDeviceIndexInvalid)
    {
        m_trackedDevice = unObjectId;
        m_propertyContainer = ms_propertyHelpers->TrackedDeviceToPropertyContainer(m_trackedDevice);

        ActivateInternal();

        l_resultError = vr::VRInitError_None;
    }

    return l_resultError;
}
void CLeapController::Deactivate()
{
    ResetControls();
    m_trackedDevice = vr::k_unTrackedDeviceIndexInvalid;
}

void* CLeapController::GetComponent(const char* pchComponentNameAndVersion)
{
    void *l_result = nullptr;
    if(!strcmp(pchComponentNameAndVersion, vr::ITrackedDeviceServerDriver_Version)) l_result = this;
    return l_result;
}

vr::DriverPose_t CLeapController::GetPose()
{
    return m_pose;
}

// CLeapController
bool CLeapController::GetEnabled() const
{
    return m_pose.deviceIsConnected;
}

void CLeapController::SetEnabled(bool f_state)
{
    m_pose.deviceIsConnected = f_state;
    if(m_trackedDevice != vr::k_unTrackedDeviceIndexInvalid) ms_driverHost->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));
}

void CLeapController::ResetControls()
{
    for(auto l_button : m_buttons)
    {
        l_button->SetValue(0.f);
        l_button->SetState(false);
    }
}

void CLeapController::SetGameProfile(GameProfile f_profile)
{
    if(m_gameProfile != f_profile)
    {
        m_gameProfile = f_profile;
        ResetControls();
    }
}

void CLeapController::SwitchSpecialMode()
{
    m_specialMode = !m_specialMode;
}

void CLeapController::Update(const Leap::Frame &f_frame)
{
    if(m_trackedDevice != vr::k_unTrackedDeviceIndexInvalid)
    {
        if(m_pose.deviceIsConnected)
        {
            UpdateTransformation(f_frame);
            ms_driverHost->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));

			UpdateConfig();

            UpdateGestures(f_frame);
            UpdateInput();
        }
        else ms_driverHost->TrackedDevicePoseUpdated(m_trackedDevice, m_pose, sizeof(vr::DriverPose_t));
    }
}

void CLeapController::UpdateConfig()
{
	/*
		LeftHand Offset				x = SCL + U
									y = SCL + I
									z = SCL + O
		LeftHand Offset Rot			x = SCL + NML + U
									y = SCL + NML + I
									z = SCL + NML + O
									w = SCL + NML + P

		RightHand(+Caps) Offset		x = SCL + Caps + U
									y = SCL + Caps + I
									z = SCL + Caps + O
		RightHand(+Caps) Offset Rot	x = SCL + Caps + NML + U
									y = SCL + Caps + NML + I
									z = SCL + Caps + NML + O
									w = SCL + Caps + NML + P
		U - 0x55
		I - 0x49
		O - 0x4F
		P - 0x50
	*/
	if ((GetKeyState(VK_SCROLL) & 0xFFFF) != 0)
	{
		if ((GetKeyState(VK_CAPITAL) & 0xFFFF) != 0)
		{
			if ((GetKeyState(VK_NUMLOCK) & 0xFFFF) != 0)
			{
				if (GetAsyncKeyState(VK_UKEY) & 0x8000)		 { CDriverConfig::SetRightHandOffsetRotationEle(0, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffsetRotation().x - 0.01f) : (CDriverConfig::GetRightHandOffsetRotation().x + 0.01f)); return; }
				else if (GetAsyncKeyState(VK_IKEY) & 0x8000) { CDriverConfig::SetRightHandOffsetRotationEle(1, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffsetRotation().y - 0.01f) : (CDriverConfig::GetRightHandOffsetRotation().y + 0.01f)); return; }
				else if (GetAsyncKeyState(VK_OKEY) & 0x8000) { CDriverConfig::SetRightHandOffsetRotationEle(2, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffsetRotation().z - 0.01f) : (CDriverConfig::GetRightHandOffsetRotation().z + 0.01f)); return; }
				else if (GetAsyncKeyState(VK_PKEY) & 0x8000) { CDriverConfig::SetRightHandOffsetRotationEle(3, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffsetRotation().w - 0.01f) : (CDriverConfig::GetRightHandOffsetRotation().w + 0.01f)); return; }

				if(GetAsyncKeyState(VK_RKEY) & 0x8000)
				{
					CDriverConfig::SetRightHandOffsetRotationEle(0, 0.0);
					CDriverConfig::SetRightHandOffsetRotationEle(1, 0.0);
					CDriverConfig::SetRightHandOffsetRotationEle(2, 0.0);
					CDriverConfig::SetRightHandOffsetRotationEle(3, 1.0);

					CDriverConfig::SetRightHandOffsetEle(0, 0.0);
					CDriverConfig::SetRightHandOffsetEle(1, 0.0);
					CDriverConfig::SetRightHandOffsetEle(2, -0.15);

					CDriverConfig::SetLeftHandOffsetRotationEle(0, 0.0);
					CDriverConfig::SetLeftHandOffsetRotationEle(1, 0.0);
					CDriverConfig::SetLeftHandOffsetRotationEle(2, 0.0);
					CDriverConfig::SetLeftHandOffsetRotationEle(3, 1.0);

					CDriverConfig::SetLeftHandOffsetEle(0, 0.0);
					CDriverConfig::SetLeftHandOffsetEle(1, 0.0);
					CDriverConfig::SetLeftHandOffsetEle(2, -0.15);
					return;
				}

				if ((GetAsyncKeyState(VK_GKEY) & 0x8000) && (GetAsyncKeyState(VK_SHIFT) & 0x8000))
				{
					CDriverConfig::SaveOffsetsData();
					return;
				}
			}
			if (GetAsyncKeyState(VK_UKEY) & 0x8000)		 { CDriverConfig::SetRightHandOffsetEle(0, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffset().x - 0.001f) : (CDriverConfig::GetRightHandOffset().x + 0.001f)); return; }
			else if (GetAsyncKeyState(VK_IKEY) & 0x8000) { CDriverConfig::SetRightHandOffsetEle(1, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffset().y - 0.001f) : (CDriverConfig::GetRightHandOffset().y + 0.001f)); return; }
			else if (GetAsyncKeyState(VK_OKEY) & 0x8000) { CDriverConfig::SetRightHandOffsetEle(2, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetRightHandOffset().z - 0.001f) : (CDriverConfig::GetRightHandOffset().z + 0.001f)); return; }
		}
		if ((GetKeyState(VK_NUMLOCK) & 0xFFFF) != 0)
		{
			if (GetAsyncKeyState(VK_UKEY) & 0x8000)		 { CDriverConfig::SetLeftHandOffsetRotationEle(0, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffsetRotation().x - 0.01f) : (CDriverConfig::GetLeftHandOffsetRotation().x + 0.01f)); return; }
			else if (GetAsyncKeyState(VK_IKEY) & 0x8000) { CDriverConfig::SetLeftHandOffsetRotationEle(1, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffsetRotation().y - 0.01f) : (CDriverConfig::GetLeftHandOffsetRotation().y + 0.01f)); return; }
			else if (GetAsyncKeyState(VK_OKEY) & 0x8000) { CDriverConfig::SetLeftHandOffsetRotationEle(2, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffsetRotation().z - 0.01f) : (CDriverConfig::GetLeftHandOffsetRotation().z + 0.01f)); return; }
			else if (GetAsyncKeyState(VK_PKEY) & 0x8000) { CDriverConfig::SetLeftHandOffsetRotationEle(3, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffsetRotation().w - 0.01f) : (CDriverConfig::GetLeftHandOffsetRotation().w + 0.01f)); return;	}
		}
		if (GetAsyncKeyState(VK_UKEY) & 0x8000)		 { CDriverConfig::SetLeftHandOffsetEle(0, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffset().x - 0.001f) : (CDriverConfig::GetLeftHandOffset().x + 0.001f)); return; }
		else if (GetAsyncKeyState(VK_IKEY) & 0x8000) { CDriverConfig::SetLeftHandOffsetEle(1, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffset().y - 0.001f) : (CDriverConfig::GetLeftHandOffset().y + 0.001f)); return; }
		else if (GetAsyncKeyState(VK_OKEY) & 0x8000) { CDriverConfig::SetLeftHandOffsetEle(2, (GetAsyncKeyState(VK_SHIFT) & 0x8000) ? (CDriverConfig::GetLeftHandOffset().z - 0.001f) : (CDriverConfig::GetLeftHandOffset().z + 0.001f)); return; }
	}

	//char buffer[128];
	//sprintf(buffer, "LeftHand Rot Offset: %f, %f, %f, %f; LeftHand Off: %f, %f, %f\n\0", CDriverConfig::GetLeftHandOffsetRotation().x, CDriverConfig::GetLeftHandOffsetRotation().y, CDriverConfig::GetLeftHandOffsetRotation().z, CDriverConfig::GetLeftHandOffsetRotation().w, \
	//	CDriverConfig::GetLeftHandOffset().x, CDriverConfig::GetLeftHandOffset().y, CDriverConfig::GetLeftHandOffset().z);
	//OutputDebugStringA(buffer);
	//buffer[0] = '\0';

	//sprintf(buffer, "RightHand Rot Offset: %f, %f, %f, %f; RightHand Off: %f, %f, %f\n\0", CDriverConfig::GetRightHandOffsetRotation().x, CDriverConfig::GetRightHandOffsetRotation().y, CDriverConfig::GetRightHandOffsetRotation().z, CDriverConfig::GetRightHandOffsetRotation().w, \
	//	CDriverConfig::GetRightHandOffset().x, CDriverConfig::GetRightHandOffset().y, CDriverConfig::GetRightHandOffset().z);
	//OutputDebugStringA(buffer);
}

void CLeapController::UpdateInput()
{
    for(auto l_button : m_buttons)
    {
        if(l_button->IsUpdated())
        {
            switch(l_button->GetInputType())
            {
                case CControllerButton::IT_Boolean:
                    ms_driverInput->UpdateBooleanComponent(l_button->GetHandle(), l_button->GetState(), .0);
                    break;
                case CControllerButton::IT_Float:
                    ms_driverInput->UpdateScalarComponent(l_button->GetHandle(), l_button->GetValue(), .0);
                    break;
            }
            l_button->ResetUpdate();
        }
    }
    UpdateInputInternal();
}

void CLeapController::UpdateTransformation(const Leap::Frame &f_frame)
{
    bool l_handFound = false;
    const Leap::HandList l_hands = f_frame.hands();
    for(const auto l_hand : l_hands)
    {
        if(l_hand.isValid())
        {
            if(((m_hand == CH_Left) && l_hand.isLeft()) || ((m_hand == CH_Right) && l_hand.isRight()))
            {
                switch(CDriverConfig::GetOrientationMode())
                {
                    case CDriverConfig::OM_HMD:
                    {
                        std::memcpy(&m_pose.qWorldFromDriverRotation, &ms_headRotation, sizeof(vr::HmdQuaternion_t));
                        std::memcpy(m_pose.vecWorldFromDriverTranslation, ms_headPosition, sizeof(double) * 3U);

                        const Leap::Vector l_position = l_hand.palmPosition();
                        const glm::vec3 &l_handOffset = ((m_hand == CH_Left) ? CDriverConfig::GetLeftHandOffset() : CDriverConfig::GetRightHandOffset());
                        m_pose.vecPosition[0] = -0.001f*l_position.x + l_handOffset.x;
                        m_pose.vecPosition[1] = -0.001f*l_position.z + l_handOffset.y;
                        m_pose.vecPosition[2] = -0.001f*l_position.y + l_handOffset.z;

                        const Leap::Vector l_velocity = l_hand.palmVelocity();
                        glm::vec3 l_resultVelocity(-0.001f*l_velocity.x, -0.001f*l_velocity.z, -0.001f*l_velocity.y);
                        const glm::quat l_headRotation(ms_headRotation.w, ms_headRotation.x, ms_headRotation.y, ms_headRotation.z);
                        l_resultVelocity = l_headRotation*l_resultVelocity;
                        m_pose.vecVelocity[0] = l_resultVelocity.x;
                        m_pose.vecVelocity[1] = l_resultVelocity.y;
                        m_pose.vecVelocity[2] = l_resultVelocity.z;

                        const Leap::Quaternion l_handOrientation = l_hand.orientation();
                        glm::quat l_rotation(l_handOrientation.w, l_handOrientation.x, l_handOrientation.y, l_handOrientation.z);
                        l_rotation = g_reverseRotation*l_rotation;
                        l_rotation *= ((m_hand == CH_Left) ? g_rotateHalfPiZN : g_rotateHalfPiZ);
                        l_rotation *= ((m_hand == CH_Left) ? CDriverConfig::GetLeftHandOffsetRotation() : CDriverConfig::GetRightHandOffsetRotation());

                        m_pose.qRotation.x = l_rotation.x;
                        m_pose.qRotation.y = l_rotation.y;
                        m_pose.qRotation.z = l_rotation.z;
                        m_pose.qRotation.w = l_rotation.w;
                    } break;
                    case CDriverConfig::OM_Desktop:
                    {
                        // Controller follows HMD position only
                        std::memcpy(m_pose.vecWorldFromDriverTranslation, ms_headPosition, sizeof(double) * 3U);

                        const glm::vec3 &l_offset = CDriverConfig::GetDesktopOffset();
                        m_pose.vecWorldFromDriverTranslation[0U] += l_offset.x;
                        m_pose.vecWorldFromDriverTranslation[1U] += l_offset.y;
                        m_pose.vecWorldFromDriverTranslation[2U] += l_offset.z;

                        const Leap::Vector l_position = l_hand.palmPosition();
                        const glm::vec3 &l_handOffset = ((m_hand == CH_Left) ? CDriverConfig::GetLeftHandOffset() : CDriverConfig::GetRightHandOffset());
                        m_pose.vecPosition[0] = 0.001f*l_position.x + l_handOffset.x;
                        m_pose.vecPosition[1] = 0.001f*l_position.y + l_handOffset.y;
                        m_pose.vecPosition[2] = 0.001f*l_position.z + l_handOffset.z;

                        const Leap::Vector l_velocity = l_hand.palmVelocity();
                        m_pose.vecVelocity[0] = 0.001f*l_velocity.x;
                        m_pose.vecVelocity[1] = 0.001f*l_velocity.y;
                        m_pose.vecVelocity[2] = 0.001f*l_velocity.z;

                        const Leap::Quaternion l_handOrientation = l_hand.orientation();
                        glm::quat l_rotation(l_handOrientation.w, l_handOrientation.x, l_handOrientation.y, l_handOrientation.z);
                        l_rotation *= ((m_hand == CH_Left) ? g_rotateHalfPiZN : g_rotateHalfPiZ);
                        l_rotation *= ((m_hand == CH_Left) ? CDriverConfig::GetLeftHandOffsetRotation() : CDriverConfig::GetRightHandOffsetRotation());

                        m_pose.qRotation.x = l_rotation.x;
                        m_pose.qRotation.y = l_rotation.y;
                        m_pose.qRotation.z = l_rotation.z;
                        m_pose.qRotation.w = l_rotation.w;
                    } break;
                }

                l_handFound = true;
                m_pose.result = vr::TrackingResult_Running_OK;
                break;
            }
        }
    }

    if(!l_handFound)
    {
        for(size_t i = 0U; i < 3U; i++) m_pose.vecVelocity[i] = .0;
        m_pose.result = vr::TrackingResult_Running_OutOfRange;
    }

    if((m_gameProfile == GP_VRChat) || !CDriverConfig::IsHandsResetEnabled()) l_handFound = true;
    m_pose.poseIsValid = l_handFound;
}

void CLeapController::SetInterfaces(vr::IVRServerDriverHost *f_host, vr::IVRDriverInput *f_input, vr::CVRPropertyHelpers *f_helpers)
{
    ms_driverHost = f_host;
    ms_driverInput = f_input;
    ms_propertyHelpers = f_helpers;
}

void CLeapController::UpdateHMDCoordinates()
{
    vr::TrackedDevicePose_t l_hmdPose;
    ms_driverHost->GetRawTrackedDevicePoses(0.f, &l_hmdPose, 1U); // HMD has device ID 0
    if(l_hmdPose.bPoseIsValid)
    {
        glm::mat4 l_rotMat(1.f);
        ConvertMatrix(l_hmdPose.mDeviceToAbsoluteTracking, l_rotMat);

        const glm::quat l_headRot = glm::quat_cast(l_rotMat);
        ms_headRotation.x = l_headRot.x;
        ms_headRotation.y = l_headRot.y;
        ms_headRotation.z = l_headRot.z;
        ms_headRotation.w = l_headRot.w;

        for(size_t i = 0U; i < 3U; i++) ms_headPosition[i] = l_hmdPose.mDeviceToAbsoluteTracking.m[i][3];
    }
}
