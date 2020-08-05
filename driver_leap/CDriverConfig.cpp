#include "stdafx.h"
#include "CDriverConfig.h"
#include "Utils.h"

extern char g_modulePath[];

unsigned char CDriverConfig::ms_emulatedController = CDriverConfig::EC_Vive;
bool CDriverConfig::ms_leftHand = true;
bool CDriverConfig::ms_rightHand = true;
unsigned char CDriverConfig::ms_orientation = CDriverConfig::OM_HMD;
bool CDriverConfig::ms_skeleton = true;
unsigned char CDriverConfig::ms_trackingLevel = CDriverConfig::TL_Partial;
glm::vec3 CDriverConfig::ms_desktopOffset(0.f, -0.5f, -0.5f);
glm::vec3 CDriverConfig::ms_leftHandOffset(0.f);
glm::quat CDriverConfig::ms_leftHandOffsetRotation(1.f, 0.f, 0.f, 0.f);
glm::vec3 CDriverConfig::ms_rightHandOffset(0.f);
glm::quat CDriverConfig::ms_rightHandOffsetRotation(1.f, 0.f, 0.f, 0.f);
bool CDriverConfig::ms_input = true;
bool CDriverConfig::ms_menu = true;
bool CDriverConfig::ms_applicationMenu = true;
bool CDriverConfig::ms_trigger = true;
bool CDriverConfig::ms_grip = true;
bool CDriverConfig::ms_touchpad = true;
bool CDriverConfig::ms_touchpadTouch = true;
bool CDriverConfig::ms_touchpadPress = true;
bool CDriverConfig::ms_touchpadAxes = true;
bool CDriverConfig::ms_buttonA = true;
bool CDriverConfig::ms_buttonB = true;
bool CDriverConfig::ms_thumbstick = true;
bool CDriverConfig::ms_handsReset = false;

const std::vector<std::string> g_configAttributes
{
    "emulatedController", "leftHand", "rightHand", "orientation", "skeleton", "trackingLevel",
    "desktopOffset", "leftHandOffset", "leftHandOffsetRotation", "rightHandOffset", "rightHandOffsetRotation",
    "input", "menu", "appMenu", "trigger", "grip",
    "touchpad", "touchpadTouch", "touchpadPress", "touchpadAxes",
    "buttonA", "buttonB", "thumbstick",
    "handsReset"
};
enum ConfigParamIndex : size_t
{
    CPI_EmulatedController = 0U,
    CPI_LeftHand,
    CPI_RightHand,
    CPI_Orientation,
    CPI_Skeleton,
    CPI_TrackingLevel,
    CPI_DesktopOffset,
    CPI_LeftHandOffset,
    CPI_LeftHandOffsetRotation,
    CPI_RightHandOffset,
    CPI_RightHandOffsetRotation,
    CPI_Input,
    CPI_Menu,
    CPI_ApplicationMenu,
    CPI_Trigger,
    CPI_Grip,
    CPI_Touchpad,
    CPI_TouchpadTouch,
    CPI_TouchpadPress,
    CPI_TouchpadAxes,
    CPI_ButtonA,
    CPI_ButtonB,
    CPI_Thumbstick,
    CPI_HandsReset
};
const std::vector<std::string> g_orientationModes
{
    "hmd", "desktop"
};
const std::vector<std::string> g_emulatedControllers
{
    "vive", "index"
};
const std::vector<std::string> g_trackingLevels
{
    "partial", "full"
};

const void CDriverConfig::SetLeftHandOffsetEle(short i, const float& v)
{
	switch (i)
	{
		case 0:	ms_leftHandOffset.x = v; break;
		case 1:	ms_leftHandOffset.y = v; break;
		case 2:	ms_leftHandOffset.z = v; break;
	}
}

const void CDriverConfig::SetLeftHandOffsetRotationEle(short i, const float& v)
{
	switch (i)
	{
		case 0:	ms_leftHandOffsetRotation.x = v; break;
		case 1:	ms_leftHandOffsetRotation.y = v; break;
		case 2:	ms_leftHandOffsetRotation.z = v; break;
		case 3:	ms_leftHandOffsetRotation.w = v; break;
	}
}

const void CDriverConfig::SetRightHandOffsetEle(short i, const float& v)
{
	switch (i)
	{
		case 0:	ms_rightHandOffset.x = v; break;
		case 1:	ms_rightHandOffset.y = v; break;
		case 2:	ms_rightHandOffset.z = v; break;
	}
}

const void CDriverConfig::SetRightHandOffsetRotationEle(short i, const float& v)
{
	switch (i)
	{
		case 0:	ms_rightHandOffsetRotation.x = v; break;
		case 1:	ms_rightHandOffsetRotation.y = v; break;
		case 2:	ms_rightHandOffsetRotation.z = v; break;
		case 3:	ms_rightHandOffsetRotation.w = v; break;
	}
}

