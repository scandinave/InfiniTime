#pragma once
#include <cstdint>
#include <array>
#define min // workaround: nimble's min/max macros conflict with libstdc++
#define max
#include <host/ble_gap.h>
#include <host/ble_uuid.h>
#undef max
#undef min

/**
 * UUID Apple Notification Center Service : 7905F431-B5CE-4E99-A40F-4B1E122D00D0
 * Value obtains by decoding this uuid to string, reversing the result and prefixing all part with 0x
 */
#define APPLE_NOTIFICATION_CENTER_SERVICE_UUID_BASE                                                                                               \
  { 0xd0, 0x00, 0x2D, 0x12, 0x1E, 0x4B, 0x0F, 0x24, 0x99, 0x0E, 0xCE, 0xB5, 0x31, 0xF4, 0x05, 0x79 }


namespace Pinetime {
    
    namespace System {
        class SystemTask
    }

    namespace Controllers {
        class NotificationManager;

        class AppleNotificationCenterService {
            public:
                AppleNotificationCenterService(Pinetime::System::SystemTask& systemTask, Pinetime::Controllers::NotificationManager& notificationManager);
                void Init();
                
                int OnAlert(uint16_t conn_handle, uint16_t attr_handle, struct ble_gatt_access_ctxt* ctxt);
            private:
               /**
               * see: https://developer.apple.com/library/archive/documentation/CoreBluetooth/Reference/AppleNotificationCenterServiceSpecification/Appendix/Appendix.html#//apple_ref/doc/uid/TP40013460-CH3-SW2
               */
                enum class Category : uint8_t {
                  Other = 0x00,
                  IncomingCall = 0x01,
                  MissedCall = 0x02,
                  Voicemail = 0x03,
                  Social = 0x04,
                  Schedule = 0x05,
                  Email = 0x06,
                  News = 0x07,
                  HealthAndFitness = 0x08,
                  BusinessAndFinance = 0x09,
                  Location = 0x10,
                  Entertainment = 0x11
                };

                enum class Event : uint8_t {
                  NotificationAdded = 0x00,
                  NotificationModified = 0x01,
                  NotificationRemoved = 0x02
                };

                enum class EventFlag : uint8_t {
                  Silent = 0x00,
                  Important = 0x01,
                  PreExisting = 0x02,
                  PositiveAction = 0x03,
                  NegativeAction = 0x04,
                  Schedule = 0x05
                };

                enum class Command : uint8_t {
                  GetNotificationAttributes = 0x00,
                  GetAppAttributes = 0x01,
                  PerformNotificationAction = 0x02
                };

                enum class NotificationAttribute : uint8_t {
                  AppIdentifier = 0x00,
                  Title = 0x01,
                  Subtitle = 0x02,
                  Message = 0x03,
                  MessageSize = 0x04,
                  Date = 0x05,
                  PositiveActionLabel = 0x06,
                  NegativeActionLabel = 0x07
                };

                enum class Action : uint8_t {
                  Positive = 0x00,
                  Negative = 0x01
                };

                enum class AppAttribute : uint8_t {
                  DisplayName = 0x00
                };

                // 9FBF120D-6301-42D9-8C58-25E699A21DBD
                static constexpr ble_uuid128_t ancsChar { {BLE_UUID_TYPE_128}, {0xBD, 0x1D, 0xA2, 0x99,
                                                                                 0xE6, 0x25, 0x58, 0X0C, 0xD9, 0x02,
                                                           0x01, 0x63, 0x0D,0x12, 0xBF, 0x9F}};
                // 69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9
                static constexpr ble_uuid128_t controlPointChar {{BLE_UUID_TYPE_128},
                                                                   {0xD9, 0xD9, 0xAA, 0xFD, 0xBD, 0x9B,
                                                                    0x21, 0X18,0xA8, 0x09,0xE1, 0x45, 0xF3,
                                                                    0xD8, 0xD1, 0x69 }};
                // 22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB
                static constexpr ble_uuid128_t dataSourceChar {{BLE_UUID_TYPE_128},
                                                                   {0xFB, 0x7B, 0x7C, 0xCE, 0x6A, 0xB3,
                                                                    0x44, 0X3E,0xB5, 0x0B,0xD6, 0x24, 0xE9,
                                                                    0xC6, 0xEA, 0x22 }};
                struct ble_gatt_chr_def characteristicDefinition[2];
                struct ble_gatt_svc_def serviceDefinition[2];
                
                Pinetime::System::SystemTask& systemTask;
                NotificationManager& notificationManager;
        };
    }
}