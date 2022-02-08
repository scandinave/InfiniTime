#include "components/ble/AppleNotificationCenterService.h"
#include <hal/nrf_rtc.h>
#include "components/ble/NotificationManager.h"
#include "systemtask/SystemTask.h"

using namespace Pinetime::Controllers;

constexpr ble_uuid128_t AppleNotificationCenterService::ancsChar;
constexpr ble_uuid128_t AppleNotificationCenterService::dataSourceChar;
constexpr ble_uuid128_t AppleNotificationCenterService::controlPointChar;

int AppleNotificationCenterCallback(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt, void* arg) {
  auto ancService = static_cast<AppleNotificationCenterService*>(arg);
  return ancService->OnAlert(conn_handle, attr_handle, ctxt);
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
    {.uuid = &ancsChar.u, .access_cb = AppleNotificationCenterCallback, .arg = this, .flags = BLE_GATT_CHR_F_NOTIFY},
    {.uuid =  0}},
    serviceDefinition {
      {/* Device Information Service */
       .type = BLE_GATT_SVC_TYPE_PRIMARY,
       .uuid = &ancsChar.u,
       .characteristics = characteristicDefinition
      },
      {0},
    },
    systemTask {systemTask},
    notificationManager {notificationManager} {}

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
    os_mbuf_copydata(ctxt->om, 4, 32, &notificationUUID);
  }
  return 0;
}