void CDriverConfig::SaveOffsetsData()
{
	std::string l_path(g_modulePath);
	l_path.erase(l_path.begin() + l_path.rfind('\\'), l_path.end());
	l_path.append("\\..\\..\\resources\\settings.xml");

	pugi::xml_document *l_config = new pugi::xml_document();

	if (l_config->load_file(l_path.c_str()))
	{
		std::stringstream l_handOffset, l_handrotOffset, r_handOffset, r_handrotOffset;
		l_handOffset << ms_leftHandOffset.x << " " << ms_leftHandOffset.y << " " << ms_leftHandOffset.z;
		r_handOffset << ms_rightHandOffset.x << " " << ms_rightHandOffset.y << " " << ms_rightHandOffset.z;
		l_handrotOffset << ms_leftHandOffsetRotation.x << " " << ms_leftHandOffsetRotation.y << " " << ms_leftHandOffsetRotation.z << " " << ms_leftHandOffsetRotation.w;
		r_handrotOffset << ms_rightHandOffsetRotation.x << " " << ms_rightHandOffsetRotation.y << " " << ms_rightHandOffsetRotation.z << " " << ms_rightHandOffsetRotation.w;

		l_config->select_node("//setting[@name='leftHandOffset']").node().attribute("value").set_value(l_handOffset.str().c_str());
		l_config->select_node("//setting[@name='leftHandOffsetRotation']").node().attribute("value").set_value(r_handOffset.str().c_str());
		l_config->select_node("//setting[@name='rightHandOffset']").node().attribute("value").set_value(l_handrotOffset.str().c_str());
		l_config->select_node("//setting[@name='rightHandOffsetRotation']").node().attribute("value").set_value(r_handrotOffset.str().c_str());

		l_config->save_file(l_path.c_str());
	}
}

void CDriverConfig::LoadConfig()
{
    std::string l_path(g_modulePath);
    l_path.erase(l_path.begin() + l_path.rfind('\\'), l_path.end());
    l_path.append("\\..\\..\\resources\\settings.xml");

    pugi::xml_document *l_config = new pugi::xml_document();
    if(l_config->load_file(l_path.c_str()))
    {
        const pugi::xml_node l_root = l_config->child("settings");
        if(l_root)
        {
            for(pugi::xml_node l_node = l_root.child("setting"); l_node; l_node = l_node.next_sibling("setting"))
            {
                const pugi::xml_attribute l_attribName = l_node.attribute("name");
                const pugi::xml_attribute l_attribValue = l_node.attribute("value");
                if(l_attribName && l_attribValue)
                {
                    switch(ReadEnumVector(l_attribName.as_string(), g_configAttributes))
                    {
                        case ConfigParamIndex::CPI_EmulatedController:
                        {
                            size_t l_tableIndex = ReadEnumVector(l_attribValue.as_string(), g_emulatedControllers);
                            if(l_tableIndex != std::numeric_limits<size_t>::max()) ms_emulatedController = static_cast<unsigned char>(l_tableIndex);
                        } break;
                        case ConfigParamIndex::CPI_LeftHand:
                            ms_leftHand = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_RightHand:
                            ms_rightHand = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_Orientation:
                        {
                            size_t l_tableIndex = ReadEnumVector(l_attribValue.as_string(), g_orientationModes);
                            if(l_tableIndex != std::numeric_limits<size_t>::max()) ms_orientation = static_cast<unsigned char>(l_tableIndex);
                        } break;
                        case ConfigParamIndex::CPI_Skeleton:
                            ms_skeleton = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_TrackingLevel:
                        {
                            const size_t l_tableIndex = ReadEnumVector(l_attribValue.as_string(), g_trackingLevels);
                            if(l_tableIndex != std::numeric_limits<size_t>::max()) ms_trackingLevel = static_cast<unsigned char>(l_tableIndex);
                        } break;

                        case ConfigParamIndex::CPI_DesktopOffset:
                        {
                            std::stringstream l_hmdOffset(l_attribValue.as_string());
                            l_hmdOffset >> ms_desktopOffset.x >> ms_desktopOffset.y >> ms_desktopOffset.z;
                        } break;
                        case ConfigParamIndex::CPI_LeftHandOffset:
                        {
                            std::stringstream l_handOffset(l_attribValue.as_string());
                            l_handOffset >> ms_leftHandOffset.x >> ms_leftHandOffset.y >> ms_leftHandOffset.z;
                        } break;
                        case ConfigParamIndex::CPI_LeftHandOffsetRotation:
                        {
                            std::stringstream l_handOffsetRotation(l_attribValue.as_string());
                            l_handOffsetRotation >> ms_leftHandOffsetRotation.x >> ms_leftHandOffsetRotation.y >> ms_leftHandOffsetRotation.z >> ms_leftHandOffsetRotation.w;
                        } break;
                        case ConfigParamIndex::CPI_RightHandOffset:
                        {
                            std::stringstream l_handOffset(l_attribValue.as_string());
                            l_handOffset >> ms_rightHandOffset.x >> ms_rightHandOffset.y >> ms_rightHandOffset.z;
                        } break;
                        case ConfigParamIndex::CPI_RightHandOffsetRotation:
                        {
                            std::stringstream l_handOffsetRotation(l_attribValue.as_string());
                            l_handOffsetRotation >> ms_rightHandOffsetRotation.x >> ms_rightHandOffsetRotation.y >> ms_rightHandOffsetRotation.z >> ms_rightHandOffsetRotation.w;
                        } break;

                        case ConfigParamIndex::CPI_Input:
                            ms_input = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_Menu:
                            ms_menu = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_ApplicationMenu:
                            ms_applicationMenu = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_Trigger:
                            ms_trigger = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_Grip:
                            ms_grip = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_Touchpad:
                            ms_touchpad = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_TouchpadTouch:
                            ms_touchpadTouch = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_TouchpadPress:
                            ms_touchpadPress = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_TouchpadAxes:
                            ms_touchpadAxes = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_ButtonA:
                            ms_buttonA = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_ButtonB:
                            ms_buttonB = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_Thumbstick:
                            ms_thumbstick = l_attribValue.as_bool(true);
                            break;
                        case ConfigParamIndex::CPI_HandsReset:
                            ms_handsReset = l_attribValue.as_bool(false);
                            break;
                    }
                }
            }
        }
    }
    delete l_config;
}
