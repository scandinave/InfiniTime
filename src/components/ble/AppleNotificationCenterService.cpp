#include "components/ble/AppleNotificationCenterService.h"
#include <hal/nrf_rtc.h>
#include "components/ble/NotificationManager.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

constexpr ble_uuid128_t AppleNotificationCenterService::ancsSvc;
constexpr ble_uuid128_t AppleNotificationCenterService::ancsChar;
constexpr ble_uuid128_t AppleNotificationCenterService::dataSourceChar;
constexpr ble_uuid128_t AppleNotificationCenterService::controlPointChar;

int AppleNotificationCenterAlertCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  auto ancService = static_cast<AppleNotificationCenterService*>(arg);
  return ancService->OnAlert(conn_handle, attr_handle, ctxt);
}

int AppleNotificationCenterDataCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  auto ancService = static_cast<AppleNotificationCenterService*>(arg);
  return ancService->OnData(conn_handle, attr_handle, ctxt);
}

void AppleNotificationCenterService::Init() {
  int res;
  res = ble_gatts_count_cfg(serviceDefinition);
  ASSERT(res == 0);

  res = ble_gatts_add_svcs(serviceDefinition);
  ASSERT(res == 0);
}

AppleNotificationCenterService::AppleNotificationCenterService(System::SystemTask& systemTask, NotificationManager& notificationManager)
  : characteristicDefinition {
    {
      .uuid = &ancsChar.u, 
      .access_cb = AppleNotificationCenterAlertCallback, 
      .arg = this, 
      .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_ENC | BLE_GATT_CHR_F_WRITE_AUTHOR
    },
    { 
      .uuid = &dataSourceChar.u, 
      .access_cb = AppleNotificationCenterDataCallback, 
      .arg = this, 
      .flags = BLE_GATT_CHR_F_NOTIFY | BLE_GATT_CHR_F_WRITE_ENC | BLE_GATT_CHR_F_WRITE_AUTHOR
    },
    {0}
  },
  serviceDefinition {
    {/* Device Information Service */
      .type = BLE_GATT_SVC_TYPE_PRIMARY,
      .uuid = &ancsSvc.u,
      .characteristics = characteristicDefinition
    },
    {0},
  },
  systemTask {systemTask},
  notificationManager {notificationManager} {
}

// Handle notification
int AppleNotificationCenterService::OnAlert(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt) {
  if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
    Event event;
    EventFlag eventFlag;
    Category category;
    size_t categoryCount;
    uint32_t notificationUUID;

    NotificationManager::Notification notif;
    os_mbuf_copydata(ctxt->om, 0, 1, &event);
    os_mbuf_copydata(ctxt->om, 1, 1, &eventFlag);
    os_mbuf_copydata(ctxt->om, 2, 1, &category);
    os_mbuf_copydata(ctxt->om, 3, 1, &categoryCount);
    os_mbuf_copydata(ctxt->om, 4, 4, &notificationUUID);

    switch (category) {
      case Category::Email:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::Email;
        break;
      case Category::IncomingCall:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::IncomingCall;
        break;
      case Category::MissedCall:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::MissedCall;
        break;
      case Category::News:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::News;
        break;
      case Category::Schedule:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::Sms;
        break;
      case Category::Voicemail:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::VoiceMail;
        break;
      default:
        notif.category = Pinetime::Controllers::NotificationManager::Categories::SimpleAlert;
        break;
    }

    notif.message = std::array<char, 101> {"Hello\0World"};
    notif.size = 11;

    auto notifEvent = Pinetime::System::Messages::OnNewNotification;
    notificationManager.Push(std::move(notif));
    systemTask.PushMessage(notifEvent);
 
  }
  return 0;
}

// Handle data from dataSourceChar
int AppleNotificationCenterService::OnData(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt) {
  return 0;
}

// Send a Get Notification Attributes command to Control Point
void AppleNotificationCenterService::GetNotificationAttribute() {}
// Send a Get App Attributes command to Control Point
void AppleNotificationCenterService::GetAppAttributes() {}
// Send a Perform Notification Action command to Control Point
void AppleNotificationCenterService::PerformNotificationAction() {}

